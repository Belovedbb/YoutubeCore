//
// Created by Beloved on 22-Jan-20.
//

#ifndef YOUTUBECORE_EXTRACTS_H
#define YOUTUBECORE_EXTRACTS_H

#include <string>
#include <regex>
#include <map>
#include <vector>
#include <algorithm>
#include <utility>
#include <rapidjson/document.h>
#include "parse_url.h"
#include "common.h"

namespace extractor{

using std::string;
using std::regex;
using std::smatch;
using std::map;
using std::to_string;
using std::vector;
using std::pair;
using std::make_pair;
using std::regex_search;
using url_construct::parse_url;
//using url_space::
using JsonParser =  rapidjson::Document;
using JsonValue  = rapidjson::Value;

    class ExtractYoutube{
        private:
            string url, protocol;
            string extract_vid_url();
            string download_page(const char * url_page);
            vector<vector<string*>> all_formats ;
            map<string, string> general_details;
        public:
            explicit ExtractYoutube(const string& url):
             url(url), all_formats({}), general_details({}){
            }
            ExtractYoutube(const ExtractYoutube& )= default;
            ExtractYoutube operator=(const ExtractYoutube&);
            ExtractYoutube extract_details(string protocol = "");
            vector<vector<string* >> get_details();
            map<string, string> get_general_details();
    };
};
#endif //YOUTUBECORE_EXTRACTS_H
