#include "mythread.h"
#include"log.h"
#include "util.h"
#include <cerrno>
#include <pthread.h>
#include <semaphore.h>
#include <stdexcept>
#include <threads.h>
#include <functional>

namespace sylar{

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system"); 

Semaphore::Semaphore(uint32_t count){
    if(sem_init(&m_semaphore, 0, count)){
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore(){
    sem_destroy(&m_semaphore);
}

void Semaphore::wait(){
    if(sem_wait(&m_semaphore)){
        throw std::logic_error("sem_wait error");
    } 
}
void Semaphore::notify(){
    if(sem_post(&m_semaphore)){
        throw std::logic_error("sem_notify error");
    } 
}





//static方法

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

void* Thread::run(void *arg){
    Thread* thread = static_cast<Thread*>(arg);
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = sylar::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str());
    std::function<void()> cb;
    cb.swap(thread->m_cb);
    thread->m_semaphore.notify();
    cb();
    return 0;
}

//成员方法

Thread::Thread(std::function<void()> cb, const std::string& name):m_cb(cb) ,m_name(name){
    if(name.empty()){
        m_name = "UNKNOW";
    }
    //启动线程
    int ret = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if(ret){
        SYLAR_LOG_ERROR(g_logger) << "pthread_create thread fail, ret=" << ret << " name=" << m_name;
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();
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