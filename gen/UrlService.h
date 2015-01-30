/**
 * Autogenerated by Thrift Compiler (0.8.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef UrlService_H
#define UrlService_H

#include <TProcessor.h>
#include "url_types.h"

namespace radar {

class UrlServiceIf {
 public:
  virtual ~UrlServiceIf() {}
  virtual void GetUrls(std::vector<Url> & _return, const int32_t num) = 0;
  virtual int32_t SendParserUrls(const std::vector<ParserUrl> & urls) = 0;
  virtual int32_t SendFailedUrls(const std::vector<FailedUrl> & urls) = 0;
};

class UrlServiceIfFactory {
 public:
  typedef UrlServiceIf Handler;

  virtual ~UrlServiceIfFactory() {}

  virtual UrlServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(UrlServiceIf* /* handler */) = 0;
};

class UrlServiceIfSingletonFactory : virtual public UrlServiceIfFactory {
 public:
  UrlServiceIfSingletonFactory(const boost::shared_ptr<UrlServiceIf>& iface) : iface_(iface) {}
  virtual ~UrlServiceIfSingletonFactory() {}

  virtual UrlServiceIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(UrlServiceIf* /* handler */) {}

 protected:
  boost::shared_ptr<UrlServiceIf> iface_;
};

class UrlServiceNull : virtual public UrlServiceIf {
 public:
  virtual ~UrlServiceNull() {}
  void GetUrls(std::vector<Url> & /* _return */, const int32_t /* num */) {
    return;
  }
  int32_t SendParserUrls(const std::vector<ParserUrl> & /* urls */) {
    int32_t _return = 0;
    return _return;
  }
  int32_t SendFailedUrls(const std::vector<FailedUrl> & /* urls */) {
    int32_t _return = 0;
    return _return;
  }
};

typedef struct _UrlService_GetUrls_args__isset {
  _UrlService_GetUrls_args__isset() : num(false) {}
  bool num;
} _UrlService_GetUrls_args__isset;

class UrlService_GetUrls_args {
 public:

  UrlService_GetUrls_args() : num(0) {
  }

  virtual ~UrlService_GetUrls_args() throw() {}

  int32_t num;

  _UrlService_GetUrls_args__isset __isset;

  void __set_num(const int32_t val) {
    num = val;
  }

