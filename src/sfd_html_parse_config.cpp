#include "sfd_html_parse_config.h"
#include "sfd_parser.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <string>

namespace sfd {

extern const char* g_fields_text [];

parse_sys_conf_t g_parse_sys_conf;

void debug_parser_config()
{
}

bool parse_sys_conf_init(char *fpath, char *fname)
{
    struct collection_item *config = NULL;

    strncpy(g_parse_sys_conf.fpath, fpath, MAX_CHAR_SIZE);
    strncpy(g_parse_sys_conf.fname, fname, MAX_CHAR_SIZE);
    conf_init(fpath, fname, &config);
    if (config == NULL) {
        FATAL("parse sys conf failed");
        return false;
    }

    conf_int(config, "sys", "thread_num", 1, &(g_parse_sys_conf.thread_num));
    conf_int(config, "sys", "seed_url_num", 0, &(g_parse_sys_conf.seed_url_num));
    store_conf_init(config, g_parse_sys_conf);
    seed_url_init(config, g_parse_sys_conf);
    conf_free(config);
    return true;
}

void store_conf_init(collection_item *config, parse_sys_conf_t &parse_sys_conf)
{
    char store_type[MAX_CHAR_SIZE];
    char store_list[MAX_CHAR_SIZE];
    char *pStore = NULL;
    conf_string(config, "sys", "store", "", store_list, MAX_CHAR_SIZE);

    while((pStore = strtok(pStore ? NULL : store_list, "; "))) {
        store_conf_t store_conf;
        conf_string(config, pStore, "type", "", store_type, MAX_CHAR_SIZE);
        if (0 == strcasecmp(store_type, "log")) {
            store_log_conf(config, pStore,store_conf);
        } else if (0 == strcasecmp(store_type, "mysql")) {
            store_mysql_conf(config, pStore, store_conf);
        } else if (0 == strcasecmp(store_type, "hbase")) {
            store_hbase_conf(config, pStore, store_conf);
        } else if (0 == strcasecmp(store_type, "thrift")) {
            store_thrift_conf(config, pStore, store_conf);
        } else if (0 == strcasecmp(store_type, "local")) {
            store_local_conf(config, pStore, store_conf);
        } else if (0 == strcasecmp(store_type, "crawl")){
            store_crawl_conf(config, pStore, store_conf);
        }else {
            FATAL("unkown store type %s", store_type);
            continue;
        }
        parse_sys_conf.store_conf.push_back(store_conf);
    }
    return ;
}

void seed_url_init(collection_item *config, parse_sys_conf_t &parse_sys_conf)
{
    char section[MAX_CHAR_SIZE];
    char url_seed[MAX_CHAR_SIZE];
    for( int i = 0; i < parse_sys_conf.seed_url_num; ++i) {
        parser_conf_t parser_conf;
        sprintf(section, "url_seed_%d", i);

        conf_string(config, section, "url_seed", "", url_seed, MAX_CHAR_SIZE);
        if (0 == strlen(url_seed)) {
            continue;
        }
        parser_conf.seed_url.url = url_seed;

        int seed_id;
        conf_int(config, section, "url_seed_id", i+1, &seed_id);
        parser_conf.seed_url.seed_id = (int64_t) seed_id;

        conf_int(config, section, "url_depth", 1, &(parser_conf.seed_url.depth));
        conf_double(config, section, "seed_weight", 0, &(parser_conf.seed_url.weight));
        char expand_way[MAX_CHAR_SIZE];
        conf_string(config, section, "expand_way", "NOLIMIT", expand_way, MAX_CHAR_SIZE);

        if (strcasecmp(expand_way, "NOLIMIT") == 0) {
           parser_conf.seed_url.expand_way = BY_NOLIMIT;
        } else if (strcasecmp(expand_way, "ROOT") == 0) {
            parser_conf.seed_url.expand_way = BY_SEED_ROOT;
        } else if (strcasecmp(expand_way, "DOMAIN") == 0) {
            parser_conf.seed_url.expand_way = BY_SEED_DOMAIN;
        } else if (strcasecmp(expand_way, "DIRECTORY") == 0) {
            parser_conf.seed_url.expand_way = BY_SEED_DIRECTORY;
        }
        parser_conf_init(config, section, parser_conf);
        filter_conf_init(config, section, parser_conf);
        g_parse_sys_conf.parse_conf.push_back(parser_conf);
    }
}

void parser_conf_init(collection_item *config, char *section, parser_conf_t &parser_conf)
{
    char *pParser = NULL;
    char parser_type[MAX_CHAR_SIZE];
    char parser_list[MAX_CHAR_SIZE];
    conf_string(config, section, "parser", "", parser_list, MAX_CHAR_SIZE);
    while((pParser = strtok(pParser ? NULL : parser_list, "; "))) {
        conf_string(config, pParser, "type", "", parser_type, MAX_CHAR_SIZE);
        if (0 == strcasecmp(parser_type, "regex")) {
            parser_regex_conf(config, pParser, parser_conf);
        } else if (0 == strcasecmp(parser_type, "dom")) {
            parser_dom_conf(config, pParser, parser_conf);
        } else if (0 == strcasecmp(parser_type, "block")) {
            parser_block_conf(config, pParser, parser_conf);
        }
    }
    return ;
}

void parser_dom_conf(struct collection_item *config, char *section, parser_conf_t &parser_conf)
{
    char value[MAX_CHAR_SIZE];
    const char *key;
    int i = 0;
    while((key = g_fields_text[i++]) != NULL) {
        conf_string(config, section, key, "", value, MAX_CHAR_SIZE);
        if (strlen(value) <= 0) {
            continue;
        }

        if (!strncasecmp(value, "meta.", 5)) {
            parser_conf.dom_parse.v_meta.push_back(std::make_pair(key, value+5));
        } else if (!strncasecmp(value, "tag.", 4)) {
            parser_conf.dom_parse.v_tag.push_back(std::make_pair(key, value+4));
        }
    }
}

void parser_regex_conf(collection_item *config, char *section, parser_conf_t &parser_conf)
{
    char value[MAX_CHAR_SIZE];
    const char *key;
    int i = 0;
    while((key = g_fields_text[i++]) != NULL) {
        conf_string(config, section, key, "", value, MAX_CHAR_SIZE);
        if (strlen(value)) {
            parser_conf.regex_parse.v_regex.push_back(std::make_pair(key, value));
        }
    }
    return ;
}

void parser_block_conf(collection_item *config, char *section, parser_conf_t &parser_conf)
{
    conf_int(config, section, "block_size", 3, &(parser_conf.block_parse.block_size));
    conf_int(config, section, "word_threshold", 86, &(parser_conf.block_parse.word_threshold));
    return ;
}

void filter_conf_init(collection_item *config, char *section, parser_conf_t &parser_conf)
{
    char value[MAX_CHAR_SIZE];
    conf_string(config, section, "url_include", "", value, MAX_CHAR_SIZE);
    filter_conf(parser_conf.url_filter._include_charset, value);
    conf_string(config, section, "url_exclude", "", value, MAX_CHAR_SIZE);
    filter_conf(parser_conf.url_filter._exclude_charset, value);
    conf_string(config, section, "url_delete", "", value, MAX_CHAR_SIZE);
    filter_conf(parser_conf.url_filter._delete_charset, value);

    conf_string(config, section, "content_include", "", value, MAX_CHAR_SIZE);
    filter_conf(parser_conf.content_filter._include_charset, value);
    conf_string(config, section, "content_exclude", "", value, MAX_CHAR_SIZE);
    filter_conf(parser_conf.content_filter._exclude_charset, value);
    conf_string(config, section, "content_delete", "", value, MAX_CHAR_SIZE);
    filter_conf(parser_conf.content_filter._delete_charset, value);
}

void filter_conf(std::vector<std::string> &vFilterCharset, char *value)
{
    char *pFilter = NULL;

    while((pFilter = strtok(pFilter ? NULL : value, "; "))) {
        vFilterCharset.push_back(pFilter);
    }
    return ;
}

void store_log_conf(collection_item *config, char *section, store_conf_t &store_conf)
{
    store_conf.type = E_STORE_LOG;
    conf_string(config, section, "path", "", store_conf.v.store_log.path, MAX_CHAR_SIZE);
    conf_string(config, section, "file", "", store_conf.v.store_log.file, MAX_CHAR_SIZE);

}

void store_mysql_conf(collection_item *config, char *section, store_conf_t &store_conf)
{
    store_conf.type = E_STORE_MYSQL;
    conf_string(config, section, "user_name", "", store_conf.v.store_mysql.user_name, MAX_CHAR_SIZE);
    conf_string(config, section, "user_pass", "", store_conf.v.store_mysql.user_pass, MAX_CHAR_SIZE);
    conf_string(config, section, "db_name", "", store_conf.v.store_mysql.db_name, MAX_CHAR_SIZE);
    conf_string(config, section, "table_name", "", store_conf.v.store_mysql.table_name, MAX_CHAR_SIZE);
}

void store_hbase_conf(collection_item *config, char *section, store_conf_t &store_conf)
{
    store_conf.type = E_STORE_HBASE;
    conf_string(config, section, "path", "", store_conf.v.store_hbase.path, MAX_CHAR_SIZE);
    conf_string(config, section, "file", "", store_conf.v.store_hbase.file, MAX_CHAR_SIZE);
}

void store_thrift_conf(collection_item *config, char *section, store_conf_t &store_conf)
{
    store_conf.type = E_STORE_THRIFT;
    conf_string(config, section, "host_port", "", store_conf.v.store_thrift.host_port, MAX_CHAR_SIZE);
}

void store_local_conf(collection_item *config, char *section, store_conf_t &store_conf)
{
    store_conf.type = E_STORE_LOCAL;
    conf_string(config, section, "path", "", store_conf.v.store_local.path, MAX_CHAR_SIZE);
}

void store_crawl_conf(collection_item *config, char *section, store_conf_t &store_conf)
{
    store_conf.type = E_STORE_CRAWL;
}

} // namespace
