#ifndef __SYLAR__FIBER_H__
#define __SYLAR__FIBER_H__

#include <cstdint>
#include <sys/ucontext.h>
#include <ucontext.h>
#include<memory>
#include<functional>
#include "mythread.h"

namespace sylar{

class Scheduler;
class Fiber: public std::enable_shared_from_this<Fiber> {
friend class Scheduler;
public:
    using  ptr = std::shared_ptr<Fiber>;

    //协程状态
    enum State{
        INIT,   //初始化
        HOLD,   //挂起
        READY,  //就绪
        EXEC,   //执行
        TERM,   //终止
        EXCEPT  //异常
    };

private:
    Fiber();
public:
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();

    void reset(std::function<void()> cb);   //重置协程函数  重置状态     INIT， TERM
    void swapIn();  //切换到当前协程
    void swapOut(); //切换到后台执行    让出
    void call();
    void back();

    uint64_t getId()const {return m_id;}
    State getState()const {return m_state;}
    void setState(State state) { m_state = state;}
public:
    static Fiber::ptr GetThis();    //返回当前协程
    static uint64_t GetFiberId();
    static void SetThis(Fiber* f);
    static void YieldToReady(); //切换到后台 设置为ready
    static void YieldToHold();  //切换到后台，设置为hold
    static uint64_t TotalFibers();  //总协程数
    static void MainFunc();
    static void CallerMainFunc();
private:
    uint64_t m_id = 0;                  //协程id
    uint32_t m_stacksize = 0;           //栈深
    State m_state = INIT;               //协程状态
    ucontext_t m_ctx;                   //上下文
    void* m_stack = nullptr;            //栈指针
    std::function<void()> m_cb;         //回调
};







}

#endif