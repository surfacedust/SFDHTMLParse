#ifndef SFD_PARSESTD_H
#define SFD_PARSESTD_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>

namespace sfd {


enum EnumExpandWay {
    BY_SEED_DIRECTORY = 0,
    BY_SEED_ROOT = 1,
    BY_SEED_DOMAIN = 2,
    BY_NOLIMIT = 3
};

enum EnumParseStatus {
    OK = 0,
    FILTERED,
    URL_ERROR,
    AD_PASS,
    CONTENT_ERROR
};

//URL
struct url_t{
    std::string url;
    int64_t seed_id;
    int64_t parent_id;
    int64_t id;
    int32_t depth;
    double weight;
    EnumExpandWay expand_way;
    bool operator == (const url_t& _url)
    {
        return url == _url.url;
    }

};

//HTML
struct html_t{
    url_t url;
    std::string html;
};


// Thread
struct thread_stat_t {
    pthread_t pid;
};

} //namespace
#endif
