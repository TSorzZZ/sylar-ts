#include "mythread.h"
#include"log.h"
#include "util.h"
#include <pthread.h>
#include <stdexcept>
#include <threads.h>
#include <functional>

namespace sylar{

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system"); 

Thread* Thread::GetThis(){
    return t_thread;
}

const std::string& Thread::GetName(){
    return t_thread_name;
}

void Thread::SetName(const std::string& name){
    if(t_thread){
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name):m_cb(cb) ,m_name(name){
    if(name.empty()){
        m_name = "UNKNOW";
    }
    int ret = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(ret){
        SYLAR_LOG_ERROR(g_logger) << "pthread_create thread fail, ret=" << ret << " name=" << m_name;
        throw std::logic_error("pthread_create error");
    }
}

void* Thread::run(void *arg){
    Thread* thread = static_cast<Thread*>(arg);
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = sylar::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str());
    std::function<void()> cb;
    cb.swap(thread->m_cb);
    cb();
    return 0;
}

Thread::~Thread(){
    if(m_thread){
        pthread_detach(m_thread);
    }
}


void Thread::join(){
    if(m_thread){
        int ret = pthread_join(m_thread, nullptr);
        if(ret){
        SYLAR_LOG_ERROR(g_logger) << "pthread_join thread fail, ret=" << ret << " name=" << m_name;
        throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}



}