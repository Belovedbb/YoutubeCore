//
// Created by Beloved on 20-Jan-20.
//

#ifndef YOUTUBECORE_YOUTUBE_INTERFACE_H
#define YOUTUBECORE_YOUTUBE_INTERFACE_H

#include <string>

namespace tubby{
    using std::string;

    enum class YoutubeConstants{
        DOWNLOAD_IN_PROGRESS,
        DOWNLOAD_ABORTED,
        DOWNLOAD_COMPLETED
    };

    class YoutubeInterface{
    private:
        const string url;
        int num_of_downloads;
        float timeout;
        //-------optionals-------
        //cookiefile
        //socks protocol
        //proxy
        //debug headers

    public:
        explicit YoutubeInterface(const string& youtube_url) : url(youtube_url){}
        virtual YoutubeConstants download(const string filename) = 0;
        virtual ~YoutubeInterface(){};
    };


};

#endif //YOUTUBECORE_YOUTUBE_INTERFACE_H
