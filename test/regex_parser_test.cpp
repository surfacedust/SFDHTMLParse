#include "sfd_html_parse_config.h"
#include "sfd_parser.h"
#include "log.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
using namespace std;
extern const char * g_fields_text[];

int main()
{
    log_init("./config", "log");
    sfd::Filter filter;
    sfd::RegexParser parser;
    parser.set_url_filter(&filter);
    parser.set_content_filter(&filter);
    sfd::regex_t regex_str;
    regex_str.type = sfd::URL;
    regex_str.regex_str = "<div class=\"news_list\">(.*?)</div>";
    parser.addRegexStr(regex_str);
    regex_str.regex_str = "<div class=\"box flow\">(.*?)<div class=\"sidebar fr\">";
    parser.addRegexStr(regex_str);
    regex_str.type = sfd::TITLE;
    regex_str.regex_str = "<title>(.*?)</title>";
    parser.addRegexStr(regex_str);
    sfd::html_t html;
    html.url.url = "www.163.com";
    html.url.depth = 0;
    html.url.weight = 1;
    html.url.expand_way = sfd::BY_NOLIMIT;
    fstream in("163.html");
    ostringstream tmp;
    tmp << in.rdbuf();
    html.html = tmp.str();
    parser.set_html(html);
    cout << "parser start" << endl;
    parser.parse();
    cout << "parser end" << endl;
    parser.debug();
    return 0;
}

