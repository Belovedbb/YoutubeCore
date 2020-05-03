#include "downloader.h"

namespace downloader{

    void sanitize(std::string &stringValue){
        for (auto i = stringValue.begin();;) {
            auto const pos = std::find_if(
                i, stringValue.end(),
                [](char const c) { return '\\' == c || '\'' == c || '"' == c; }
            );
            if (pos == stringValue.end()) {
                break;
            }
            i = std::next(stringValue.insert(pos, '\\'), 2);
        }
    
        stringValue.erase(
            std::remove_if(
                stringValue.begin(), stringValue.end(), [](char const c) {
                    return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c;
                }
            ),
            stringValue.end()
        );
    }

    void append_random_characters(string& file_name){
        const int range_from_lower = 65, range_to_lower = 90;
        const int range_from_upper = 97, range_to_upper = 122;
        const int word_range = 6;
        random_device rand_dev;
        mt19937 generator(rand_dev());
        uniform_int_distribution<int>  distr_lower(range_from_lower, range_to_lower);
        uniform_int_distribution<int>  distr_upper(range_from_upper, range_to_upper);
        uniform_int_distribution<int>  dispatcher_distr(0, 1);
        int generated = 0;
        for(int i = 0; i < word_range; i++){
            int generated_dispatch = dispatcher_distr(generator);
            if(generated_dispatch == 0)
                generated = distr_lower(generator);
            else
                generated = distr_upper(generator);
            file_name.push_back(static_cast<char>(generated));
        }
    }

    string generic_attr_get(const vector<string*> itag, const string key){
        auto mime_iterator = find_if(itag.begin(), itag.end(), [&key](string* ele){
            return ele[0] == key;
        });
        return (*mime_iterator)[1];
    }

    string get_mime_type(const vector<string*>& itag){
        string ext = ".";
        string mime = generic_attr_get(itag, "ext");
        ext.append(mime);
        return ext;
    }

    string generate_file_name(const vector<string*>& itag, string file_name){
        string mime_ext = get_mime_type(itag);
        sanitize(file_name);
        //append_random_characters(file_name);
        file_name.append(mime_ext);
        return file_name;
    }

    Download::Download(vector<vector<string*>>& specs_container, int spec_index , string file_name, bool use_range ) :
        specs_container(specs_container), sp(spec_index), range_enabled(use_range){
            vector<string*> itag_container = specs_container.at(spec_index);
            this->file_name_ext = generate_file_name(itag_container, file_name);
        }

    void Download::download_to_file(){
        const vector<string*>& container = specs_container.at(sp);
        uint64_t range_start = 0, range_end = 0, temp_incrementer = 0;
        if(range_enabled){
            range_end = stoull(generic_attr_get(container, "file_size"));
        }
        
        string url = generic_attr_get(container, "url");
        url = "https://" + url.substr(url.find("r2"), url.size());
        auto parsed_url = parse_url(url, false).first;
        string authority = get<1>(parsed_url);
        int find_beg = url.find(authority) + authority.size();
        string y_url_authority= url.substr(find_beg , url.size());
        SSLClient cli(authority);
        cli.set_ca_cert_path("./lib/ca-bundle.crt");
        cli.enable_server_certificate_verification(true);
        Headers headers;
        uint64_t slice_factor = range_end / 10 > 10 ? range_end / 10 : range_end;
        temp_incrementer = slice_factor;
        std::remove(file_name_ext.c_str());
        ofstream file(file_name_ext, std::ios_base::app | std::ios_base::binary);
        for(range_start; range_start <= range_end; ){
            headers.insert( make_range_header({{range_start, temp_incrementer}}) );
            auto res = cli.Get(y_url_authority.c_str(), headers, 
            [&](const char* data, uint64_t data_length) {
                    file<<string(data);
                    return true;
                }
            );
            if(temp_incrementer == range_end)
                break;
            range_start += slice_factor;
            temp_incrementer += slice_factor;
            if(temp_incrementer > range_end){
                temp_incrementer = range_end;
            }
        }
        file.close();

    }

};