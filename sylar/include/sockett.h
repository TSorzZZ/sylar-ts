#ifndef _SYLAR_SOCKETT_H__
#define _SYLAR_SOCKETT_H__

#include "address.h"
#include"noncopyable.h"
#include <bits/types/struct_iovec.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <sys/socket.h>

namespace sylar {

class Socket: public std::enable_shared_from_this<Socket>, Noncopyable{
public:
    using ptr = std::shared_ptr<Socket>;
    using weak_ptr = std::weak_ptr<Socket>;

    enum Type{
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };

    enum Family{
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX,
    };

    //根据tcp/udp family类型创建
    static Socket::ptr CreateTCP(sylar::Address::ptr address);
    static Socket::ptr CreateUDP(sylar::Address::ptr address);
    
    //创建ipv4
    static Socket::ptr CreateTCPSocket();
    static Socket::ptr CreateUDPSocket();
    //创建ipv6
    static Socket::ptr CreateTCPSocket6();
    static Socket::ptr CreateUDPSocket6();
    //
    static Socket::ptr CreateUnixTCPSocket();
    static Socket::ptr CreateUnixUDPSocket();
    


    Socket(int family, int type, int protocol = 0);
    ~Socket();

    //获取/设置收发超时时间
    int64_t getSendTimeout();
    void setSendTimeout(int64_t v);
    int64_t getRecvTimeout();
    void setRecvTimeout(int64_t v);

    //对应getsockopt
    bool getOption(int level, int option, void* result, socklen_t* len);
    template<class T>
    bool getOption(int level, int option, T& result) {
        socklen_t length = sizeof(T);
        return getOption(level, option, &result, &length);
    }
    //对应setsockopt
    bool setOption(int level, int option, const void* result, socklen_t len);
    template<class T>
    bool setOption(int level, int option, const T& value) {
        return setOption(level, option, &value, sizeof(T));
    }
    //新建一个socket 绑定本地地址
    bool bind(const Address::ptr addr);
    //监听socket 手动设定接受连接的socket的最大连接数。
    bool listen(int backlog = SOMAXCONN);
    //接受连接 返回accept后的新socket
    Socket::ptr accept();
    //连接指定的地址 设置超时时间
    bool connect(const Address::ptr addr, uint64_t timeout_ms = -1);
    //关闭socket连接
    bool close();

    //发送1-多个数据块
    int send(const void* buffer, size_t length, int flags = 0);
    int send(const iovec* buffers, size_t length, int flags = 0);
    //向指定的地址发送数据
    int sendTo(const void* buffer, size_t length, Address::ptr to, int flags = 0);
    int sendTo(const iovec* buffers, size_t length, Address::ptr to, int flags = 0);
    //接收1-多个数据块
    int recv(void* buffer, size_t length, int flags = 0);
    int recv(iovec* buffers, size_t length, int flags = 0);
    //从指定的地址接收数据
    int recvFrom(void* buffer, size_t length, Address::ptr from, int flags = 0);
    int recvFrom(iovec* buffers, size_t length, Address::ptr from, int flags = 0);

    //创建/获取 远端或本地地址
    Address::ptr getRemoteAddress();
    Address::ptr getLocalAddress();

    int getFamily() const{ return m_family;}
    int getType() const{ return m_type;}
    int getProtocol() const{ return m_protocol;}
    bool isConnected() const{ return m_isConnected;}
    bool isValid() const;
    int getError();
    //打印socket信息到输出流中
    std::ostream& dump(std::ostream& os) const;
    int getSocket() const{ return m_sock;}

    bool cancelRead();
    bool cancelWrite();
    bool cancelAccept();
    bool cancelAll();

private:
    //创建新的socket 并初始化
    void newSock();
    //初始化socket 套接字关闭后立即释放该端口  开启Nagle算法
    void initSock();
    //初始化当前socket fd的状态  用于复用socket
    bool init(int sock);
private:
    int m_sock;     //socket fd
    int m_family;   //协议族
    int m_type;     //通信类型
    int m_protocol; //协议
    bool m_isConnected; //是否连接

    Address::ptr m_localAddress;    //本地地址
    Address::ptr m_remoteAddress;   //远端地址

};

std::ostream& operator<<(std::ostream& os, const Socket& sock);

}


#endif