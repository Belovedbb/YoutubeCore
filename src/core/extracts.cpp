//
// Created by Beloved on 22-Jan-20.
//

#include <extracts.h>
#include <iostream>
using namespace std;
namespace extractor {
    const string YOUTUBE_URL_PATTERN = R"(((?:https?://|//)(?:(?:(?:(?:\w+\.)?[yY][oO][uU][tT][uU][bB][eE](?:-nocookie|kids)?\.com/|(?:www\.)?deturl\.com/www\.youtube\.com/|(?:www\.)?pwnyoutube\.com/|(?:www\.)?hooktube\.com/|(?:www\.)?yourepeat\.com/|tube\.majestyc\.net/|youtube\.googleapis\.com/)(?:.*?\#/)?(?:(?:(?:v|embed|e)/(?!videoseries))|(?:(?:(?:watch|movie)(?:_popup)?(?:\.php)?/?)?(?:\?|\#!?)(?:.*?[&;])??v=)))|(?:youtu\.be|vid\.plus|zwearz\.com/watch|)/|(?:www\.)?cleanvideosearch\.com/media/action/yt/watch\?videoId=))?([0-9A-Za-z_-]{11})(?!.*?\blist=(?:(?:PL|LL|EC|UU|FL|RD|UL|TL|PU|OLAK5uy_)[0-9A-Za-z-_]{10,}s|WL)))";
    const string CODEC_PATTERN = R"(([a-zA-Z_-]+)=(["\']?)(.+?)(:;|$))";
    const string THUMBNAIL_PATTERN = R"(.*?href=\"(.*?)\">)";
    map<string , string> vid_info;
    vector<string> list_mpd;
    string y_embed_page;
    
bool contains_ele(const std::vector<std::string>& list, const std::string& value){
    for(auto iter = list.begin(); iter < list.end(); iter++){
        if(value != *iter)
            continue;
        else
            return true;
    }
    return false;
}

string parse_youtube_thumbnail(string y_url_page, string beg_string, string end_string){
    int find_beg = y_url_page.find(beg_string);
    y_url_page = y_url_page.substr(find_beg + beg_string.size(), y_url_page.size());
    y_url_page = y_url_page.substr(0, y_url_page.find(end_string) + end_string.size());
    return y_url_page;
}

void parse_yt_setting(JsonParser& json_object, string y_url_page, string beg_string, string end_string){
    int find_beg = y_url_page.find(beg_string);
    y_url_page = y_url_page.substr(find_beg + beg_string.size(), y_url_page.size());
    y_url_page = y_url_page.substr(0, y_url_page.size() - (y_url_page.size() - y_url_page.find(end_string)));
    json_object.Parse(y_url_page.c_str());
}

    string get_mime_type(string str, string type){
        if(str.empty())
            return "";
            if(str == "audio/mp4")
                return "m4a";
            else if( str == "audio/mpeg")
                return "mp3";
        map<string, string> format = {
            {"3gpp", "3gp"},
            {"smptett+xml", "tt"},
            {"ttaf+xml", "dfxp"},
            {"ttml+xml", "ttml"},
            {"x-flv",  "flv"},
            {"x-mp4-fragmented", "mp4"},
            {"x-ms-sami", "sami"},
            {"x-ms-wmv",  "wmv"},
            {"mpegurl", "m3u8"},
            {"x-mpegurl",  "m3u8"},
            {"vnd.apple.mpegurl", "m3u8"},
            {"dash+xml",  "mpd"},
            {"f4m+xml", "f4m"},
            {"hds+xml", "f4m"},
            {"vnd.ms-sstr+xml",  "ism"},
            {"quicktime", "mov"},
            {"mp2t", "ts"}                     
        };
        string type_str = "";
        try{
            type_str = format.at("type");
        }catch(std::out_of_range& ex){
            return type;
        }
            
        return type_str;
    }

