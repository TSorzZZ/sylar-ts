#include "address.h"
#include "bytearray.h"
#include "sockett.h"
#include"tcp_server.h"
#include"log.h"
#include"iomanager.h"
#include <bits/types/struct_iovec.h>
#include <iostream>
#include <vector>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

class EchoServer: public sylar::TcpServer{
public:
    EchoServer(int type);   //二进制或文本
    void handleClient(sylar::Socket::ptr client);
private:
    int m_type = 0;
};

EchoServer::EchoServer(int type):m_type(type){
}


void EchoServer::handleClient(sylar::Socket::ptr client){
    SYLAR_LOG_INFO(g_logger) << "handleClient " << *client; 

    sylar::ByteArray::ptr ba(new sylar::ByteArray);
    while(true){
        ba->clear();
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, 1024);    // 1K

        int rt = client->recv(&iovs[0], iovs.size());
        if(rt == 0) {
            SYLAR_LOG_INFO(g_logger) << "client close: " << *client;
            break;
        } else if(rt < 0) {
            SYLAR_LOG_INFO(g_logger) << "client error rt=" << rt
                << " errno=" << errno << " errstr=" << strerror(errno);
            break;
        }
        ba->setPosition(ba->getPosition() + rt);

        ba->setPosition(0);
        if(m_type == 1){
            std::cout << ba->toString();
        }else{
            std::cout << ba->toHexString();
        }
        std::cout.flush();
    }
    

}

int type = 1;

void run(){
    SYLAR_LOG_INFO(g_logger) << "server type=" << type;
    EchoServer::ptr es(new EchoServer(type));
    auto addr = sylar::Address::LookupAny("0.0.0.0:8020");
    while(!es->bind(addr)){
        sleep(2);
    }

    es->start();
}

//argc  字符数组长度   argv字符数组
int main(int argc, char** argv){
    if(argc < 2) {
        SYLAR_LOG_INFO(g_logger) << "used as[" << argv[0] << " -t] or [" << argv[0] << " -b]";
        return 0;
    }

    //二进制 -b    字符 -t   默认-t
    if(!strcmp(argv[1], "-b")) {
        type = 2;
    }

    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}