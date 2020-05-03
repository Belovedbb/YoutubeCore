#include "tube.h"
#include <iostream>
using namespace std;

Tube::Tube(const string& url, bool extract_dependencies) : YoutubeInterface(url), url(url) {
    init_network_proc();
    if(extract_dependencies){
        this->extract_dependencies = extract_dependencies;
        extractor = ExtractYoutube(url).extract_details();
    }
}

YoutubeConstants Tube::download(const string filename) {
    Download download(this->download_tags, 2, filename);
    download.download_to_file();
    return YoutubeConstants::DOWNLOAD_ABORTED;
}

void Tube::init_network_proc() {
    //set cookie jar
}

Tube &Tube::extracts() {
    if(!extract_dependencies)
        extractor = ExtractYoutube(url).extract_details();
    this->download_tags = extractor.get_details();
    this->vid_details = extractor.get_general_details();
    return *this;
}

const tuple<string,string,string, bool> &Tube::get_extractor_container() const {
    return extractor_container;
}

