#include "sfd_parse_regex.h"
#include "log.h"

#include <boost/regex.hpp>

namespace sfd {

const char* g_html_regex[] = {
    "<!DOCTYPE.*?>",
    "<!--.*?-->",
    "<script.*?>.*?</script>",
    "<style.*?>.*?</style>",
    "^( |\n|\t)+",
    "&.{2,5};|&#.{2,5};",
    "#[0-9a-zA-Z_-]+=[&#0-9a-zA-Z_=-]+",
    "<.*?>",
    "<a .*?href=\"(.*?)\".*?>",
    "<title>(.*)</title>",
    "",
    ""
};

// 替换字符串

void regex_replace(std::string& src, E_HTML_REGEX t, const char* fmt)
{
    if (t >= REGEX_LAST) {
        FATAL("E_HTML_REGEX more than REGEX_LAST");
        return ;
    }
    try {
        src = boost::regex_replace(src, boost::regex(g_html_regex[t], boost::regex::icase|boost::regex::perl), fmt);
    }catch (const boost::regex_error &e) {
        FATAL("regex_replace %s error %s",g_html_regex[t], e.what());
    }
}

void regex_replace(std::string& src, E_HTML_REGEX t, std::string& fmt)
{
    if (t >= REGEX_LAST) {
        FATAL("E_HTML_REGEX more than REGEX_LAST");
        return ;
    }
    try {
        src = boost::regex_replace(src, boost::regex(g_html_regex[t], boost::regex::icase|boost::regex::perl), fmt);
    }catch (const boost::regex_error &e) {
        FATAL("regex_replace %s error %s",g_html_regex[t], e.what());
    }
}

void regex_replace(std::string& src, std::string& regex, std::string& fmt)
{
    try {
        src = boost::regex_replace(src, boost::regex(regex, boost::regex::icase|boost::regex::perl), fmt);
    }catch (const boost::regex_error &e) {
        FATAL("regex_replace %s error %s", regex.c_str(), e.what());
    }
}

}// namespace


