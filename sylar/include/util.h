#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <cstdint>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>

namespace sylar {

pid_t GetThreadId();
uint32_t GetFiberId();

void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);
std::string BacktraceToString(int size = 64, int skip = 2, const std::string& prefix = "");

std::string Time2Str(time_t ts = time(0), const std::string& format = "%Y-%m-%d %H:%M:%S");
time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");

uint64_t GetCurrentMS();
uint64_t GetCurrentUS();

class FSUtil{
public:
    static void ListAllFile(std::vector<std::string>& files, 
                            const std::string& path,
                            const std::string& subfix);
};

}

#endif