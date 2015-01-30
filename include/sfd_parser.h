#ifndef SFD_PARSER_H
#define SFD_PARSER_H

#include "gumbo.h"

#include "sfd_parsestd.h"
#include "sfd_filter.h"

namespace sfd {


enum EnumFields {
    URL,
    TITLE,
    KEYWORDS,
    DESCRIPTION,
    TIME,
    AUTHOR,
    CONTENT,
    UNKNOW
};

// 正则 与 该 正则提取到的内容标识
struct regex_t{
    EnumFields type;
    std::string regex_str;
};

//meta 名字 与 其代表的标识
struct meta_t{
    EnumFields type;
    std::string name;
};

struct tag_name_t {
    GumboTag e_tag;
    std::string attr;
};

//tag 名字 与 其代表的标识
struct tag_t{
    EnumFields type;
    std::vector<tag_name_t> tag;
};

//内容与内容标识
struct text_t{
    EnumFields type;
    std::string text;
};

//解析器基础类，接口类，所有解析器类都需要继承该接口
//基础类包含一套默认的提取规则，并且智能提取默认是打开的
class Parser {
public:
    Parser();
    virtual ~Parser() {
    }

    void set_intelligent_url_filter_sw(bool intelligent_url_filter);
    void set_intelligent_content_filter_sw(bool intelligent_content_filter_sw);
    void set_url_filter(Filter* filter);
    void set_content_filter(Filter* filter);
    virtual void set_html(html_t &html);
    void html_pre();
    void url_completion(std::string &url);
    bool url_filter(std::string &url);
    void url_parser(std::string &html);
    void content_parser(std::string &html);
    virtual void parse();
    virtual Parser* clone() const;
    virtual std::vector<url_t>& get_urls(){ return _urls;}
    virtual std::vector<text_t>& get_context(){ return _text;}
    virtual void debug();
protected:
    bool _intelligent_url_filter_sw;
    bool _intelligent_content_filter_sw;
    Filter* _url_filter;
    Filter* _content_filter;
    html_t _html;
    std::vector<text_t> _text;
    std::vector<url_t> _urls;
    int _type;
};

//基于正则的解析器
class RegexParser : public Parser{
public:
    RegexParser() { _type = 1; }
    void content_parser(regex_t &regex_str, std::string &regex_ret);
    virtual ~RegexParser() {}
    virtual void parse();
    virtual Parser* clone() const;
    virtual void set_html(html_t &html) { Parser::set_html(html);}
    virtual std::vector<url_t> &get_urls(){ return _urls;}
    virtual std::vector<text_t> &get_context(){ return _text;}
    void setRegexVector(std::vector<regex_t> &regex_str);
    void addRegexStr(regex_t &regex_str);
    virtual void debug() {Parser::debug();}
protected:
    std::vector<regex_t> _regex_strs;
};

//基于DOM树的解析器
class DomParser : public Parser {
public :
    DomParser() { _type = 2; _gumboOptions = NULL; _parserOut = NULL; init(); }
    virtual ~DomParser() {
        if (_parserOut) {
            gumbo_destroy_output(_gumboOptions == NULL ? &kGumboDefaultOptions : _gumboOptions, _parserOut);
        }

        if (_gumboOptions) {
            free(_gumboOptions);
        }
    }
    virtual void parse();
    void content_parser(tag_t &tag, std::string &html);
    void meta_parse();
    void tag_parse();
    virtual Parser* clone() const;
    virtual void set_html(html_t &html);
    virtual std::vector<url_t> &get_urls(){ return _urls;}
    virtual std::vector<text_t> &get_context(){ return _text;}
    void addMetaText(meta_t &meta_text);
    void addTagText(tag_t &tag_text);
    void setGumboOptions(GumboOptions *options);
    void init();
    virtual void debug() {Parser::debug();}

protected:
    void find_tag_GumboNode(GumboNode *root, std::vector<tag_name_t>::iterator begin_it,
                            std::vector<tag_name_t>::iterator end_it, GumboNode **ret);
    std::vector<meta_t> _meta_texts;
    std::vector<tag_t> _tag_texts;
    GumboOutput *_parserOut;
    GumboOptions *_gumboOptions;
};

//基于行块的解析器
class BlockParser : public Parser {
public :
    BlockParser(int block_num = 5, int word_threshold = 86);
    virtual ~BlockParser();
    virtual void parse();
    virtual Parser* clone() const;
    virtual void set_html(html_t &html) { Parser::set_html(html);}
    virtual std::vector<url_t> &get_urls() { return _urls;}
    virtual std::vector<text_t> &get_context(){ return _text;}
    virtual void debug() {Parser::debug();}

protected:
    int _block_num;
    int _word_threshold;
    std::vector<std::string> _remove_regex;
};

EnumFields get_Enmufields_by_fields(const std::string &text);
Parser* create_block_parser(block_parser_conf_t &conf);
Parser* create_dom_parser(dom_parser_conf_t &conf);
Parser* create_regex_parser(regex_parser_conf_t &conf);

bool url_cmp(const url_t &a, const url_t &b);
void add_urls(std::vector<url_t> &sum_urls, std::vector<url_t> &urls);
void add_contents(std::vector <text_t> &sum_texts,  std::vector <text_t> &text);
void remove_duplicate_url(std::vector <url_t> &urls);

} // namespace
#endif
