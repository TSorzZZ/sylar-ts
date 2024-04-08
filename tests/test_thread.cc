#include "../sylar/sylar.h"
#include <memory>
#include <string>
#include <vector>

sylar::Logger::ptr g_logger =  SYLAR_LOG_ROOT();
sylar::RWMutex s_mutex;

int count = 0;

void fun1(){
    SYLAR_LOG_INFO(g_logger) << "name: " << sylar::Thread::GetName()
                             << " this.name: " << sylar::Thread::GetThis()->getName()
                             << " id: " << sylar::GetThreadId()
                             << " this.id: " << sylar::Thread::GetThis()->getId();

    for(int i = 0; i < 100000; i++){
        sylar::RWMutex::WriteLock lock(s_mutex);
        ++count;
    }

}

void fun2(){
    while(true){
        SYLAR_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxx";
    }
}

void fun3(){
    while(true){
        SYLAR_LOG_INFO(g_logger) << "===================";
    }
}


using threadPtr = sylar::Thread::ptr;

int main(){
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    YAML::Node root = YAML::LoadFile("/home/ts/project/sylar-ts/bin/conf/log2.yml");
    sylar::Config::LoadFromYaml(root);

    std::vector<threadPtr> threadPool;
    for(int i = 0; i < 2; i++){
        threadPtr thr(std::make_shared<sylar::Thread>(fun2, "name_" + std::to_string(i*2)));
        threadPtr thr2(std::make_shared<sylar::Thread>(fun3, "name_" + std::to_string(i*2+1)));
        threadPool.emplace_back(thr);
        threadPool.emplace_back(thr2);
    }

    for(int i = 0; i < threadPool.size(); i++) threadPool[i]->join();

    SYLAR_LOG_INFO(g_logger) << "thread test end";
    SYLAR_LOG_INFO(g_logger) << "count= " << count;

    sylar::Config::Visit([](sylar::ConfigVarBase::ptr var){
        SYLAR_LOG_INFO(g_logger) << "name= " << var->getName()
                                    << "description= " << var->getDescription()
                                    << "typename= " << var->getTypeName()
                                    << "value= " << var->toString();
    });

    return 0;
}