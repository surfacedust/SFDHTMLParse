#ifndef SFD_FILTER_H
#define SFD_FILTER_H

#include "sfd_html_parse_config.h"

#include <string>
#include <vector>

// 过滤类，主要功能如下：
// 1.如果包含某些字符，则是需要的
// 2.如果不包含某些字符， 则是需要的
// 3.如果包含某些字符， 则过滤这些字符
// filter module
// url filter && text filter
// url filter {
//    include_charset;
//    exclude_charset;
//    delete_charset;
// }
// filter class

namespace sfd {

enum EnumFilter {
    F_NONE    ,
    F_INCLUDE ,
    F_EXCLUDE ,
    F_DELETE
};

class Filter {
public:
    Filter () {}
    Filter (filter_conf_t &filter_conf);
    EnumFilter filter(std::string &match_str);
    bool include_charset_filter(std::string &match_str);
    bool exclude_charset_filter(std::string &match_str);
    bool delete_charset_filter (std::string &match_str);
    void set_include_filter(std::vector <std::string> &include_charset);
    void set_exclude_filter(std::vector <std::string> &exclude_charset);
    void set_delete_filter (std::vector <std::string> &delete_charset);
protected:
    std::vector<std::string> _include_charset;
    std::vector<std::string> _exclude_charset;
    std::vector<std::string> _delete_charset;
};

} // namespace
#endif // SFD_FILTER_H
