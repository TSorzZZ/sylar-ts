#ifndef __SYLAR_HTTP_SERVER_H__
#define __SYLAR_HTTP_SERVER_H__

#include "iomanager.h"
#include "servlet.h"
#include "sockett.h"
#include "tcp_server.h"
#include "http_session.h"
#include <memory>

namespace sylar{

namespace http{


class HttpServer: public TcpServer{
public:
    using ptr = std::shared_ptr<HttpServer>;
    HttpServer(bool keepAlive = false, IOManager* worker = IOManager::GetThis(),
               IOManager* accept_worker = IOManager::GetThis());

    ServletDispatch::ptr getServletDispatch() const {return m_dispatch;}
    void setServletDispatch(ServletDispatch::ptr v) { m_dispatch = v;}
protected:
    virtual void handleClient(Socket::ptr client) override;

private:
    bool m_isKeepalive;
    ServletDispatch::ptr m_dispatch;
};


}

}

#endif