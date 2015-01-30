#ifndef SFD_PARSE_REGEX_H
#define SFD_PARSE_REGEX_H

#include <string>

namespace sfd {

enum E_HTML_REGEX {
    REGEX_DOCUMENT,
    REGEX_ANNOTATION,
    REGEX_SCRIPT,
    REGEX_STYLE,
    REGEX_WHITESPACE,
    REGEX_SPECIAL_CHAR,
    REGEX_URL_SUFFIX,
    REGEX_ALL_TAG,
    REGEX_URL,
    REGEX_TITLE,
    REGEX_UNKONW,
    REGEX_LAST
};

void regex_replace(std::string& src, E_HTML_REGEX t, std::string& fmt);
void regex_replace(std::string& src, std::string& regex, std::string& fmt);
void regex_replace(std::string& src, E_HTML_REGEX t, const char* fmt);

extern const char* g_html_regex[];

} //namespace
#endif // SFD_PARSE_REGEX_H