    ExtractYoutube ExtractYoutube::extract_details(string protocol) {
        this->protocol = protocol == "" ? "https" : "http";
        auto parsed_url = parse_url(url, true);
        string vid_id = extract_vid_url();
        string y_url = url = "/watch?v="+ vid_id +"&gl=US&hl=en&has_verified=1&bpctr=9999999999";
        const string y_url_page_original = this->download_page(y_url.c_str());
        string y_url_page = y_url_page_original;
        //TODO handle download for embeded vids
        if(!regex_search(y_url_page, regex(R"(player-age-gate-content\">)"))){
            JsonParser yt_setting;
            parse_yt_setting(yt_setting, y_url_page, ";ytplayer.config = ", ";ytplayer");
            JsonValue& yt_settings_args = yt_setting["args"];
            JsonValue& player_res = yt_setting["args"]["player_response"];
            string player_res_text = player_res.GetString();
            yt_setting.Parse(player_res_text.c_str());
            JsonValue& player_res_mpd = yt_setting["streamingData"].get_value("dashManifestUrl");
            if((!player_res_mpd.IsNull() && player_res_mpd.IsString() ) && !contains_ele(list_mpd, player_res.GetString())){
                list_mpd.push_back(player_res_mpd.GetString());
            }
            JsonValue& vid_detail = yt_setting.get_value("videoDetails");
            string title = vid_detail.get_value("title").GetString();
            general_details["title"] = title;
            string short_description = vid_detail.get_value("shortDescription").GetString();
            general_details["description"] = short_description;
            string view_count = vid_detail.get_value("viewCount").GetString();
            general_details["view_count"] = view_count;
            string is_live = to_string((int)vid_detail.get_value("isLiveContent").GetBool());
            general_details["is_live"] = is_live;
            string average_rating = to_string(vid_detail.get_value("averageRating").GetDouble());
            general_details["average_rating"] = average_rating;
            string author = vid_detail.get_value("author").GetString();
            general_details["author"] = author;
            string y_url_page_thumb = y_url_page_original;
            string rg_thumb_str = parse_youtube_thumbnail(y_url_page_thumb, "<span itemprop=\"thumbnail\"", ".jpg\">");
            regex thumb_r = regex(THUMBNAIL_PATTERN);
            smatch match_thumbnail;
            bool thumb_r_search = regex_search(rg_thumb_str, match_thumbnail, thumb_r);
            string thumb_url_ = thumb_r_search ? match_thumbnail[1].str() : vid_detail.get_value("thumbnail").GetString();
            general_details["thumbnail_url"] = thumb_url_;
            
            JsonValue& streaming_format = yt_setting.get_value("streamingData").get_value("formats");
            JsonValue& streaming_format_extend = yt_setting.get_value("streamingData").get_value("adaptiveFormats");
            if(is_live != "true"){
                map<string, vector<pair<string, string>> > formats;
                auto array1 = streaming_format.GetArray();
                auto array2 = streaming_format_extend.GetArray();
                for (auto& streamNode : array1){
                    vector<string*> fmt_spec = {};
                    JsonValue& itag_value = streamNode;
                    string itag_name = to_string(itag_value.get_value("itag").GetInt());
                    if(!itag_value.IsNull()){
                        formats = {};
                        JsonValue& mime_type = itag_value.get_value("mimeType");
                        string quality = itag_value.get_value("quality").GetString();
                        string fmt_audio_sample_rate[] = {"audio_sample_rate", itag_value.get_value("audioSampleRate").GetString()};
                        string fmt_file_size[] = {"file_size", itag_value.get_value("contentLength").GetString()};
                        string fmt_width[] = {"width", to_string(itag_value.get_value("width").GetInt())};
                        string fmt_height[] = {"height", to_string(itag_value.get_value("height").GetInt())};
                        string fmt_quality_note[] = {"format_label", std::move(fmt_quality_note[1])};
                        string fmt_bitrate[] = {"bitrate", itag_name != "43" ? to_string(itag_value.get_value("bitrate").GetInt()) : ""};
                        string itag_download_url[] = {"url", itag_value.get_value("url").GetString()};
                        string format_id[] = {"format_id", itag_name};
                        string mime_type_container[] = {"mime_type", mime_type.GetString()};
                    
                        
                        fmt_spec.push_back(fmt_audio_sample_rate);
                        fmt_spec.push_back(fmt_file_size);
                        fmt_spec.push_back(fmt_width);
                        fmt_spec.push_back(fmt_height);
                        fmt_spec.push_back(fmt_quality_note);
                        fmt_spec.push_back(fmt_bitrate);
                        fmt_spec.push_back(format_id);
                        fmt_spec.push_back(itag_download_url);
                        fmt_spec.push_back(mime_type_container);
                        
                        auto itag_url_data  = parse_url(itag_download_url[1], true).second;

                        if(!mime_type.IsNull()){
                            auto splitted_mime_type = url_space::ParseUrl::split(mime_type_container[1], ';');
                            auto type_vid = url_space::ParseUrl::split(splitted_mime_type.front(), '/');
                            if(type_vid.size() == 2){
                                string type_vid_first = type_vid.front();
                                string type_vid_second = type_vid.back();
                                string ext[] = {"ext", type_vid_second};
                                fmt_spec.push_back(ext);
                                string mime_type_selector[] = {"mts", get_mime_type(splitted_mime_type.front(), type_vid_second)};
                                if(type_vid_first == "audio" || type_vid_first == "video"){
                                    regex r = regex(CODEC_PATTERN);
                                    smatch matches;
                                    bool r_search = regex_search(mime_type_container[1], matches, r);
                                    if(r_search && matches[1].str() == "codecs"){
                                        string codecs[] = {"codecs", matches[3].str()};
                                        fmt_spec.push_back(codecs);
                                    }
                                }
                                
                            }
                        }
                    }
                    all_formats.push_back(fmt_spec);
                }
                for(auto& array: array2){
                    vector<string*> fmt_spec = {};
                    JsonValue& itag_value = array;
                    string itag_name = to_string(itag_value.get_value("itag").GetInt());
                    if(!itag_value.IsNull()){
                        formats = {};
                        JsonValue& mime_type = itag_value.get_value("mimeType");
                        string quality = itag_value.get_value("quality").GetString();
                        if(!itag_value.get_value("audioSampleRate").IsNull()){
                            string fmt_audio_sample_rate[] = {"audio_sample_rate", itag_value.get_value("audioSampleRate").GetString()};
                            fmt_spec.push_back(fmt_audio_sample_rate);
                        }
                        string fmt_file_size[] = {"file_size", itag_value.get_value("contentLength").GetString()};
                        if(!itag_value.get_value("width").IsNull()){
                            string fmt_width[] = {"width", to_string(itag_value.get_value("width").GetInt())};
                            fmt_spec.push_back(fmt_width);
                        }
                        if(!itag_value.get_value("height").IsNull()){
                            string fmt_height[] = {"height", to_string(itag_value.get_value("height").GetInt())};
                            fmt_spec.push_back(fmt_height);
                        }
                        string fmt_quality_note[] = {"format_label", std::move(fmt_quality_note[1])};
                        string fmt_bitrate[] = {"bitrate", itag_name != "43" ? to_string(itag_value.get_value("bitrate").GetInt()) : ""};
                        string itag_download_url[] = {"url", itag_value.get_value("url").GetString()};
                        string format_id[] = {"format_id", itag_name};
                        string mime_type_container[] = {"mime_type", mime_type.GetString()};
                    
                        fmt_spec.push_back(fmt_file_size);
                        fmt_spec.push_back(fmt_quality_note);
                        fmt_spec.push_back(fmt_bitrate);
                        fmt_spec.push_back(format_id);
                        fmt_spec.push_back(itag_download_url);
                        fmt_spec.push_back(mime_type_container);
                        
                        auto itag_url_data  = parse_url(itag_download_url[1], true).second;

                        if(!mime_type.IsNull()){
                            auto splitted_mime_type = url_space::ParseUrl::split(mime_type_container[1], ';');
                            auto type_vid = url_space::ParseUrl::split(splitted_mime_type.front(), '/');
                            if(type_vid.size() == 2){
                                string type_vid_first = type_vid.front();
                                string type_vid_second = type_vid.back();
                                string ext[] = {"ext", type_vid_second};
                                fmt_spec.push_back(ext);
                                string mime_type_selector[] = {"mts", get_mime_type(splitted_mime_type.front(), type_vid_second)};
                                if(type_vid_first == "audio" || type_vid_first == "video"){
                                    regex r = regex(CODEC_PATTERN);
                                    smatch matches;
                                    bool r_search = regex_search(mime_type_container[1], matches, r);
                                    if(r_search && matches[1].str() == "codecs"){
                                        string codecs[] = {"codecs", matches[3].str()};
                                        fmt_spec.push_back(codecs);
                                    }
                                }
                                
                            }
                        }
                    }
                    all_formats.push_back(fmt_spec);
                }
                
            }
        }
        return *this;
    }

