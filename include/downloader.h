#ifndef YOUTUBECORE_DOWNLOADER_H
#define YOUTUBECORE_DOWNLOADER_H

#include <vector>
#include <string>
#include <cstddef>
#include <algorithm>
#include <random>
#include <iterator>
#include <fstream>
#include <tuple>
#include "common.h"
#include "parse_url.h"

#include <iostream>
using namespace std;


namespace downloader{
    using std::vector;
    using std::string;
    using std::random_device;
    using std::uniform_int_distribution;
    using std::mt19937;
    using std::ofstream;
    using std::get;
    using std::stoull;
    using httplib::SSLClient;
    using url_construct::parse_url;
    using std::find_if;
    using std::next;
    using httplib::Headers;
    using httplib::make_range_header;

    
    class Download{
        private:
            bool range_enabled;
            string file_name_ext;
            long data_size;
            short buffer_size;
            const vector<vector<string*>>& specs_container;
            int sp;
        public:
            Download(vector<vector<string*>>& specs_container, int spec_index, string file_name = "", bool use_range = true);
            void download_to_file();
    };
}

#endif