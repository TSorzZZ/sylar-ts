#include "sylar.h"
#include <unistd.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_fiber(){
    static int s_count = 5;
    SYLAR_LOG_INFO(g_logger) << "test in fiber scout = " << s_count;
    sleep(1);
    if(--s_count >= 0){
        sylar::Scheduler::GetThis()->schedule(test_fiber, sylar::GetThreadId());
    }
}

int main(int argc, char** argv){
    SYLAR_LOG_INFO(g_logger) << "main begin";
    sylar::Scheduler sc(3, true, "testt");
    sc.start();
    sc.schedule(test_fiber);
    sc.stop();
    SYLAR_LOG_INFO(g_logger) << "main end";
}