  bool operator == (const UrlService_GetUrls_args & rhs) const
  {
    if (!(num == rhs.num))
      return false;
    return true;
  }
  bool operator != (const UrlService_GetUrls_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UrlService_GetUrls_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class UrlService_GetUrls_pargs {
 public:


  virtual ~UrlService_GetUrls_pargs() throw() {}

  const int32_t* num;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _UrlService_GetUrls_result__isset {
  _UrlService_GetUrls_result__isset() : success(false) {}
  bool success;
} _UrlService_GetUrls_result__isset;

class UrlService_GetUrls_result {
 public:

  UrlService_GetUrls_result() {
  }

  virtual ~UrlService_GetUrls_result() throw() {}

  std::vector<Url>  success;

  _UrlService_GetUrls_result__isset __isset;

  void __set_success(const std::vector<Url> & val) {
    success = val;
  }

  bool operator == (const UrlService_GetUrls_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const UrlService_GetUrls_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UrlService_GetUrls_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _UrlService_GetUrls_presult__isset {
  _UrlService_GetUrls_presult__isset() : success(false) {}
  bool success;
} _UrlService_GetUrls_presult__isset;

class UrlService_GetUrls_presult {
 public:


  virtual ~UrlService_GetUrls_presult() throw() {}

  std::vector<Url> * success;

  _UrlService_GetUrls_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _UrlService_SendParserUrls_args__isset {
  _UrlService_SendParserUrls_args__isset() : urls(false) {}
  bool urls;
} _UrlService_SendParserUrls_args__isset;

class UrlService_SendParserUrls_args {
 public:

  UrlService_SendParserUrls_args() {
  }

  virtual ~UrlService_SendParserUrls_args() throw() {}

  std::vector<ParserUrl>  urls;

  _UrlService_SendParserUrls_args__isset __isset;

  void __set_urls(const std::vector<ParserUrl> & val) {
    urls = val;
  }

  bool operator == (const UrlService_SendParserUrls_args & rhs) const
  {
    if (!(urls == rhs.urls))
      return false;
    return true;
  }
  bool operator != (const UrlService_SendParserUrls_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UrlService_SendParserUrls_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class UrlService_SendParserUrls_pargs {
 public:


  virtual ~UrlService_SendParserUrls_pargs() throw() {}

  const std::vector<ParserUrl> * urls;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _UrlService_SendParserUrls_result__isset {
  _UrlService_SendParserUrls_result__isset() : success(false) {}
  bool success;
} _UrlService_SendParserUrls_result__isset;

class UrlService_SendParserUrls_result {
 public:

  UrlService_SendParserUrls_result() : success(0) {
  }

  virtual ~UrlService_SendParserUrls_result() throw() {}

  int32_t success;

  _UrlService_SendParserUrls_result__isset __isset;

  void __set_success(const int32_t val) {
    success = val;
  }

  bool operator == (const UrlService_SendParserUrls_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const UrlService_SendParserUrls_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UrlService_SendParserUrls_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _UrlService_SendParserUrls_presult__isset {
  _UrlService_SendParserUrls_presult__isset() : success(false) {}
  bool success;
} _UrlService_SendParserUrls_presult__isset;

class UrlService_SendParserUrls_presult {
 public:


  virtual ~UrlService_SendParserUrls_presult() throw() {}

  int32_t* success;

  _UrlService_SendParserUrls_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _UrlService_SendFailedUrls_args__isset {
  _UrlService_SendFailedUrls_args__isset() : urls(false) {}
  bool urls;
} _UrlService_SendFailedUrls_args__isset;

class UrlService_SendFailedUrls_args {
 public:

  UrlService_SendFailedUrls_args() {
  }

  virtual ~UrlService_SendFailedUrls_args() throw() {}

  std::vector<FailedUrl>  urls;

  _UrlService_SendFailedUrls_args__isset __isset;

  void __set_urls(const std::vector<FailedUrl> & val) {
    urls = val;
  }

  bool operator == (const UrlService_SendFailedUrls_args & rhs) const
  {
    if (!(urls == rhs.urls))
      return false;
    return true;
  }
  bool operator != (const UrlService_SendFailedUrls_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UrlService_SendFailedUrls_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class UrlService_SendFailedUrls_pargs {
 public:


  virtual ~UrlService_SendFailedUrls_pargs() throw() {}

  const std::vector<FailedUrl> * urls;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _UrlService_SendFailedUrls_result__isset {
  _UrlService_SendFailedUrls_result__isset() : success(false) {}
  bool success;
} _UrlService_SendFailedUrls_result__isset;

class UrlService_SendFailedUrls_result {
 public:

  UrlService_SendFailedUrls_result() : success(0) {
  }

  virtual ~UrlService_SendFailedUrls_result() throw() {}

  int32_t success;

  _UrlService_SendFailedUrls_result__isset __isset;

  void __set_success(const int32_t val) {
    success = val;
  }

  bool operator == (const UrlService_SendFailedUrls_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const UrlService_SendFailedUrls_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const UrlService_SendFailedUrls_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _UrlService_SendFailedUrls_presult__isset {
  _UrlService_SendFailedUrls_presult__isset() : success(false) {}
  bool success;
} _UrlService_SendFailedUrls_presult__isset;

class UrlService_SendFailedUrls_presult {
 public:


  virtual ~UrlService_SendFailedUrls_presult() throw() {}

  int32_t* success;

  _UrlService_SendFailedUrls_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class UrlServiceClient : virtual public UrlServiceIf {
 public:
  UrlServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  UrlServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void GetUrls(std::vector<Url> & _return, const int32_t num);
  void send_GetUrls(const int32_t num);
  void recv_GetUrls(std::vector<Url> & _return);
  int32_t SendParserUrls(const std::vector<ParserUrl> & urls);
  void send_SendParserUrls(const std::vector<ParserUrl> & urls);
  int32_t recv_SendParserUrls();
  int32_t SendFailedUrls(const std::vector<FailedUrl> & urls);
  void send_SendFailedUrls(const std::vector<FailedUrl> & urls);
  int32_t recv_SendFailedUrls();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class UrlServiceProcessor : public ::apache::thrift::TProcessor {
 protected:
  boost::shared_ptr<UrlServiceIf> iface_;
  virtual bool process_fn(apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, std::string& fname, int32_t seqid, void* callContext);
 private:
  std::map<std::string, void (UrlServiceProcessor::*)(int32_t, apache::thrift::protocol::TProtocol*, apache::thrift::protocol::TProtocol*, void*)> processMap_;
  void process_GetUrls(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_SendParserUrls(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_SendFailedUrls(int32_t seqid, apache::thrift::protocol::TProtocol* iprot, apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  UrlServiceProcessor(boost::shared_ptr<UrlServiceIf> iface) :
    iface_(iface) {
    processMap_["GetUrls"] = &UrlServiceProcessor::process_GetUrls;
    processMap_["SendParserUrls"] = &UrlServiceProcessor::process_SendParserUrls;
    processMap_["SendFailedUrls"] = &UrlServiceProcessor::process_SendFailedUrls;
  }

  virtual bool process(boost::shared_ptr<apache::thrift::protocol::TProtocol> piprot, boost::shared_ptr<apache::thrift::protocol::TProtocol> poprot, void* callContext);
  virtual ~UrlServiceProcessor() {}
};

class UrlServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  UrlServiceProcessorFactory(const ::boost::shared_ptr< UrlServiceIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< UrlServiceIfFactory > handlerFactory_;
};

class UrlServiceMultiface : virtual public UrlServiceIf {
 public:
  UrlServiceMultiface(std::vector<boost::shared_ptr<UrlServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~UrlServiceMultiface() {}
 protected:
  std::vector<boost::shared_ptr<UrlServiceIf> > ifaces_;
  UrlServiceMultiface() {}
  void add(boost::shared_ptr<UrlServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void GetUrls(std::vector<Url> & _return, const int32_t num) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        ifaces_[i]->GetUrls(_return, num);
        return;
      } else {
        ifaces_[i]->GetUrls(_return, num);
      }
    }
  }

  int32_t SendParserUrls(const std::vector<ParserUrl> & urls) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        return ifaces_[i]->SendParserUrls(urls);
      } else {
        ifaces_[i]->SendParserUrls(urls);
      }
    }
  }

  int32_t SendFailedUrls(const std::vector<FailedUrl> & urls) {
    size_t sz = ifaces_.size();
    for (size_t i = 0; i < sz; ++i) {
      if (i == sz - 1) {
        return ifaces_[i]->SendFailedUrls(urls);
      } else {
        ifaces_[i]->SendFailedUrls(urls);
      }
    }
  }

};

} // namespace

#endif