#include "../sylar/sylar.h"
#include <memory>
#include <string>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void run_in_fiber(){
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber begin";
    sylar::Fiber::YieldToHold();
    SYLAR_LOG_INFO(g_logger) << "run_in_fiber end";
    sylar::Fiber::YieldToHold();
}

void test_fiber(){
    SYLAR_LOG_INFO(g_logger) << "main begin -1";
    {
        sylar::Fiber::GetThis();
        SYLAR_LOG_INFO(g_logger) << "main begin";
        sylar::Fiber::ptr fiber = std::make_shared<sylar::Fiber>(run_in_fiber);
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "main after swapin";
        fiber->swapIn();
        SYLAR_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    SYLAR_LOG_INFO(g_logger) << "main after end2";
}


int main(){
    sylar::Thread::SetName("mainThread");
    std::vector<sylar::Thread::ptr> threadPool;
    for(int i = 0; i < 3; i++){
        threadPool.push_back(sylar::Thread::ptr(new sylar::Thread(&test_fiber, "name_" + std::to_string(i))));
    }

    for(auto i: threadPool){
        i->join();
    }

    return 0;
}
