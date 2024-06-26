#include "scheduler.h"
#include "fiber.h"
#include "hook.h"
#include "iomanager.h"
#include "log.h"
#include"macro.h"
#include "mythread.h"
#include "util.h"
#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace sylar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static thread_local Scheduler* t_scheduler = nullptr;   //协程调度器指针
static thread_local Fiber* t_scheduler_fiber = nullptr;           //当前调度器协程

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name):m_name(name){
    SYLAR_ASSERT(threads > 0);

    if(use_caller)  //当前线程主协程直接进行调度
    {
        sylar::Fiber::GetThis();    //初始化当前线程主协程
        --threads;  //当前线程主协程直接参与调度

        SYLAR_ASSERT(GetThis() == nullptr);
        t_scheduler = this;
        //caller线程中的调度协程，协程绑定run方法 新建一个fiber绑定run方法  并且加入任务队列用于调度
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        sylar::Thread::SetName(m_name);
        t_scheduler_fiber = m_rootFiber.get();
        m_rootThread = sylar::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    }
    else
    {
        m_rootThread = -1;
    }
    m_threadCount = threads;
}

Scheduler::~Scheduler(){
    SYLAR_ASSERT(m_stopping);
    if(GetThis() == this){
        t_scheduler = nullptr;
    }
}

//获得调度器
Scheduler* Scheduler::GetThis(){
    return t_scheduler;
}

//获取当前调度协程
Fiber* Scheduler::GetMainFiber(){
    return t_scheduler_fiber;
}

//开启调度
void Scheduler::start(){
    MutexType::Lock lock(m_mutex);
    if(!m_stopping){
        return;
    }
    m_stopping = false;
    SYLAR_ASSERT(m_threads.empty());

    m_threads.resize(m_threadCount);    //重置线程池大小
    //启动多个线程 开始调度
    for(size_t i = 0; i < m_threadCount; ++i){
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this), 
        m_name +  "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    lock.unlock();
}


void Scheduler::stop(){
    m_autoStop = true;

    if(m_rootFiber && m_threadCount == 0 && 
      (m_rootFiber->getState() == Fiber::TERM ||
       m_rootFiber->getState() == Fiber::INIT) )
    {
        SYLAR_LOG_INFO(g_logger) << this << " stopped";
        m_stopping = true;
        if(stopping())return;
    }

    if(m_rootThread != -1){ //use caller线程   调度器就是当前自己
        SYLAR_ASSERT(GetThis() == this);
    }else{
        SYLAR_ASSERT( GetThis() != this);
    }

    m_stopping = true;
    for(size_t i = 0; i < m_threadCount; ++i){  //唤醒每个线程
        tickle();
    }

    if(m_rootFiber) {
        tickle();
    }

    if(m_rootFiber) {
        
        if(!stopping()) {
            m_rootFiber->call();
        }
    }
    std::vector<Thread::ptr> threadPool;
    {
        MutexType::Lock lock(m_mutex);
        threadPool.swap(m_threads);
    }

    //等待所有线程执行完
    for(auto& i: threadPool){
        i->join();
    }

    //if(stopping())return;

}

void Scheduler::setThis(){
    t_scheduler = this;
}

void Scheduler::run(){  //真正的调度方法
    //1.  初始化协程  指定调度器
    SYLAR_LOG_INFO(g_logger) << "run!!!";
    if(typeid(*this) == typeid(IOManager)){
        set_hook_enable(true);  //只使用scheduler时不能开启hook 不然会找不到iomanager
    }
    
    setThis();

    //2.如果不是caller线程 指定当前调度协程为当前协程
    if(sylar::GetThreadId() != m_rootThread){
        t_scheduler_fiber = Fiber::GetThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;

    FiberAndThread ft;
    //3. 找到一个可执行的任务
    while(true){
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            // 1、遍历任务队列
            while(it != m_fibers.end()){
                // 2、该任务指定了线程id 且不对应
                if( it->thread != -1 && it->thread != sylar::GetThreadId())
                {
                    ++it;
                    tickle_me = true;   //通知别人处理
                    continue;
                }
                SYLAR_ASSERT(it->fiber || it->cb);
                // 3、是协程 且在执行状态  不处理
                if(it->fiber && it->fiber->getState() == Fiber::EXEC){
                    ++it;
                    continue;
                }

                ft = *it;
                m_fibers.erase(it++);
                ++m_activeThreadCount;  //添加任务时数量就++   启动协程时再++会有问题
                is_active = true;
                break;
            }
            tickle_me |= it != m_fibers.end();
        }

        // 4、是否需要唤醒其他线程处理
        if(tickle_me){
            tickle();
        }

        // 5、处理协程
        // 当前协程不在结束状态  将协程唤醒并执行
        if(ft.fiber && (ft.fiber->getState() != Fiber::TERM &&
            ft.fiber->getState() != Fiber::EXCEPT) ){ //唤醒
            
            ft.fiber->swapIn();     //执行协程
            --m_activeThreadCount;
            // 在准备状态  可以继续调度
            if(ft.fiber->getState() == Fiber::READY){
                schedule(ft.fiber);
            }
            // 并非 结束或异常状态  设为HOLD状态
            else if(ft.fiber->getState() != Fiber::TERM &&
                     ft.fiber->getState() != Fiber::EXCEPT){
                ft.fiber->m_state = (Fiber::HOLD);
            }
            ft.reset();
        }

        //6、处理函数
        else if(ft.cb){
            //存在cb_fiber  直接复用协程
            if(cb_fiber){
                cb_fiber->reset(ft.cb);
            }
            // 不存在直接智能指针新建一个Fiber
            else{
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.reset();

            // 执行协程
            cb_fiber->swapIn();  
            --m_activeThreadCount;
            if(cb_fiber->getState() == Fiber::READY){
                schedule(cb_fiber);
                cb_fiber.reset();
            }else if(cb_fiber->getState() == Fiber::EXCEPT || cb_fiber->getState() == Fiber::TERM){
                cb_fiber->reset(nullptr);
            }else{
                cb_fiber->m_state = (Fiber::HOLD);
                cb_fiber.reset();
            }
        }
        else    //没有任务执行就执行idle协程
        {
            //SYLAR_LOG_INFO(g_logger) << "idle run!!!";
            if(is_active){
                --m_activeThreadCount;
                continue;
            }

            if(idle_fiber->getState() == Fiber::TERM){
                SYLAR_LOG_INFO(g_logger) << "idle fiber term";
                break;
            }

            ++m_idleThreadCount;
            idle_fiber->swapIn();   //执行idle协程
            --m_idleThreadCount;
            if(idle_fiber->getState() != Fiber::TERM && idle_fiber->getState() != Fiber::EXCEPT){
                idle_fiber->m_state = (Fiber::HOLD);
            }
        }

    }
}

void Scheduler::tickle(){
    SYLAR_LOG_INFO(g_logger) << "tickle";
}

//没有正在运行的线程 且任务列表为空
bool Scheduler::stopping(){
    MutexType::Lock lock(m_mutex);
    return m_autoStop && m_stopping && 
           m_fibers.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle(){
    SYLAR_LOG_INFO(g_logger) << "idle";
    while(!stopping()){
        sylar::Fiber::YieldToHold();
    }
}

}



