#include "urlserver_client.h"

using std::vector;
using std::string;

using namespace radar;

UrlServClient::UrlServClient(const std::string &serv_addr) : ThriftClient(serv_addr)
{
    int ret = CheckConnect();
    if (ret) {
        ret = TransportOpen();
        if (ret) {
            FATAL("UrlServClient::UrlServClient TransportOpen fail");
        }   
    }   
    client = new (std::nothrow) UrlServiceClient(m_protocol);
    if (!client) {
        FATAL("new UrlServiceClient fail");
    }
}

UrlServClient::~UrlServClient()
{
    if (client) {
        delete client;
        client = NULL;
    }   
    int ret = CheckConnect();
    if (!ret) {
        ret = TransportClose();
        if (ret) {
            FATAL("UrlServClient::~UrlServClient() call ThriftClient::TransportClose() fail");
        }   
    }   
}

int UrlServClient::getUrls(std::vector<Url> & _return, const int32_t num)
{
    try{
        client->GetUrls(_return,num);
    }catch(const TException &e){
        FATAL("UrlServClient run GetUrls error: %s", e.what());
        return -1;
    }
    return 0;
}

int UrlServClient::sendFailedUrls(const std::vector<FailedUrl> & urls)
{
    try{
       client->SendFailedUrls(urls);
    }catch(const TException &e){
        FATAL("UrlServClient run SendFailedUrls error: %s", e.what());
        return -1; 
    }   
    return 0;
}

int UrlServClient::sendParserUrls(const std::vector<ParserUrl> &urls)
{
    try{
        return client->SendParserUrls(urls);
    }catch(const TException &e) {
        FATAL("UrlServClient run sendParserUrls error: %s", e.what());
        return -1;
    }
}