    string ExtractYoutube::extract_vid_url() {
        regex r = regex(YOUTUBE_URL_PATTERN);
        smatch matches;
        bool r_search = regex_search(url, matches, r);
        if(r_search)
            return matches[2].str();
        return "";
    }

    //TODO add  Extract fields
    ExtractYoutube ExtractYoutube::operator=(const ExtractYoutube& obj_extract) {
        this->url = obj_extract.url;
        this->protocol = obj_extract.protocol;
        for(auto i = obj_extract.all_formats.begin(); i != obj_extract.all_formats.end(); i++){
            this->all_formats.push_back(*i);
        }
        for(auto i = obj_extract.general_details.begin(); i != obj_extract.general_details.end(); i++){
            this->general_details.insert(*i);
        }
        return *this;
    }

    string ExtractYoutube::download_page(const char* url_page) {
        std::string readBuffer;
        httplib::SSLClient cli("youtube.com");
        cli.set_ca_cert_path("./lib/ca-bundle.crt");
        cli.enable_server_certificate_verification(true);
        //httplib::Headers headers = {"Accept-Encoding", "gzip, deflate" };
        string body;
        auto res = cli.Get("/", 
            [&](const char* data, uint64_t data_length) {
                body.append(data, data_length);
                return true;
            });

        assert(res->body.empty());

        if (res && res->status == 301) {
                cli.set_follow_location(true);
                res = cli.Get(url_page);
                body = string(res->body);
        }

        return body;
    }

    vector<vector<string* >> ExtractYoutube::get_details(){
        return this->all_formats;
    }

    map<string, string> ExtractYoutube::get_general_details(){
        return this->general_details;
    }

};