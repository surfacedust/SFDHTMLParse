#include "sfd_parsestd.h"
#include "sfd_parser.h"
#include "sfd_html_parse_config.h"
#include "sfd_parse_regex.h"
#include "sfd_store.h"

#include "log.h"

#include <algorithm>
#include <unistd.h>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <boost/regex.hpp>

using std::vector;
using std::string;
using std::make_pair;

namespace sfd {

// 提取内容含义
const char* g_fields_text [] = {
    "url",
    "title",
    "keywords",
    "description",
    "time",
    "author",
    "content",
    "unknow",
    NULL
};

Parser::Parser():_intelligent_url_filter_sw(true),_intelligent_content_filter_sw(true) {
    _type = 0;
    _url_filter = NULL;
    _content_filter = NULL;
}
void Parser::set_intelligent_url_filter_sw(bool intelligent_url_filter) {
    _intelligent_url_filter_sw = intelligent_url_filter;
}

void Parser::set_intelligent_content_filter_sw(bool intelligent_content_filter_sw)
{
    _intelligent_content_filter_sw = intelligent_content_filter_sw;
}

void Parser::set_url_filter(Filter *filter)
{
    _url_filter = filter;
}

void Parser::set_content_filter(Filter *filter)
{
    _content_filter = filter;
}

void Parser::set_html(html_t &html)
{
    _urls.clear();
    _text.clear();
    _html = html;
}

void Parser::html_pre()
{
    // 去掉文档类型, 注释, 脚本, 样式
    regex_replace(_html.html, REGEX_DOCUMENT, "");
    regex_replace(_html.html, REGEX_ANNOTATION, "");
    regex_replace(_html.html, REGEX_SCRIPT, "");
    regex_replace(_html.html, REGEX_STYLE, "");
}

bool Parser::url_filter(std::string &url)
{
    EnumExpandWay expand_way = _html.url.expand_way;
    if (expand_way == BY_NOLIMIT) {
        return true;
    } else if (expand_way == BY_SEED_DOMAIN) {
        size_t pos = _html.url.url.find_first_of('/', _html.url.url.find_first_of('.'));
        if (pos == string::npos) {
            pos = url.size();
        }
        if (_html.url.url.compare(0, pos, url) == 0) {
            return true;
        }
    } else if (expand_way == BY_SEED_ROOT) {
        // 目前和BY_SEED_DOMAIN相同,具体等确定好域名是否单指顶级域名之后再更换
        size_t pos = _html.url.url.find_first_of('/', _html.url.url.find_first_of('.'));
        if (pos == string::npos) {
            pos = url.size();
        }
        if (_html.url.url.compare(0, pos, url) == 0) {
            return true;
        }

    } else if (expand_way == BY_SEED_DIRECTORY) {
        size_t pos = 0;
        size_t last_dot_pos = _html.url.url.find_last_of('.');
        if (last_dot_pos == string::npos) {
            last_dot_pos = _html.url.url.size();
        }
        size_t last_backslash_pos = _html.url.url.find_last_of('/');
        if (last_backslash_pos == string::npos) {
            pos = _html.url.url.size();
        } else if (last_dot_pos > last_backslash_pos) {
            pos = last_backslash_pos;
        } else {
            FATAL("url is error: %s : %s", _html.url.url.c_str(), url.c_str());
        }
        if (_html.url.url.compare(0, pos, url) == 0) {
            return true;
        }

    } else {
        FATAL("bad expandWay %d", expand_way);
    }

    return false;
}

void Parser::url_completion(string &url)
{
    if (url[0] == '/') {
        url = _html.url.url + url;
        DEBUG("AutoComplet %s", url.c_str());
    }

    if (url.compare(0,5,"http:") != 0 &&
            url.compare(0,6,"https:") != 0 &&
            url.compare(0,4, "ftp:") != 0) {
        url = "http:" + url;
        DEBUG("AutoComplet %s", url.c_str());
    }
    return ;
}

void Parser::url_parser(string &html)
{
    url_t tmp;
    EnumFilter f_ret;
    boost::smatch m;
    boost::regex reg(g_html_regex[REGEX_URL], boost::regex::icase|boost::regex::perl);
    regex_replace(html, REGEX_URL_SUFFIX, "");
    string::const_iterator it = html.begin();
    string::const_iterator end = html.end();
    try {
        while(boost::regex_search(it, end, m, reg)) {
            tmp.url = m[1];
            tmp.weight = _html.url.weight;
            tmp.depth = _html.url.depth + 1;
            if (tmp.url[tmp.url.size() - 1] == '/') {
                tmp.url.resize(tmp.url.size() -1);
            }

            if (tmp.url.size() > 0) {
                f_ret =_url_filter->filter(tmp.url);
                if (f_ret == F_NONE || f_ret == F_DELETE) {
                    url_completion(tmp.url);
                    if (url_filter(tmp.url))
                        _urls.push_back(tmp);
                } else {
                    DEBUG("url_filter");
                }
            }

            it = m[0].second;
        }
    }catch (const boost::regex_error &e) {
        DEBUG(" Parser::url_parser error %s", e.what());
        return ;
    }
}

void Parser::content_parser(string &html)
{
    EnumFilter f_ret;
    // 默认提取两个字段 ：标题和正文
    // 正文部分默认为body 中除去标签之外的所有文字
    text_t text;
    boost::smatch m;
    try {
        if (boost::regex_search(html, m, boost::regex(g_html_regex[REGEX_TITLE], boost::regex::icase|boost::regex::perl))) {
            text.text = m[1];
            text.type = TITLE;
            _text.push_back(text);
        }
    }catch (const boost::regex_error &e) {
        FATAL(" Parser::content_parser title error %s", e.what());
    }

    try {

        if (boost::regex_search(html, m, boost::regex("<body>(.*)</body>"))) {
            string tmp_str = m[1];
            // 去掉所有<>
            regex_replace(tmp_str, REGEX_ALL_TAG, "");
            regex_replace(tmp_str, REGEX_SPECIAL_CHAR, "");
            regex_replace(tmp_str, REGEX_WHITESPACE, "");
            text.type = CONTENT;
            text.text = tmp_str;
            f_ret =_content_filter->filter(text.text);
            if (f_ret == F_NONE || f_ret == F_DELETE) {
                _text.push_back(text);
            }
        }
    }catch (const boost::regex_error &e) {
        FATAL(" Parser::content_parser body error %s", e.what());
    }
}

void Parser::parse()
{
    // 预处理HTML
    html_pre();
    url_parser(_html.html);
    content_parser(_html.html);
    remove_duplicate_url(_urls);
}

void Parser::debug()
{
    for (vector<url_t>::iterator it = _urls.begin(); it != _urls.end(); ++it) {
        url_t &url = *it;
        std::cout << "url :" << url.url << std::endl;
    }

    for (vector<text_t>::iterator it = _text.begin(); it != _text.end(); ++it) {
        text_t &text = *it;
        std::cout << g_fields_text[text.type] << " : " << text.text << std::endl;
    }
}

Parser* Parser::clone() const
{
    Parser *parser = new Parser();
    parser->_content_filter = _content_filter;
    parser->_url_filter = _url_filter;
    parser->_intelligent_url_filter_sw = _intelligent_url_filter_sw;
    parser->_intelligent_content_filter_sw = _intelligent_content_filter_sw;
    return parser;
}


Parser *RegexParser::clone() const
{
    RegexParser *parser = new RegexParser();
    parser->_regex_strs = _regex_strs;
    parser->_url_filter = _url_filter;
    parser->_content_filter = _content_filter;
    parser->_intelligent_url_filter_sw = _intelligent_url_filter_sw;
    parser->_intelligent_content_filter_sw = _intelligent_content_filter_sw;
    return parser;
}

void RegexParser::content_parser(regex_t &regex_str, string &regex_ret)
{
    EnumFilter f_ret;
    text_t text;

    regex_replace(regex_ret, REGEX_ALL_TAG, "");
    regex_replace(regex_ret, REGEX_SPECIAL_CHAR, "");
    regex_replace(regex_ret, REGEX_WHITESPACE, "");
    text.text = regex_ret;
    text.type = regex_str.type;
    f_ret =_content_filter->filter(text.text);
    if (f_ret == F_NONE || f_ret == F_DELETE) {
        _text.push_back(text);
    }

}

void RegexParser::parse()
{
	html_pre();
    boost::smatch m;
    string regex_ret;

    for (vector<regex_t>::iterator it = _regex_strs.begin(); it != _regex_strs.end(); ++it) {
        regex_t &regex_str = *it;
        string::const_iterator html_it = _html.html.begin();
        string::const_iterator html_end = _html.html.end();
        try {
            while(boost::regex_search(html_it, html_end, m, boost::regex(regex_str.regex_str))) {
                regex_ret = m[1];
                if (regex_str.type == URL) {
                    url_parser(regex_ret);
                } else {
                    content_parser(regex_str, regex_ret);
                }
                html_it = m[0].second;
            }
        }catch (const boost::regex_error &e) {
            FATAL(" Parser::url_parser error %s", e.what());
            return ;
        }
    }
    remove_duplicate_url(_urls);
}

void RegexParser::setRegexVector(vector<regex_t> &regex_str)
{
    _regex_strs.clear();
    _regex_strs = regex_str;
}

void RegexParser::addRegexStr(regex_t &regex_str)
{
    _regex_strs.push_back(regex_str);
}

void DomParser::meta_parse()
{
    text_t text_str;
    assert(_parserOut->root->type == GUMBO_NODE_ELEMENT);
    const GumboVector* root_children = &_parserOut->root->v.element.children;
    GumboNode* head = NULL;
    for (unsigned int i = 0; i < root_children->length; ++i) {
      GumboNode* child = (GumboNode*)root_children->data[i];
      if (child->type == GUMBO_NODE_ELEMENT &&
          child->v.element.tag == GUMBO_TAG_HEAD) {
        head = child;
        break;
      }
    }
    assert(head != NULL);
    GumboAttribute* attr;
    for (vector<meta_t>::iterator it = _meta_texts.begin(); it != _meta_texts.end(); ++it) {
        meta_t &meta_text = *it;
        GumboVector* head_children = &head->v.element.children;
        for (unsigned int i = 0; i < head_children->length; ++i) {
            GumboNode* child = (GumboNode*)head_children->data[i];
            if (child->type == GUMBO_NODE_ELEMENT &&
                    child->v.element.tag == GUMBO_TAG_META) {
                attr = gumbo_get_attribute(&child->v.element.attributes, "name");
                if (attr == NULL) {
                    continue;
                }
                if(!strcasecmp(meta_text.name.c_str(), attr->value)) {
                   attr = gumbo_get_attribute(&child->v.element.attributes, "content");
                    if (attr) {
                        text_str.type = meta_text.type;
                        text_str.text = string(attr->value);
                        _text.push_back(text_str);
                        break;
                    }
                }
            }
        }
        if (text_str.type != meta_text.type) {
            INFO("seed %ld, url %s meta %s can't find", _html.url.seed_id, _html.url.url.c_str(), g_fields_text[meta_text.type]);
        }
    }
}

void DomParser::content_parser(tag_t &tag, std::string &html)
{
    EnumFilter f_ret;
    text_t text;

    regex_replace(html, REGEX_ALL_TAG, "");
    regex_replace(html, REGEX_SPECIAL_CHAR, "");
    regex_replace(html, REGEX_WHITESPACE, "");
    text.text = html;
    text.type = tag.type;
    f_ret =_content_filter->filter(text.text);
    if (f_ret == F_NONE || f_ret == F_DELETE) {
        _text.push_back(text);
    }
}

void DomParser::tag_parse()
{
    assert(_parserOut->root->type == GUMBO_NODE_ELEMENT);
    for (vector<tag_t>::iterator tag_it = _tag_texts.begin(); tag_it != _tag_texts.end(); ++tag_it) {
        tag_t &tag = *tag_it;
        vector<tag_name_t>::iterator tag_begin_it = tag.tag.begin();
        vector<tag_name_t>::iterator tag_end_it = tag.tag.end();
        GumboNode *tag_Node = NULL;
        find_tag_GumboNode(_parserOut->root, tag_begin_it, tag_end_it, &tag_Node);
        if (tag_Node && tag_Node->v.element.original_tag.data && tag_Node->v.element.original_end_tag.data) {
            string tag_s(tag_Node->v.element.original_tag.data,
                         tag_Node->v.element.original_end_tag.data + tag_Node->v.element.original_end_tag.length);
            if (tag_s.size() > 0) {
                if (tag.type == URL) {
                    url_parser(tag_s);
                } else {
                    content_parser(tag, tag_s);
                }
            }
        } else {
            DEBUG("seed %ld, url %s tag %s can't find", _html.url.seed_id, _html.url.url.c_str(), g_fields_text[tag.type]);
        }
    }
}

void DomParser::parse()
{
    html_pre();
    _parserOut = gumbo_parse_with_options(_gumboOptions == NULL ? &kGumboDefaultOptions : _gumboOptions, _html.html.c_str(), _html.html.size());
    // meta
    meta_parse();
    //tag
    tag_parse();
    remove_duplicate_url(_urls);
}

void DomParser::find_tag_GumboNode(GumboNode *root, vector<tag_name_t>::iterator begin_it,
                        vector<tag_name_t>::iterator end_it, GumboNode **ret)
{
    assert(root != NULL);
    assert(root->type == GUMBO_NODE_ELEMENT);
    if (begin_it == end_it) {
        *ret = root;
        return ;
    }
    unsigned int i = 0;
    GumboVector *children = &root->v.element.children;
    tag_name_t &tag_name = *begin_it;
    for (i = 0; i < children->length; ++i) {
        GumboNode *child = (GumboNode*)children->data[i];
        if (child->type == GUMBO_NODE_ELEMENT &&
                child->v.element.tag == tag_name.e_tag && (tag_name.attr.size() == 0 ||
        tag_name.attr.compare(0, tag_name.attr.size(), child->v.element.original_tag.data, child->v.element.original_tag.length) == 0))
        {
            find_tag_GumboNode(child, ++begin_it, end_it, ret);
            if (ret) {
                break ;
            }
        }
    }
    return ;
}

void DomParser::set_html(html_t &html)
{
    if (_parserOut) {
        gumbo_destroy_output(_gumboOptions == NULL ? &kGumboDefaultOptions : _gumboOptions, _parserOut);
        _parserOut = NULL;
    }
    Parser::set_html(html);
}

Parser* DomParser::clone() const
{
    DomParser *parser = new DomParser();
    parser->_meta_texts = _meta_texts;
    parser->_tag_texts = _tag_texts;
    parser->_url_filter = _url_filter;
    parser->_content_filter = _content_filter;
    parser->_intelligent_url_filter_sw = _intelligent_url_filter_sw;
    parser->_intelligent_content_filter_sw = _intelligent_content_filter_sw;
    return parser;
}

void DomParser::init()
{
    if (_parserOut) {
        gumbo_destroy_output(_gumboOptions == NULL ? &kGumboDefaultOptions : _gumboOptions, _parserOut);
        _parserOut = NULL;
    }
    _meta_texts.clear();
    _tag_texts.clear();
    if (_gumboOptions) {
        free(_gumboOptions);
        _gumboOptions = NULL;
    }
}

void DomParser::setGumboOptions(GumboOptions *options)
{
    if (_gumboOptions) {
        free(_gumboOptions);
    }
    _gumboOptions = options;
}

void DomParser::addMetaText(meta_t &meta_text)
{
    _meta_texts.push_back(meta_text);
}

void DomParser::addTagText(tag_t &tag_text)
{
    _tag_texts.push_back(tag_text);
}

BlockParser::BlockParser(int block_num, int word_threshold)
{
    _remove_regex.push_back("(?is)<!DOCTYPE.*?>");
    _remove_regex.push_back("(?is)<!--.*?-->");
    _remove_regex.push_back("(?is)<script.*?>.*?</script>");
    _remove_regex.push_back("(?is)<style.*?>.*?</style>");
    _remove_regex.push_back("&.{2,5};|&#.{2,5};");
    _remove_regex.push_back("(?is)<.*?>");
    _block_num = block_num;
    _word_threshold = word_threshold;
    _type = 3;
}

BlockParser::~BlockParser()
{

}

void BlockParser::parse()
{
    Parser::parse();
}

Parser* BlockParser::clone() const
{
    BlockParser *parser = new BlockParser();
    *parser = *this;
    return parser;
}

void add_urls(vector<url_t> &sum_urls, vector<url_t> &urls)
{
    for (vector<url_t>::iterator it = urls.begin(); it != urls.end(); ++it) {
        url_t &url = *it;
        sum_urls.push_back(url);
    }

    return ;
}

void add_contents(vector<text_t> &sum_texts,  vector<text_t> &text)
{
    for (vector<text_t>::iterator it = text.begin(); it != text.end(); ++it) {
        text_t &t = *it;
        sum_texts.push_back(t);
    }
    return ;
}

void remove_duplicate_url(vector <url_t> &urls)
{
    std::sort(urls.begin(), urls.end(), url_cmp);
    urls.erase(std::unique(urls.begin(), urls.end()), urls.end());
}

Parser* create_block_parser(block_parser_conf_t &conf)
{
    BlockParser *parser = NULL;
    if (conf.block_size > 0) {
        parser = new BlockParser(conf.block_size, conf.word_threshold);
    }
    return parser;
}

Parser* create_dom_parser(dom_parser_conf_t &conf)
{
    DomParser *parser = NULL;
    if (conf.v_meta.size() > 0) {
        parser = new DomParser();
        meta_t meta_text;
        for ( vector< std::pair<string, string> >::iterator pair_it = conf.v_meta.begin();
              pair_it != conf.v_meta.end(); ++pair_it) {
            std::pair<string, string> &meta = *pair_it;
            meta_text.type = get_Enmufields_by_fields(meta.first);
            if (meta_text.type != UNKNOW) {
                meta_text.name = meta.second;
                parser->addMetaText(meta_text);
            } else {
                FATAL("unkown meta %s:%s", meta.first.c_str(), meta.second.c_str());
            }
        }
    }

    if (conf.v_tag.size()) {
        if (!parser) {
            parser = new DomParser();
        }
        char tag_name[MAX_CHAR_SIZE];
        int len = 0;
        tag_t tag_text;
        tag_name_t tag_e_attr;
        char *pos;
        for ( vector< std::pair<string, string> >::iterator pair_it = conf.v_tag.begin();
              pair_it != conf.v_tag.end(); ++pair_it) {
            std::pair<string, string> &tag = *pair_it;
            tag_text.type = get_Enmufields_by_fields(tag.first);
            if (tag_text.type != UNKNOW) {
                for (size_t i = 0; i < tag.second.size(); ++i) {
                    if (tag.second[i] == '.') {
                        tag_name[len] = '\0';
                        if (tag_name[len-1] == '>' && pos > tag_name) {
                            tag_e_attr.attr = string(pos, tag_name+len);
                            *pos = '\0';
                        }else {
                            tag_e_attr.attr.clear();
                        }
                        tag_e_attr.e_tag = gumbo_tag_enum(tag_name);
                        if (GUMBO_TAG_UNKNOWN != tag_e_attr.e_tag) {
                            tag_text.tag.push_back(tag_e_attr);
                        }
                        len = 0;
                    } else {
                        if (tag.second[i] == '<') {
                            pos = tag_name + len;
                        }
                        tag_name[len++] = tag.second[i];
                    }
                }
                if (len != 0) {
                    tag_name[len] = '\0';
                    if (tag_name[len-1] == '>' && pos > tag_name) {
                        tag_e_attr.attr = string(pos, tag_name+len);
                        *pos = '\0';
                    }else {
                        tag_e_attr.attr.clear();
                    }
                    tag_e_attr.e_tag = gumbo_tag_enum(tag_name);
                    if (GUMBO_TAG_UNKNOWN != tag_e_attr.e_tag) {
                        tag_text.tag.push_back(tag_e_attr);
                    }
                }
                if (tag_text.tag.size() > 0) {
                    parser->addTagText(tag_text);
                }
            } else {
                FATAL("unkown tag %s", tag.first.c_str());
            }
        }
    }
    return parser;
}

Parser* create_regex_parser(regex_parser_conf_t &conf)
{
    RegexParser *parser = NULL;
    if (conf.v_regex.size() > 0) {
        parser = new RegexParser();
        regex_t regex_str;
        for ( vector< std::pair<string, string> >::iterator pair_it = conf.v_regex.begin();
              pair_it != conf.v_regex.end(); ++pair_it) {
            std::pair<string, string> &reg = *pair_it;
            regex_str.type = get_Enmufields_by_fields(reg.first);
            if (regex_str.type != UNKNOW) {
                regex_str.regex_str = reg.second;
                parser->addRegexStr(regex_str);
            }
        }
    }
    return parser;
}

EnumFields get_Enmufields_by_fields(const std::string &text)
{
    int i = 0;
    while(g_fields_text[i] != NULL) {
        if (strcasecmp(g_fields_text[i], text.c_str()) == 0) {
            return (EnumFields)i;
        }
        i++;
    }
    return UNKNOW;
}

bool url_cmp(const url_t &a, const url_t &b)
{
    return a.url > b.url;
}

} //namespace
