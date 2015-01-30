#ifndef SFD_STORE_H
#define SFD_STORE_H

#include "url_types.h"
#include "urlserver_client.h"

#include "sfd_parsestd.h"
#include "sfd_parser.h"
#include "sfd_html_parse_config.h"

#include <vector>

namespace sfd {

enum EnumStoreRW {
    R  = 0x1,
    WU = 0x2,
    WT = 0x4,
};

class Store
{
public:
    Store() {_rw_switch = 0;}
    virtual ~Store() {}
    virtual int readHTML(html_t &html) { return 0;}
    virtual int writeURLS(const html_t &html, const std::vector<url_t> &urls) { return 0;}
    virtual int writeTEXTS(const html_t &html, const std::vector<text_t> &texts) { return 0;}

    void setRW(int rw_switch) { _rw_switch = rw_switch;}
    bool readEnabled() {
        return _rw_switch & 0x1;
    }

    bool writeURLSEnabled() {
        return _rw_switch & 0x2;
    }

    bool writeTEXTSEnabled() {
        return _rw_switch & 0x4;
    }

    int _rw_switch;
};


// HtmlStore 目前只负责从Crawl的循环队列中获取HTML
// 故只实现readHTML 函数，不实现 writeURLS/writeTEXTS 函数
class CrawlStore :public Store {
public:
    CrawlStore(int rw_switch = R);
    virtual ~CrawlStore();
    virtual int readHTML(html_t &html);
    virtual int writeURLS(const html_t &html, const std::vector<url_t> &urls) {return 0;}
    virtual int writeTEXTS(const html_t &html, const std::vector<text_t> &texts) {return 0;}
};

// ThriftStore目前只负责发送URLS至UrlServer
// 故只实现writeURLS 函数， 不实现 readHTML/writeTEXTS 函数
class ThriftStore :public Store {
public:
    ThriftStore();
    ThriftStore(const char *host_port);
    bool CheckConnect();
    virtual ~ThriftStore();
    virtual int readHTML(html_t &html) {return 0;}
    virtual int writeURLS(const html_t &html, const std::vector<url_t> &urls);
    virtual int writeTEXTS(const html_t &html, const std::vector<text_t> &texts) {return 0;}
private:
    UrlServClient* _url_serv_client;
    std::vector <radar::ParserUrl> _vParserUrl;
};

class LocalStore :public Store {
public:
    LocalStore(char *path = NULL, int rw_switch = R|WU|WT);
    virtual ~LocalStore() {}
    void local_store_init(const char *path);
    virtual int readHTML(html_t &html);
    virtual int writeURLS(const html_t &html, const std::vector<url_t> &urls);
    virtual int writeTEXTS(const html_t &html, const std::vector<text_t> &texts);
private:
    void getFileNameWithPath(char* fileName, size_t len, const char* infixPath, const char* suffixFile);
    void createDir(const char* dir);
    // path/html
    // path/urls/$time_$url.txt 该url下提取到的urls
    // path/texts/$time_$url.txt 该url下提取到的正文
    char _path[MAX_CHAR_SIZE];
};

Store* create_log_store(store_log_conf_t &conf);
Store* create_hbase_store(store_hbase_conf_t &conf);
Store* create_thrift_store(store_thrift_conf_t &conf);
Store* create_mysql_store(store_mysql_conf_t &conf);
Store* create_local_store(store_local_conf_t &conf);
Store* create_crawl_store(store_crawl_conf_t &conf);
} //namespace
#endif // SFD_STORE_H
