/* 
 * Copyright (c) 2013 izptec.com, Inc. 
 * 
 * 
 */

#include "thrift_client.h"

#if 0
#include "cra_utils.h"
#include "crawler_conf.h"
#endif

#include "log.h"

using std::vector;
using std::string;

using boost::shared_ptr;

#if 0
extern g_conf_t g_conf;
#endif

//冒号用于分割host, port, path.
static 
int is_valid_char(char p)
{
	if (p != ':') return 1;
	else return 0;	
}

ThriftClient::ThriftClient(const string &serv_addr)
{

    SetServAddr(serv_addr);
#if 0
    m_socket = boost::shared_ptr<TSocket>(new TSocket(m_serv_addr.at(0).host, m_serv_addr.at(0).port));
#endif
    m_socket = boost::shared_ptr<TSocket>(new TSocket("10.0.2.43", 9091));
    m_transport = boost::shared_ptr<TFramedTransport>(new TFramedTransport(m_socket));
    m_protocol = boost::shared_ptr<TBinaryProtocol>(new TBinaryProtocol(m_transport));
#if 0
    m_socket->setConnTimeout((int)g_conf.url_serv_timeout);
    m_socket->setRecvTimeout((int)g_conf.url_serv_timeout);
    m_socket->setSendTimeout((int)g_conf.url_serv_timeout);
#endif

}

ThriftClient::~ThriftClient()
{
}

//解析服务地址
int ThriftClient::SetServAddr(const string &serv_addr)
{
#if 0
    if (serv_addr.empty()){
        FATAL("ThriftClient::ThriftClient FAIL, serv_addr is empty");
        exit(-1);
    }
    vector<string> vec_str;
    tokenize(serv_addr, vec_str, &is_valid_char);
    ServAddrPath tmp_serv_addr;
    tmp_serv_addr.host = vec_str.at(0);
    tmp_serv_addr.port = atoi(vec_str.at(1).c_str());
    DEBUG("host:%s, port:%d", tmp_serv_addr.host.c_str(), tmp_serv_addr.port);
    m_serv_addr.push_back(tmp_serv_addr);
    m_connflag = (m_serv_addr.size() > 0) ? CONNECT : IGNORE;
#endif
    m_connflag = CONNECT;
    return 0;
} 

int ThriftClient::TransportOpen()
{
	if (IGNORE == m_connflag) {
		return -1;
	}
    try {
		m_transport->open();
		if (!CheckConnect()) {
			return 0;
		} else {
			return -1;
		}
    } catch (TException e) {
    	FATAL("ThriftClient::TransportOpen() error:%s", e.what());
    	return -1;
    }
}

int ThriftClient::TransportClose()
{
	if (IGNORE == m_connflag) {
		return 0;
	} 
	try {
		m_transport->close();
		return 0;
	} catch (TException e) {
		FATAL("ThriftClient::TransportClose(), error:%s", e.what());
		return -1;
	}
}

int ThriftClient::CheckConnect()
{
	if (IGNORE == m_connflag) {
		WARN("connect is ignored");
		return 1;
	}
	try {
		if (true == m_transport->isOpen()) {
			return 0;
		} else {
			//WARN("transport is not open");
			return -1;
		}
	} catch (TException e) {
		FATAL("ThriftClient::CheckConnect() error:%s", e.what());
		return -1;
	}
}
