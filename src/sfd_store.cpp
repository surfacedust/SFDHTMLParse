#include "sfd_store.h"
#include "log.h"

#if 0
// 从crawler 队列中获取HTML
#include "crawler_def.h"
#include "crawler_data.h"
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <sys/time.h>
#include <sys/stat.h>


using std::vector;

namespace sfd {

extern const char* g_fields_text[];


Store* create_log_store(store_log_conf_t &conf)
{
    return NULL;
}

Store* create_hbase_store(store_hbase_conf_t &conf)
{
    return NULL;
}

Store* create_thrift_store(store_thrift_conf_t &conf)
{
    ThriftStore *store = new ThriftStore(conf.host_port);
    return store;
}

Store* create_mysql_store(store_mysql_conf_t &conf)
{
    return NULL;
}

Store* create_local_store(store_local_conf_t &conf)
{
    LocalStore *store = new LocalStore(conf.path);
    return store;
}

Store* create_crawl_store(store_crawl_conf_t &conf)
{
    CrawlStore *store = new CrawlStore();
    return store;
}

CrawlStore::CrawlStore(int rw_switch)
{
    _rw_switch = rw_switch;
}

CrawlStore::~CrawlStore()
{

}

int CrawlStore::readHTML(html_t &html)
{
#if 0
    html_info_t *crawler_html = NULL;
    if (g_crawler_data().html_pool->pop(crawler_html)!= 0) {
        return -1;
    }
    html.url.seed_id    = crawler_html->url_info.seed_id;
    html.url.id         = crawler_html->url_info.id;
    html.url.parent_id  = crawler_html->url_info.parent_id;
    html.url.depth      = crawler_html->url_info.depth;
    html.url.weight     = crawler_html->url_info.weight;
    html.url.expand_way = (EnumExpandWay)crawler_html->url_info.expend_way;
    html.url.url        = crawler_html->url_info.url;
    html.html           = crawler_html->html;
    g_crawler_data().html_pool->mfree(crawler_html);
#endif
    return 0;
}

ThriftStore::ThriftStore()
{
    ThriftStore("10.0.2.43:9091");
}

ThriftStore::ThriftStore(const char *host_port)
{
    _rw_switch = WU;
    std::string tmp(host_port);
    if (tmp.size() == 0) {
        tmp = "10.0.2.43:9091";
    }

    _url_serv_client = new UrlServClient(tmp);
}

ThriftStore::~ThriftStore()
{
    delete _url_serv_client;
}

bool ThriftStore::CheckConnect()
{
    int ret = _url_serv_client->CheckConnect();
    if (ret) {
        ret = _url_serv_client->TransportOpen();
        if (ret) {
            FATAL("UrlServClient::UrlServClient TransportOpen fail");
            return false;
        }
    }
    return true;
}

int ThriftStore::writeURLS(const html_t &html, const std::vector<url_t> &urls)
{

    radar::ParserUrl parser_url;
    radar::SuccessUrl success_url;
    radar::NewUrl new_url;
    vector<radar::NewUrl> v_new_url;
    success_url.seed_id = html.url.seed_id;
    success_url.id = html.url.id;
    success_url.url = html.url.url;
    parser_url.succ_url = success_url;
    for (vector<url_t>::const_iterator it = urls.begin(); it != urls.end(); ++it) {
        new_url.url = (*it).url;
        new_url.depth = (*it).depth;
        new_url.weight = (*it).weight;
        v_new_url.push_back(new_url);
    }
    DEBUG("send urls size %d", v_new_url.size());
    parser_url.new_urls = v_new_url;
    _vParserUrl.push_back(parser_url);

    if (CheckConnect() && _url_serv_client->sendParserUrls(_vParserUrl) > 0) {
        _vParserUrl.clear();
    }
    return 0;
}

LocalStore::LocalStore(char *path, int rw_switch)
{
    local_store_init(path);
    _rw_switch = rw_switch;
}

void LocalStore::local_store_init(const char *path)
{

    if (path == NULL || strlen(path) == 0) {
        strcpy(_path, "data");
    } else {
        strncpy(_path, path, MAX_CHAR_SIZE-1);
        size_t len = strlen(_path);

        if (_path[len -1] == '/') {
            _path[len-1] = '\0';
        }
    }

}

int LocalStore::readHTML(html_t &html)
{
    std::ifstream in("163.html",std::ios::in);
    if (!in) {
        std::cout <<"File 163.html not found!\n";
        exit(EXIT_FAILURE);
    }
    html.url.url = "www.163.html";
    html.url.expand_way = BY_NOLIMIT;
    html.url.depth = 0;
    html.url.weight = 0;
    html.url.id = 1;
    html.url.seed_id = 1;
    in.seekg(0, std::ios::end);
    html.html.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&html.html[0], html.html.size());
    in.close();
    return 0;
}

void LocalStore::createDir(const char *dir)
{
    char _dir[MAX_CHAR_SIZE] = {0};
    char mkdir_cmd[MAX_CHAR_SIZE] = {0};
    snprintf(_dir, MAX_CHAR_SIZE, "%s/%s", _path, dir);
    if (access(_dir, 0) == -1) {
        snprintf(mkdir_cmd, MAX_CHAR_SIZE, "mkdir -p %s", _dir);
        system(mkdir_cmd);
    }
}

void LocalStore::getFileNameWithPath(char* fileName, size_t len, const char *infixPath, const char *suffixFile)
{
    char curTime[64] = {0};
    struct timeval tv;
    gettimeofday(&tv, NULL);
    strftime(curTime, sizeof(curTime), "%Y-%m-%d_%X", localtime(&tv.tv_sec));
    snprintf(fileName, len, "%s/%s/%s:%ld_%s.txt", _path, infixPath, curTime, tv.tv_usec/1000, suffixFile);
    return ;
}

int LocalStore::writeURLS(const html_t &html, const std::vector<url_t> &urls)
{
    size_t w_size = 0;
    char url_file[MAX_CHAR_SIZE] = {0};
    createDir((char*)"urls");
    getFileNameWithPath((char*)url_file, MAX_CHAR_SIZE, (char*)"urls", html.url.url.c_str());
    FILE* fp = fopen(url_file, "w");
    DEBUG("create url file %s", url_file);
    if (fp == NULL) {
        FATAL("create %s file failed", url_file);
        return -1;
    }
    for (vector<url_t>::const_iterator it = urls.begin(); it != urls.end(); ++it) {
        const url_t &url = *it;
        w_size = fwrite(url.url.c_str(), 1, url.url.size(), fp);
        fputc('\n', fp);
    }
    fclose(fp);
    return 0;
}

int LocalStore::writeTEXTS(const html_t &html, const std::vector<text_t> &texts)
{
    size_t w_size;
    char text_file[MAX_CHAR_SIZE] = {0};
    createDir((char*)"texts");
    getFileNameWithPath((char*)text_file, MAX_CHAR_SIZE, (char*)"texts", html.url.url.c_str());
    FILE* fp = fopen(text_file, "w");
    DEBUG("create text file %s", text_file);
    if (fp == NULL) {
        FATAL("create %s file failed", text_file);
        return -1;
    }
    for (vector<text_t>::const_iterator it = texts.begin(); it != texts.end(); ++it) {
        const text_t &text = *it;
        w_size = fprintf(fp, "%s = %c%s%c\n", g_fields_text[text.type], 1, text.text.c_str(), 1);
    }
    fclose(fp);
    return 0;
}

} //namespace
