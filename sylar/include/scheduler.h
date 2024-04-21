#ifndef __SYLAR__SCHEDULER_H__
#define __SYLAR__SCHEDULER_H__

#include <atomic>
#include <cstddef>
#include <functional>
#include <list>
#include<memory>
#include <vector>
#include"fiber.h"
#include"mythread.h"


namespace sylar {


class Scheduler{
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler();

    const std::string& getName()const {return m_name;}

    static Scheduler* GetThis();
    static Fiber* GetMainFiber();

    void start();
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if(need_tickle)tickle();
    }

    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end){
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while(begin != end){
                need_tickle = scheduleNoLock( &*begin) || need_tickle;
                ++begin;
            }
        }
        if(need_tickle)tickle();
    }
protected:
    virtual void tickle();  //唤醒线程
    virtual bool stopping();
    virtual void idle();    //什么都不干的时候做什么？
    void run();
    void setThis();
private:
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread){  //无锁调度    只有任务队列为空的时候才需要唤醒线程
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if(ft.fiber || ft.cb){
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }

private:
    struct FiberAndThread{
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        FiberAndThread(Fiber::ptr f, int thr):fiber(f), thread(thr){}
        FiberAndThread(Fiber::ptr* f, int thr): thread(thr){
            fiber.swap(*f);
        }

        FiberAndThread(std::function<void()> f, int thr): cb(f),thread(thr){}

        FiberAndThread(std::function<void()>* f, int thr): thread(thr){
            cb.swap(*f);
        }

        FiberAndThread():thread(-1){}

        void reset(){
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

private:
    MutexType m_mutex;
    std::vector<sylar::Thread::ptr> m_threads;  //线程池
    std::list<FiberAndThread> m_fibers;         //任务
    std::string m_name;
    Fiber::ptr m_rootFiber;
protected:
    std::vector<int> m_threadIds;       //线程id
    size_t m_threadCount = 0;               //线程数
    std::atomic_size_t m_activeThreadCount {0};         //激活线程数
    std::atomic_size_t m_idleThreadCount {0};           //等待线程数
    bool m_stopping = true;                    //是否停止
    bool m_autoStop = false;                    //自动停止
    int m_rootThread = 0;                   //主线程

};






}

#endif