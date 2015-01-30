/* 
 * Copyright (c) 2013 izptec.com, Inc. 
 * 
 * 
 */

#ifndef __THRIFT_CLIENT_H__
#define __THRIFT_CLIENT_H__

#include <arpa/inet.h>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <protocol/TBinaryProtocol.h>

#include <vector>
#include <string>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

enum ConnFlag 
{
	CONNECT,
	IGNORE,
};

struct ServAddrPath
{
	std::string host;
	int 		port;
	std::string path;	
};

class ThriftClient
{
public:
	explicit ThriftClient(const std::string &serv_addr);
	~ThriftClient();
	int TransportOpen();
	int TransportClose();
	int SetServAddr(const std::string &serv_addr);
	int CheckConnect();
	
protected:
    boost::shared_ptr<TSocket> m_socket;
	boost::shared_ptr<TTransport> m_transport;
	boost::shared_ptr<TBinaryProtocol> m_protocol;

private:
    std::vector<ServAddrPath> m_serv_addr;
	ConnFlag m_connflag;
};

#endif
