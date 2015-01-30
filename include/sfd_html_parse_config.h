#ifndef SFD_HTML_PARSE_CONFIG
#define SFD_HTML_PARSE_CONFIG

#include "conf.h"
#include "sfd_parsestd.h"
#include <pthread.h>

namespace sfd {

#define MAX_CHAR_SIZE 1024
enum EnumStore_t {
    E_STORE_LOG,
    E_STORE_MYSQL,
    E_STORE_HBASE,
    E_STORE_THRIFT,
    E_STORE_LOCAL,
    E_STORE_CRAWL
};


struct store_crawl_conf_t {

};

// 通过 thrift 与 url server 进行交互
struct store_thrift_conf_t {
    char host_port[MAX_CHAR_SIZE];
};

struct store_mysql_conf_t {
    char user_name[MAX_CHAR_SIZE];
    char user_pass[MAX_CHAR_SIZE];
    char db_name[MAX_CHAR_SIZE];
    char table_name[MAX_CHAR_SIZE];
};

struct store_log_conf_t {
    char path[MAX_CHAR_SIZE];
    char file[MAX_CHAR_SIZE];
};

struct store_hbase_conf_t {
    char path[MAX_CHAR_SIZE];
    char file[MAX_CHAR_SIZE];
};

struct store_local_conf_t {
    char path[MAX_CHAR_SIZE];
};

struct store_conf_t {
    EnumStore_t type;
    union {
        store_log_conf_t store_log;
        store_mysql_conf_t store_mysql;
        store_hbase_conf_t store_hbase;
        store_thrift_conf_t store_thrift;
        store_local_conf_t store_local;
        store_crawl_conf_t store_crawl;
    }v;
};

struct filter_conf_t {
    std::vector<std::string> _include_charset;
    std::vector<std::string> _exclude_charset;
    std::vector<std::string> _delete_charset;
};

struct regex_parser_conf_t {
    std::vector < std::pair<std::string, std::string> > v_regex;
};

struct dom_parser_conf_t {
    std::vector < std::pair<std::string, std::string> > v_meta;
    std::vector < std::pair<std::string, std::string> > v_tag;
};

struct block_parser_conf_t {
    int block_size;
    int word_threshold;
};

struct parser_conf_t {
    url_t seed_url;
    regex_parser_conf_t regex_parse;
    dom_parser_conf_t dom_parse;
    block_parser_conf_t block_parse;
    filter_conf_t url_filter;
    filter_conf_t content_filter;
};


struct parse_sys_conf_t {
    char fpath[MAX_CHAR_SIZE];
    char fname[MAX_CHAR_SIZE];
    int thread_num;
    int seed_url_num;
    std::vector <store_conf_t> store_conf;
    std::vector <parser_conf_t> parse_conf;
};

// 测试配置用
void debug_parser_config();

bool parse_sys_conf_init(char *fpath, char *fname);
void seed_url_init(struct collection_item *config, parse_sys_conf_t &parse_sys_conf);

void parser_conf_init(struct collection_item *config, char *section, parser_conf_t &parser_conf);
void parser_dom_conf(struct collection_item *config, char *section, parser_conf_t &parser_conf);
void parser_regex_conf(struct collection_item *config, char *section, parser_conf_t &parser_conf);
void parser_block_conf(struct collection_item *config, char *section, parser_conf_t &parser_conf);

void filter_conf_init(struct collection_item *config, char *section, parser_conf_t &parser_conf);
void filter_conf(std::vector<std::string> &vFilterCharset, char *value);

void store_conf_init(struct collection_item *config, parse_sys_conf_t &parse_sys_conf);
void store_log_conf(struct collection_item *config, char *section, store_conf_t &store_conf);
void store_mysql_conf(struct collection_item *config, char *section, store_conf_t &store_conf);
void store_hbase_conf(struct collection_item *config, char *section, store_conf_t &store_conf);
void store_thrift_conf(struct collection_item *config, char *section, store_conf_t &store_conf);
void store_local_conf(struct collection_item *config, char *section, store_conf_t &store_conf);
void store_crawl_conf(struct collection_item *config, char *section, store_conf_t &store_conf);
extern parse_sys_conf_t g_parse_sys_conf;

} // namespcae

#endif // SFD_HTML_PARSE_CONF
