#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include <memory>
#include <pthread.h>
#include <functional>
#include <sched.h>
#include <string>

namespace sylar{
    
class Thread{
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string& name);
    ~Thread();

    pid_t getId()const {return m_id;}
    const std::string& getName() const {return m_name;}
    void join();

    static Thread* GetThis();
    static const std::string& GetName();
    static void SetName(const std::string& name);
    static void* run(void* arg);
private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread& operator = (const Thread&) = delete;
private:
    pid_t m_id;
    pthread_t m_thread;
    std::function<void()> m_cb;
    std::string m_name;

};


}










#endif