#include "util.h"
#include "log.h"
#include <bits/types/struct_timeval.h>
#include <cstddef>
#include <cstdlib>
#include<execinfo.h>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <sys/time.h>
#include "fiber.h"

namespace sylar {
    
sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint32_t GetFiberId() {
    return sylar::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string>& bt, int size, int skip){
    void** array = (void**)malloc((sizeof(void*) * size));
    size_t s = ::backtrace(array, size);

    char** strings = backtrace_symbols(array, s);
    if(strings == NULL){
        SYLAR_LOG_ERROR(g_logger) << "backtrace_symbols error";
        return;
    }

    for(size_t i = skip; i < s; ++i){
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& prefix){
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for(size_t i = 0; i < bt.size(); ++i){
        ss <<   prefix << bt[i]<< std::endl;
    }
    return ss.str();
}

uint64_t GetCurrentMS(){
    timeval tv;
    gettimeofday(&tv, NULL);
    SYLAR_LOG_DEBUG(g_logger) << tv.tv_sec << "  " << tv.tv_usec;
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t GetCurrentUS(){
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}


}