#include "sfd_html_parse.h"
#include "log.h"

namespace sfd {

static void *parse_thread_fn(void *arg);

SfdHtmlParse::SfdHtmlParse()
{

}

SfdHtmlParse::~SfdHtmlParse()
{
    for (std::vector<html_parser_t*>::iterator it = _html_parser.begin();
         it != _html_parser.end(); ++it) {
        delete *it;
    }

    for (std::vector<Store*>::iterator it = _store.begin(); it != _store.end(); ++it) {
        delete *it;
    }
}

void SfdHtmlParse::initSfdHtmlParse(parse_sys_conf_t &parse_sys_conf)
{
    initStore(parse_sys_conf.store_conf);
    initParser(parse_sys_conf.parse_conf);
}

void SfdHtmlParse::initStore(std::vector<store_conf_t> &store_conf)
{
    Store* store;
    for (std::vector<store_conf_t>::iterator it = store_conf.begin(); it != store_conf.end(); ++it) {
        store_conf_t &store_c = *it;
        switch (store_c.type) {
        case E_STORE_LOG:
            store = create_log_store(store_c.v.store_log);
            break;
        case E_STORE_MYSQL:
            store = create_mysql_store(store_c.v.store_mysql);
            break;
        case E_STORE_THRIFT:
            store = create_thrift_store(store_c.v.store_thrift);
            break;
        case E_STORE_HBASE:
            store = create_hbase_store(store_c.v.store_hbase);
            break;
        case E_STORE_LOCAL:
            store = create_local_store(store_c.v.store_local);
            break;
        case E_STORE_CRAWL:
            store = create_crawl_store(store_c.v.store_crawl);
            break;
        default:
            FATAL("bad store mode %d", store_c.type);
            break;
        }
        if (store != NULL) {
            _store.push_back(store);
        } else {
            FATAL("create store[%d] error", store_c.type);
        }
    }
}


bool SfdHtmlParse::read_data()
{
    Store *store;
    for (std::vector<Store*>::iterator it = _store.begin(); it != _store.end(); ++it) {
        store = *it;
        if (store->readEnabled() && 0 == store->readHTML(_html)) {
            return true;
        }
    }
    return false;
}

bool SfdHtmlParse::write_data()
{
    Store *store;
    for (std::vector<Store*>::iterator it = _store.begin(); it != _store.end(); ++it) {
        store = *it;
        if (store->writeURLSEnabled()) {
            store->writeURLS(_html, _urls);
        }
        if (store->writeTEXTSEnabled()) {
            store->writeTEXTS(_html, _texts);
        }
    }
    return true;
}

void SfdHtmlParse::run()
{
    html_parser_t *html_parser;
    Parser *parser;
    if (read_data()) {
        _urls.clear();
        _texts.clear();
        html_parser = get_html_parser(_html.url.seed_id);
        for (std::vector<Parser*>::iterator it = html_parser->parser.begin();
             it != html_parser->parser.end(); ++it) {
            parser = *it;
            parser->set_html(_html);
            parser->parse();
            if (html_parser->url_seed.depth < 0 || _html.url.depth < html_parser->url_seed.depth) {
                add_urls(_urls, parser->get_urls());
            }
            add_contents(_texts, parser->get_context());
        }
        write_data();
    } else {
        sleep(1);
    }
    DEBUG("sfd_html_parse run");
}

void SfdHtmlParse::initParser(std::vector<parser_conf_t> &parser_conf)
{
    Parser *parser;
    html_parser_t *html_parser;
    for (std::vector<parser_conf_t>::iterator it = parser_conf.begin(); it != parser_conf.end(); ++it) {
        parser_conf_t &parser_c = *it;
        html_parser = new html_parser_t();
        html_parser->url_seed = parser_c.seed_url;
        html_parser->url_filter = new Filter(parser_c.url_filter);
        html_parser->content_filter = new Filter(parser_c.content_filter);

        parser = create_block_parser(parser_c.block_parse);
        if (parser != NULL) {
            parser->set_url_filter(html_parser->url_filter);
            parser->set_content_filter(html_parser->content_filter);
            html_parser->parser.push_back(parser);
        }

        parser = create_dom_parser(parser_c.dom_parse);
        if (parser != NULL) {
            parser->set_url_filter(html_parser->url_filter);
            parser->set_content_filter(html_parser->content_filter);
            html_parser->parser.push_back(parser);
        }

        parser = create_regex_parser(parser_c.regex_parse);
        if (parser != NULL) {
            parser->set_url_filter(html_parser->url_filter);
            parser->set_content_filter(html_parser->content_filter);
            html_parser->parser.push_back(parser);
        }

        if (html_parser->parser.size() == 0) {
            parser = new BlockParser();
            if (parser != NULL) {
                parser->set_url_filter(html_parser->url_filter);
                parser->set_content_filter(html_parser->content_filter);
                html_parser->parser.push_back(parser);
            }
        }
        _html_parser.push_back(html_parser);
    }
    // 添加一个默认的解析器
    html_parser = new html_parser_t();
    parser = new BlockParser();
    html_parser->url_seed.depth = -1;
    html_parser->url_seed.expand_way = BY_NOLIMIT;
    html_parser->url_filter = new Filter();
    html_parser->content_filter = new Filter();
    parser->set_url_filter(html_parser->url_filter);
    parser->set_content_filter(html_parser->content_filter);
    html_parser->parser.push_back(parser);
    _html_parser.push_back(html_parser);
}

html_parser_t* SfdHtmlParse::get_html_parser(int64_t seed_id)
{
    if (seed_id < 0 && (size_t)seed_id < _html_parser.size()) {
        return _html_parser[seed_id -1];
    }
    return _html_parser[_html_parser.size()-1];
}

static pthread_t* ptid;
static bool bStop;
void *parse_thread_fn(void *arg)
{
    pthread_t* tid = (pthread_t*)arg;
    SfdHtmlParse htmlParse;
    htmlParse.initSfdHtmlParse(g_parse_sys_conf);
    INFO("the %d thread run", *tid);
    while(!bStop) {
        htmlParse.run();
    }
    return NULL;
}

void parse_thread_start()
{
    bStop = false;
    parse_sys_conf_init("./conf", "parser");
    if (ptid != NULL) {
        free(ptid);
    }
    ptid = (pthread_t*)malloc(sizeof(pthread_t) * g_parse_sys_conf.thread_num);
    if (ptid == NULL) {
        FATAL("parse_thread_start malloc error");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < g_parse_sys_conf.thread_num; ++i) {
        pthread_create(&(ptid[i]), NULL, parse_thread_fn, &(ptid[i]));
    }
}

void parse_thread_join()
{
    for (int i = 0; i < g_parse_sys_conf.thread_num; ++i) {
        pthread_join(ptid[i], NULL);
    }
    free(ptid);
    ptid = NULL;
}

} // namespace

