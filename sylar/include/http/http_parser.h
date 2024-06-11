#ifndef SYLAR_HTTP_PARSER_H__
#define SYLAR_HTTP_PARSER_H__

#include "http11_parser.h"
#include "httpclient_parser.h"
#include "http.h"
#include <memory>
namespace sylar {

namespace http {



class Parser{
public:
    virtual ~Parser();

    int isFinished() const;
    int hasError() const;
    size_t execute(const char* data, size_t len, size_t off);
private:

};

class HttpRequestParser{
public:
    using ptr = std::shared_ptr<HttpRequestParser>;
    HttpRequestParser();

    int isFinished() ;
    int hasError() ;
    size_t execute(char* data, size_t len);
    HttpRequest::ptr getData() const{return  m_data;}
    void setError(int v){m_error = v;}
    uint64_t getContentLength();

    static uint64_t GetHttpRequestBufferSize();
    static uint64_t GetHttpRequestMaxBodySize();
    const http_parser& getParser() const { return m_parser;}
private:
    http_parser m_parser;
    HttpRequest::ptr m_data;
    // 1000: invalid method
    // 1001: invalid version
    // 1002: invalid field
    int m_error;
};

class HttpResponseParser{
public:
    using ptr = std::shared_ptr<HttpResponseParser>;
    HttpResponseParser();

    int isFinished() ;
    int hasError() ;
    size_t execute(char* data, size_t len, bool chunck);

    HttpResponse::ptr getData() const{return  m_data;}
    void setError(int v){m_error = v;}
    uint64_t getContentLength();

    
    static uint64_t GetHttpResponseBufferSize();
    static uint64_t GetHttpResponseMaxBodySize();
    const httpclient_parser& getParser() const { return m_parser;}
private:
    httpclient_parser m_parser;
    HttpResponse::ptr m_data;
    int m_error;
};


}//http

}//sylar

#endif
