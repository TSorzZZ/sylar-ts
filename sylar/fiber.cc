#include"fiber.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include<functional>
#include<atomic>
#include <memory>
#include <string>
#include <ucontext.h>
#include"config.h"
#include "../sylar/macro.h"
#include "log.h"
#include "../sylar/scheduler.h"
#include "util.h"

namespace sylar{

static Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static std::atomic<uint64_t> s_fiber_id {0};        //协程id
static std::atomic<uint64_t> s_fiber_count {0};     //协程数

static thread_local Fiber* t_fiber = nullptr;           //当前线程正在运行的协程
static thread_local Fiber::ptr t_threadFiber = nullptr;    //线程主协程

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>("fiber.stack_size", 128 * 1024, "fiber stack size");


class MallocStackAllocator{
public:
    static void* Alloc(size_t size){
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size){
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId(){
    if(t_fiber){
        return t_fiber->getId();
    }
    return 0;
}

Fiber::Fiber(){
    m_state = EXEC;
    SetThis(this);
    if(getcontext(&m_ctx)){
        SYLAR_ASSERT2("false", "getcontext");
    }

    ++s_fiber_count;
    SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber() id=" << m_id;
}

//构造新协程
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller):m_id(++s_fiber_id), m_cb(cb){ 
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();

    m_stack = StackAllocator::Alloc(m_stacksize);
    if(getcontext(&m_ctx)){
        SYLAR_ASSERT2("false", "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    if(!use_caller){
        makecontext(&m_ctx, &Fiber::MainFunc, 0);
    }else{
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }
    
    SYLAR_LOG_DEBUG(g_logger) << "Fiber::Fiber(cb) id=" << m_id;
}


Fiber::~Fiber(){
    --s_fiber_count;
    if(m_stack){    //协程栈不为空则释放
        SYLAR_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);
        StackAllocator::Dealloc(m_stack, m_stacksize);
    }else{  
        SYLAR_ASSERT(!m_cb);
        SYLAR_ASSERT(m_state == EXEC);
        Fiber* cur = t_fiber;
        if(cur == this){
            SetThis(nullptr);
        }
    }
    
    SYLAR_LOG_DEBUG(g_logger) << "Fiber::~Fiber id=" << m_id;
}

//设置当前协程
void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

//复用协程函数  重置状态     INIT， TERM
void Fiber::reset(std::function<void()> cb){
    SYLAR_ASSERT(m_stack);
    SYLAR_ASSERT(m_state == TERM || m_state == INIT || m_state == EXCEPT);;
    m_cb = cb;
    if(getcontext(&m_ctx)){ 
        SYLAR_ASSERT2(false, "getcontext");
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;

}

void Fiber::call(){
    SetThis(this);
    m_state = EXEC;
    if(swapcontext(&t_threadFiber->m_ctx, &m_ctx)){
        SYLAR_ASSERT2(false, "swapcontext");
    }
}

void Fiber::back(){
    SetThis( t_threadFiber.get());
        if(swapcontext(&m_ctx, &t_threadFiber->m_ctx)){
        SYLAR_ASSERT2(false, "swapcontext");
        }
}

//切换到当前协程    主协程 -> 子协程
void Fiber::swapIn(){
    SetThis(this);
    SYLAR_ASSERT(m_state != EXEC);
    m_state = EXEC;
    if(swapcontext( &Scheduler::GetMainFiber()->m_ctx, &m_ctx) ){
        SYLAR_ASSERT2(false, "swapcontext");
    }

}  


//切换到后台执行   子协程 -> 主协程 
void Fiber::swapOut(){
    if(this != Scheduler::GetMainFiber()){  //
        SetThis( Scheduler::GetMainFiber());
        if(swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx)){
        SYLAR_ASSERT2(false, "swapcontext");
        }
    }
}

//返回当前正在执行的协程      如果线程未存在协程 则创建主协程    如果存在  直接返回  
Fiber::ptr Fiber::GetThis(){
    if(t_fiber){
        return t_fiber->shared_from_this();
    }
    Fiber::ptr mainFiber(new Fiber);
    SYLAR_ASSERT(t_fiber == mainFiber.get());
    t_threadFiber = mainFiber;
    return t_fiber->shared_from_this();
}    

//切换到后台 设置为ready
void Fiber::YieldToReady(){
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur->m_state == EXEC);
    cur->m_state = READY;
    cur->swapOut();
}

//切换到后台，设置为hold
void Fiber::YieldToHold(){
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur->m_state == EXEC);
    cur->m_state  = HOLD;
    cur->swapOut();
}

//总协程数
uint64_t Fiber::TotalFibers(){
    return s_fiber_count;
} 

// 协程主函数 执行完之后返回term状态
void Fiber::MainFunc(){
    Fiber::ptr cur = GetThis(); //引用技术会+1 导致无法析构
    SYLAR_ASSERT(cur);
    try{
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }catch(std::exception& ex){
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except:" << ex.what()
            << "\n" << sylar::BacktraceToString();
    }catch(...){
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except:"
        << "\n" << sylar::BacktraceToString();
    }
    auto raw_ptr = cur.get();
    cur.reset();    //释放智能指针
    raw_ptr->swapOut(); //切换回主协程

        SYLAR_ASSERT2(false, "never  reach fiber_id = " + std::to_string(raw_ptr->getId()));

}

void Fiber::CallerMainFunc(){
    Fiber::ptr cur = GetThis(); //引用技术会+1 导致无法析构
    SYLAR_ASSERT(cur);
    try{
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    }catch(std::exception& ex){
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except:" << ex.what()
            << "\n" << sylar::BacktraceToString();
    }catch(...){
        cur->m_state = EXCEPT;
        SYLAR_LOG_ERROR(g_logger) << "Fiber Except:"
        << "\n" << sylar::BacktraceToString();
    }
    auto raw_ptr = cur.get();
    cur.reset();    //释放智能指针
    raw_ptr->back(); //

    SYLAR_ASSERT2(false, "never  reach fiber_id = " + std::to_string(raw_ptr->getId()));
}


}

