#ifndef __SYLAR_HTTP_SESSION_H__
#define __SYLAR_HTTP_SESSION_H__

#include "socket_stream.h"
#include"http.h"
#include "sockett.h"

namespace sylar{

namespace http {



class HttpSession: public SocketStream{
public:
    using ptr = std::shared_ptr<HttpSession>;
    HttpSession(Socket::ptr sock, bool owner = true);
    http::HttpRequest::ptr recvRequest();
    int sendResponse(http::HttpResponse::ptr rsp);

};




}

}


#endif