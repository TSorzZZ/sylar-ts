#include "http_server.h"
#include "http.h"
#include "http_session.h"
#include "servlet.h"
#include "tcp_server.h"
#include "log.h"
#include <cerrno>
#include <cstring>

namespace sylar {

namespace http {

static Logger::ptr g_logger = SYLAR_LOG_NAME("system");

HttpServer::HttpServer(bool keepAlive, IOManager* worker, IOManager* accept_worker)
    :TcpServer(worker, accept_worker), m_isKeepalive(keepAlive){
    m_dispatch.reset(new ServletDispatch); 
}

void HttpServer::handleClient(Socket::ptr client){
    HttpSession::ptr session(new HttpSession(client));
    do{
        auto req = session->recvRequest();
        if(!req){
            SYLAR_LOG_WARN(g_logger) << "recv http request fail, errno=" << errno 
                                     << "errstr=" << strerror(errno) << " client: " << *client;
            break;
        }


        HttpResponse::ptr rsp( new HttpResponse(req->getVersion(), req->isClose() || !m_isKeepalive));
        m_dispatch->handle(req, rsp, session);
        
        // rsp->setBody("hello sylar");

        // SYLAR_LOG_INFO(g_logger) << "request:" << std:: endl << *req;
        // SYLAR_LOG_INFO(g_logger) << "response:" << std:: endl << *rsp;
        session->sendResponse(rsp); 
    }while(m_isKeepalive);
    session->close();
}


}

}