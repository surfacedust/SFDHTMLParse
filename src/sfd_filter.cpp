#include "sfd_filter.h"

namespace sfd {

using std::string;
using std::vector;

Filter::Filter (filter_conf_t &filter_conf)
{
    set_include_filter(filter_conf._include_charset);
    set_exclude_filter(filter_conf._exclude_charset);
    set_delete_filter(filter_conf._delete_charset);
}

EnumFilter Filter::filter(string &match_str) {
    EnumFilter ret =F_NONE;
    if (include_charset_filter(match_str)) {
        ret = F_INCLUDE;
    } else if (exclude_charset_filter(match_str)){
        ret = F_EXCLUDE;
    }
    if (delete_charset_filter(match_str) && ret == F_NONE) {
        ret = F_DELETE;
    }
    return ret;
}

bool Filter::include_charset_filter(string &match_str) {
    for (vector<string>::iterator it = _include_charset.begin();
         it != _include_charset.end(); ++it) {

        string &s_include = *it;
        if (match_str.find(s_include) == match_str.npos) {
            return true;
        }

    }
    return false;
}

bool Filter::exclude_charset_filter(string &match_str) {
    for (vector<string>::iterator it = _exclude_charset.begin();
         it != _exclude_charset.end(); ++it) {

        string &s_exclude = *it;
        if (match_str.find(s_exclude) != match_str.npos) {
            return true;
        }
    }
    return false;
}

bool Filter::delete_charset_filter(string &match_str) {
    bool ret = false;
    size_t pos;
    for (vector<string>::iterator it = _delete_charset.begin();
         it != _delete_charset.end(); ++it) {

        string &s_delete = *it;
        do {
            pos = match_str.find(s_delete);
            if (pos != match_str.npos) {
                ret = true;
                match_str.erase(pos,s_delete.size());
            }
        }while(pos != match_str.npos);
    }
    return ret;
}

void Filter::set_include_filter(vector<string> &include_charset)
{
    _include_charset.clear();
    _include_charset = include_charset;
}

void Filter::set_exclude_filter(vector<string> &exclude_charset)
{
    _exclude_charset.clear();
    _exclude_charset = exclude_charset;
}

void Filter::set_delete_filter(vector<string> &delete_charset)
{
    _delete_charset.clear();
    _delete_charset = delete_charset;
}

} //namespace
