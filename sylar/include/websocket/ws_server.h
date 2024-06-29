#ifndef __SYLAR_WS_SERVER_H__
#define __SYLAR_WS_SERVER_H__



#include "tcp_server.h"
#include "ws_servlet.h"
#include <memory>
namespace sylar {

namespace http {


class WSServer: public TcpServer{
public:
    using ptr = std::shared_ptr<WSServer>;

    WSServer(sylar::IOManager* worker = sylar::IOManager::GetThis()
                , sylar::IOManager* accept_worker = sylar::IOManager::GetThis());

    WSServletDispatch::ptr getWSServletDispatch() const { return m_dispatch;}
    void setWSServletDispatch(WSServletDispatch::ptr v) { m_dispatch = v;}


protected:
    virtual void handleClient(Socket::ptr client) override;
protected:
    WSServletDispatch::ptr m_dispatch;
};


}

}




#endif