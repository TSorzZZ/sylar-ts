#include "address.h"
#include "iomanager.h"
#include "log.h"
#include"tcp_server.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run() {
    auto addr = sylar::Address::LookupAny("0.0.0.0:8033");
    //auto addr2 = sylar::UnixAddress::ptr(new sylar::UnixAddress("/tmp/unix_addr"));
    std::vector<sylar::Address::ptr> addrs, fails;
    addrs.push_back(addr);
    //addrs.push_back(addr2);

    sylar::TcpServer::ptr tcp_server(new sylar::TcpServer);
    while(!tcp_server->bind(addrs, fails)) {
        sleep(2);
    }
    tcp_server->start();
    
}

int main(int argc, char** argv){
    sylar::IOManager iom(2);
    iom.schedule(run);

    return 0;

}