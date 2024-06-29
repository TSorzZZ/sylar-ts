
#include "ws_server.h"
#include "http.h"
#include"log.h"
#include "tcp_server.h"
#include "ws_servlet.h"


namespace sylar {
namespace http {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

WSServer::WSServer(sylar::IOManager* worker,
                   sylar::IOManager* accept_worker):TcpServer(worker, accept_worker){
    m_dispatch.reset(new WSServletDispatch);
}

//一个client对应一个会话 也就是session
void WSServer::handleClient(Socket::ptr client){
    SYLAR_LOG_DEBUG(g_logger) << "handleClient " << *client;
    WSSession::ptr session(new WSSession(client));
    do {
        //完成握手 获得请求
        HttpRequest::ptr header = session->handleShake();
        if(!header) {
            SYLAR_LOG_DEBUG(g_logger) << "handleShake error";
            break;
        }

        //获得对应的servlet
        WSServlet::ptr servlet = m_dispatch->getWSServlet(header->getPath());
        if(!servlet) {
            SYLAR_LOG_DEBUG(g_logger) << "no match WSServlet";
            break;
        }

        //执行连接后的回调
        int rt = servlet->onConnect(header, session);
        if(rt) {
            SYLAR_LOG_DEBUG(g_logger) << "onConnect return " << rt;
            break;
        }


        //接受数据   servlet执行服务的回调函数
        while(true) {
            auto msg = session->recvMessage();
            if(!msg) {
                break;
            }
            rt = servlet->handle(header, msg, session);
            if(rt) {
                SYLAR_LOG_DEBUG(g_logger) << "handle return " << rt;
                break;
            }
        }
        //执行关闭连接的回调函数
        servlet->onClose(header, session);

    }while (0);
    
    session->close();
}


}

}