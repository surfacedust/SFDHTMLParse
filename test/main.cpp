#include "sfd_html_parse.h"
#include "sfd_html_parse_config.h"

#include "log.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
using namespace std;
extern const char* g_fields_text[];


int main()
{
    log_init("./conf", "log");
    sfd::parse_thread_start();
    sfd::parse_thread_join();
    sfd::parse_sys_conf_init("./conf", "parser");
    sfd::SfdHtmlParse htmlParse;
    htmlParse.initSfdHtmlParse(sfd::g_parse_sys_conf);
    htmlParse.run();
    log_close();

    return 0;
}

