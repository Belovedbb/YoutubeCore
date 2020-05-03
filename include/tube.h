#ifndef TUBE_H
#define TUBE_H

#include <string>
#include <vector>
#include <tuple>
#include <map>
#include "extracts.h"
#include "downloader.h"
#include "youtube_interface.h"


using std::string;
using std::tuple;
using std::vector;
using std::map;

using tubby::YoutubeConstants ;
using tubby::YoutubeInterface;
using extractor::ExtractYoutube;

using downloader::Download;

class Tube: public YoutubeInterface{
    private:
        string url;
        bool extract_dependencies;
        tuple<string, string, string, bool> extractor_container;
        void init_network_proc();
        ExtractYoutube extractor = ExtractYoutube("");
        vector<vector<string*>> download_tags;
        map<string, string> vid_details;
    public:
        Tube(const string& url, bool extract_dependencies = true);
        Tube& extracts();
        YoutubeConstants download(const string filename);
        const tuple<string,string,string, bool> &get_extractor_container() const;

};

#endif