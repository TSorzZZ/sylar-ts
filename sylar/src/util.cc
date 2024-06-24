#include "util.h"
#include "log.h"
#include <bits/types/struct_timeval.h>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include<execinfo.h>
#include <ostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/time.h>
#include<sys/types.h>
#include<dirent.h>
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
    //SYLAR_LOG_DEBUG(g_logger) << tv.tv_sec << "  " << tv.tv_usec;
    return tv.tv_sec * 1000ul + tv.tv_usec / 1000;
}

uint64_t GetCurrentUS(){
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 * 1000ul + tv.tv_usec;
}


std::string Time2Str(time_t ts, const std::string& format) {
    struct tm tm;
    localtime_r(&ts, &tm);
    char buf[64];
    strftime(buf, sizeof(buf), format.c_str(), &tm);
    return buf;
}

time_t Str2Time(const char* str, const char* format) {
    struct tm t;
    memset(&t, 0, sizeof(t));
    if(!strptime(str, format, &t)) {
        return 0;
    }
    return mktime(&t);
}

void FSUtil::ListAllFile(std::vector<std::string>& files, 
                            const std::string& path,
                            const std::string& subfix)
{
    //检查调用进程是否有权限访问指定的文件或目录
    if(access(path.c_str(), 0) != 0){
        return;
    }
    //用于打开一个目录，以便后续的目录读取操作。
    DIR* dir = opendir(path.c_str());
    if(dir == nullptr){
        return;
    }

    struct dirent* dp = nullptr;
    while( (dp = readdir(dir)) != nullptr){
        //递归子文件夹
        if(dp->d_type == DT_DIR){
            if(!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") ){
                continue;
            }
            ListAllFile(files, path + "/" + dp->d_name, subfix);
        }else if(dp->d_type == DT_REG){
            std::string filename(dp->d_name);
            if(subfix.empty()){
                files.push_back(path + "/" + filename);
            }else{
                if(filename.size() < subfix.size()){
                    continue;
                }
                if(filename.substr(filename.length() - subfix.size()) == subfix){
                    files.push_back(path + "/" + filename);
                }
            }
        }
    }
    closedir(dir);

}




}