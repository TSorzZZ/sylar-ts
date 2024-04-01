#include "../sylar/sylar.h"
#include <memory>
#include <string>
#include <vector>

sylar::Logger::ptr g_logger =  SYLAR_LOG_ROOT();

void fun1(){
    SYLAR_LOG_INFO(g_logger) << "name: " << sylar::Thread::GetName()
                             << " this.name: " << sylar::Thread::GetThis()->getName()
                             << " id: " << sylar::GetThreadId()
                             << " this.id: " << sylar::Thread::GetThis()->getId();
}

void fun2(){

}

using threadPtr = sylar::Thread::ptr;

int main(){
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    std::vector<threadPtr> threadPool;
    for(int i = 0; i < 5; i++){
        threadPtr thr(std::make_shared<sylar::Thread>(fun1, "name_" + std::to_string(i)));
        threadPool.emplace_back(thr);
    }

    for(int i = 0; i < 5; i++) threadPool[i]->join();

    SYLAR_LOG_INFO(g_logger) << "thread test end";
    return 0;
}