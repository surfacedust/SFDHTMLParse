
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>

#include "gumbo.h"
#include "sfd_parser.h"
#include "sfd_parsestd.h"
using std::string;
int main(int argc, char **argv)
{
    sfd::html_t html;
    if (argc != 2) {
        std::cout << "Usage: need <html filname>\n";
        exit(EXIT_FAILURE);
    }
    const char* filename = argv[1];
    std::ifstream in(filename, std::ios::in);
    if (!in) {
        std::cout <<"File " << filename << "not found!\n";
        exit(EXIT_FAILURE);
    }
    html.url.url = std::string(filename);
    html.url.expand_way = sfd::BY_NOLIMIT;
    html.url.depth = 0;
    html.url.weight = 0;
    html.url.id = 1;
    html.url.seed_id = 0;
    in.seekg(0, std::ios::end);
    html.html.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&html.html[0], html.html.size());
    in.close();
    sfd::Filter filter;
    sfd::DomParser parser;
    parser.set_url_filter(&filter);
    parser.set_content_filter(&filter);
    sfd::meta_t meta_text;
    sfd::tag_t tag_text;
    meta_text.type = sfd::KEYWORDS;
    meta_text.name = "keywords";
    parser.addMetaText(meta_text);

    meta_text.type = sfd::DESCRIPTION;
    meta_text.name = "description";
    parser.addMetaText(meta_text);

    meta_text.type = sfd::AUTHOR;
    meta_text.name = "author";
    parser.addMetaText(meta_text);

    tag_text.type = sfd::URL;
    char tag_name[MAX_CHAR_SIZE];
    int len = 0;
    char *pos;
    sfd::tag_name_t tag_e_attr;
    std::string tag_s;

    tag_s = "body.div<div class=\"ep-header\">.div<div class=\"N-nav-channel JS_NTES_LOG_FE\" data-module-name=\"xwwzy_11_headdaohang\">";

    std::cout << "tag --: " << tag_s << std::endl;
    for (size_t i = 0; i < tag_s.size(); ++i) {
        if (tag_s[i] == '.') {
            tag_name[len] = '\0';
            if (tag_name[len-1] == '>' && pos > tag_name) {
                tag_e_attr.attr = string(pos, tag_name+len);
                *pos = '\0';
            } else {
                tag_e_attr.attr.clear();
            }
            std::cout << "attr :" << tag_e_attr.attr << std::endl;
            std::cout <<"tag :" << tag_name << std::endl;
            tag_e_attr.e_tag = gumbo_tag_enum(tag_name);
            if (GUMBO_TAG_UNKNOWN != tag_e_attr.e_tag) {
                tag_text.tag.push_back(tag_e_attr);
            }
            len = 0;
        } else {
            if (tag_s[i] == '<') {
                pos = tag_name + len;
            }
            tag_name[len++] = tag_s[i];
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

        std::cout << "attr :" << tag_e_attr.attr << std::endl;
        std::cout <<"tag :" << tag_name << std::endl;

        tag_e_attr.e_tag = gumbo_tag_enum(tag_name);
        if (GUMBO_TAG_UNKNOWN != tag_e_attr.e_tag) {
            tag_text.tag.push_back(tag_e_attr);
        }
    }
    parser.addTagText(tag_text);

    tag_text.tag.clear();
    tag_text.type = sfd::CONTENT;
    len = 0;
    pos = NULL;
    tag_s = "body.div<div class=\"ep-content\" id=\"js-epContent\" style=\"position: relative;\">"
            ".div<div class=\"ep-content-bg clearfix\">"
            ".div<div class=\"ep-content-main\" id=\"epContentLeft\">"
            ".div<div id=\"endText\" class=\"end-text\">";
    std::cout << "tag --: " << tag_s << std::endl;
    for (size_t i = 0; i < tag_s.size(); ++i) {
        if (tag_s[i] == '.') {
            tag_name[len] = '\0';
            if (tag_name[len-1] == '>' && pos > tag_name) {
                tag_e_attr.attr = string(pos, tag_name+len);
                *pos = '\0';
            } else {
                tag_e_attr.attr.clear();
            }
            std::cout << "attr :" << tag_e_attr.attr << std::endl;
            std::cout <<"tag :" << tag_name << std::endl;
            tag_e_attr.e_tag = gumbo_tag_enum(tag_name);
            if (GUMBO_TAG_UNKNOWN != tag_e_attr.e_tag) {
                tag_text.tag.push_back(tag_e_attr);
            }
            len = 0;
        } else {
            if (tag_s[i] == '<') {
                pos = tag_name + len;
            }
            tag_name[len++] = tag_s[i];
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

        std::cout << "attr :" << tag_e_attr.attr << std::endl;
        std::cout <<"tag :" << tag_name << std::endl;

        tag_e_attr.e_tag = gumbo_tag_enum(tag_name);
        if (GUMBO_TAG_UNKNOWN != tag_e_attr.e_tag) {
            tag_text.tag.push_back(tag_e_attr);
        }
    }
    parser.addTagText(tag_text);


    parser.set_html(html);
    parser.parse();
    parser.debug();
    return 0;
}
