#ifndef __SYLAR_DAEMON_H__
#define __SYLAR_DAEMON_H__

#include <cstdint>
#include <functional>
#include"singleton.h"
#include <sched.h>
#include <string>
namespace sylar{


struct ProcessInfo{
    pid_t parent_id = 0;
    pid_t main_id = 0;
    uint64_t parent_start_time = 0;
    uint64_t main_start_time = 0;
    uint32_t restart_count = 0;

    std::string toString() const;
};

using ProcessInfoMgr = Singleton<ProcessInfo>;

int start_daemon(int argc, char** argv, 
std::function<int(int argc, char** argv)> main_cb, bool is_daemon);



}

#endif