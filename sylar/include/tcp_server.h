#ifndef __SYLAR_TCP_SERVER_H__
#define __SYLAR_TCP_SERVER_H__

#include "address.h"
#include "iomanager.h"
#include "noncopyable.h"
#include "sockett.h"
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
namespace sylar{

class TcpServer: public std::enable_shared_from_this<TcpServer>, Noncopyable{
public:
    using ptr = std::shared_ptr<TcpServer>;
    TcpServer(sylar::IOManager* worker = sylar::IOManager::GetThis(),
              sylar::IOManager* accept_worker = sylar::IOManager::GetThis());
    virtual ~TcpServer();
    
    //包括bind + listen
    virtual bool bind(sylar::Address::ptr addr, bool ssl = false);
    virtual bool bind(const std::vector<sylar::Address::ptr>& addrs, 
    std::vector<sylar::Address::ptr>& fails, bool ssl = false);
    //开始accept建立连接
    virtual bool start();
    virtual void stop();

    uint64_t getRecvTimeout() const {return  m_recvTimeout;}
    std::string getName() const{return m_name;}

    void setRecvTimeout(uint64_t v) { m_recvTimeout = v;}
    void setName(const std::string& v){m_name = v;}

    bool isStop() const{return m_isStop;}

protected:
    //
    virtual void handleClient(Socket::ptr client);
    //成功建立连接 实际的数据处理
    virtual void startAccept(Socket::ptr sock);
private:
    std::vector<Socket::ptr> m_socks;   //支持不止一种协议 保存的是listen成功的队列
    IOManager* m_worker;                //处理新的客户端连接
    IOManager* m_acceptWorker;          //专门负责监听和接受新的连接请求
    uint64_t m_recvTimeout;
    std::string m_name;
    bool m_isStop;
};

}


#endif