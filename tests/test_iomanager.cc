#include "iomanager.h"
#include "log.h"
#include "sylar.h"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
int sockfd = 0;
void test_fiber(){
    SYLAR_LOG_INFO(g_logger) << "test_fiber";
}

void test1(){
    sylar::IOManager iom;
    iom.schedule(&test_fiber);


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "120.232.145.185", &addr.sin_addr.s_addr);

    
    if(!connect(sockfd, (sockaddr *)&addr, sizeof(addr))){

    }else if(errno == EINPROGRESS){
        SYLAR_LOG_INFO(g_logger) << "add evemt errno= " << errno << " " << strerror(errno);
            iom.addEvent(sockfd, sylar::IOManager::READ, [](){
            SYLAR_LOG_INFO(g_logger) << "read callbak";
        }); 
        iom.addEvent(sockfd, sylar::IOManager::WRITE, [](){
            SYLAR_LOG_INFO(g_logger) << "write callback";
            sylar::IOManager::GetThis()->cancelEvent(sockfd, sylar::IOManager::READ);
            close(sockfd);
        });
    }else{
        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
    
    

}


sylar::Timer::ptr s_timer;
void test_timer() {
    sylar::IOManager iom(2);
    s_timer = iom.addTimer(1000, [](){
        static int i = 0;
        SYLAR_LOG_INFO(g_logger) << "hello timer i=" << i;
        if(++i == 3) {
            s_timer->reset(2000, true);
            //s_timer->cancel();
        }
    }, true);
}


int main(int argc, char** argv){
    //test1();
    test_timer();
    return 0;
}