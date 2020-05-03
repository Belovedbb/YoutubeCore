#ifndef PARSE_URL_H
#define PARSE_URL_H

#include <list>
#include <map>
#include <iterator>
#include <tuple>
#include <memory>
#include <utility>
#include "split_url.h"
#include "helper.h"

namespace url_space {
    using std::list;
    using std::map;
    using std::tuple;
    using std::copy;
    using std::advance;
    using std::make_tuple;
    using url_space::Token;
    using url_space::URLCODE;


    class ParseUrl {
    private:
        string  scheme, authority, path, query, fragment;
        list<Token>::iterator internal_iter;
        list<Token> url_tokens;
        bool match(URLCODE code);
        void parse();
        bool is_literal();
        bool is_end();
        Token peek_next(int);
        Token peek_prev(int);
        string decode_url(const string& str);
    public:
        explicit ParseUrl(const list<Token>& tokenized_url);
        tuple<string, string, string, string, string> parse_url();
        map<string, string> parse_query(string& query_part);
        static list<string> split(string str, char delimiter = ' ', 
            list<string>& splitters = RefInit<list<string>>(list<string>{})) {
            int current_pos = 0, prev_pos = 0;
            current_pos = str.find(delimiter, prev_pos);
            if(current_pos != -1) {
                splitters.push_back(str.substr(prev_pos, current_pos));
                split(str.substr(current_pos + 1, str.size()), delimiter, splitters);
            }
            else
                splitters.push_back(str);

            return splitters;
    }
    };

};

namespace url_construct{
    using url_space::TokenizeUrl;
    using url_space::Token;
    using url_space::ParseUrl;
    using std::string;
    using std::tuple;
    using std::get;
    using std::map;
    using std::unique_ptr;
    using std::pair;
    using std::make_pair;

    //Return a 6-tuple: (scheme, netloc, path, params, query, fragment).
    pair<tuple<string, string, string, string, string>, map<string, string>> parse_url(const string& url, bool parse_query = false);
};

#endif