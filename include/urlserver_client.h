#ifndef  __URLSERVER_CLIENT_H_
#define  __URLSERVER_CLIENT_H_

#include <vector>
#include <string>

#include "thrift_client.h"
#include "UrlService.h"
#include "log.h"

class UrlServClient : public ThriftClient
{
public:
    explicit UrlServClient(const std::string &serv_addr);
    ~UrlServClient();

    int getUrls(std::vector<radar::Url> & _return, const int32_t num);
    int sendFailedUrls(const std::vector<radar::FailedUrl> & urls);
    int sendParserUrls(const std::vector<radar::ParserUrl> & urls);
    //int nofityFailedUrls(const std::vector<FailedUrl> & urls);

public:
    radar::UrlServiceClient * client;
};

#endif

