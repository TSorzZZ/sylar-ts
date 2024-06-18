#include"http_session.h"
#include "http_parser.h"
#include "socket_stream.h"
#include <cstdint>
#include <memory>
#include <string>

namespace sylar {

namespace http {


HttpSession::HttpSession(Socket::ptr sock, bool owner):SocketStream(sock, owner){

}

HttpRequest::ptr HttpSession::recvRequest() {
    HttpRequestParser::ptr parser(new HttpRequestParser);
    uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
    //uint64_t buff_size = 100;
    std::shared_ptr<char> buffer(
            new char[buff_size], [](char* ptr){
                delete[] ptr;
            });
    char* data = buffer.get();
    int offset = 0;
    //读请求并解析
    do {
        //缓冲区读到http报文  从缓冲区中拿到http报文
        int len = read(data + offset, buff_size - offset);
        if(len <= 0) {
            close();
            return nullptr;
        }
        len += offset;
        //解析http请求
        size_t nparse = parser->execute(data, len);
        if(parser->hasError()) {
            close();
            return nullptr;
        }
        offset = len - nparse;
        if(offset == (int)buff_size) {
            close();
            return nullptr;
        }
        if(parser->isFinished()) {
            break;
        }
    } while(true);
    int64_t length = parser->getContentLength();
    //保存请求体
    if(length > 0) {
        std::string body;
        body.resize(length);

        int len = 0;
        if(length >= offset) {
            memcpy(&body[0], data, offset);
            len = offset;
        } else {
            memcpy(&body[0], data, length);
            len = length;
        }
        length -= offset;
        //从缓冲区中拿到请求体
        if(length > 0) {
            if(readFixSize(&body[len], length) <= 0) {
                close();
                return nullptr;
            }
        }
        parser->getData()->setBody(body);
    }
    std::string keep_alive = parser->getData()->getHeader("Connection");
    
    if(!strcasecmp(keep_alive.c_str(), "keep-alive")) {
        parser->getData()->setClose(false);
    }
    //parser->getData()->init();
    return parser->getData();
}

int HttpSession::sendResponse(HttpResponse::ptr rsp) {
    std::stringstream ss;
    ss << *rsp;
    std::string data = ss.str();
    return writeFixSize(data.c_str(), data.size());
}





}

}