#ifndef SFD_HTML_PARSE_H
#define SFD_HTML_PARSE_H

#include "sfd_parsestd.h"
#include "sfd_parser.h"
#include "sfd_html_parse_config.h"
#include "sfd_store.h"

#include <string>
#include <vector>

// surfacedust html parse
namespace sfd {


struct html_parser_t {
    url_t url_seed;
    std::vector < Parser* > parser;
    Filter* url_filter;
    Filter* content_filter;
    ~html_parser_t()
    {
        delete url_filter;
        delete content_filter;
        for (std::vector < Parser* >::iterator it = parser.begin(); it != parser.end(); ++it) {
            Parser *p = *it;
            delete p;
        }
    }
};

class SfdHtmlParse {
public:
    SfdHtmlParse();
    ~SfdHtmlParse();
    void initSfdHtmlParse(parse_sys_conf_t &parse_sys_conf);
    void initStore(std::vector <store_conf_t> &store_conf);
    void initParser(std::vector <parser_conf_t> &parser_conf);
    void run();
private:
    html_parser_t* get_html_parser(int64_t seed_id);
    bool read_data();
    bool write_data();
    html_t _html;           // 待解析HTML
    std::vector<html_parser_t*> _html_parser;

    std::vector<Store*> _store;
    std::vector<text_t> _texts;
    std::vector<url_t> _urls;
};

void parse_thread_start();
void parse_thread_join();
} //namespace
#endif
