#ifndef __SYLAR_TIMER_H__
#define __SYLAR_TIMER_H__

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <sys/types.h>
#include <type_traits>
#include <vector>
#include "mythread.h"

namespace sylar{

class TimerManager;
class Timer: public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;
    bool cancel();
    bool refresh();
    bool reset(uint64_t ms, bool from_now);
private:
    Timer(uint64_t ms, std::function<void()> cb, 
         bool recurring, TimerManager* manager);
    Timer(uint64_t next);
    
private:
    bool m_recurring = false;       //是否循环定时器
    uint64_t m_ms = 0;              //相对时间
    uint64_t m_next = 0;            //精确执行时间  绝对时间
    std::function<void()> m_cb;
    TimerManager* m_manager = nullptr;
private:
    struct Comparator{
        bool operator () (const Timer::ptr & lhs, const Timer::ptr & rhs) const;
    };

};

class TimerManager{
friend class Timer;
public:
    typedef RWMutex RWMutexType;

    TimerManager();
    virtual ~TimerManager();
    Timer::ptr addTimer(uint64_t ms, std::function<void()>, bool recurring = false);
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, 
                                std::weak_ptr<void> weak_cond, bool recurring = false);
    uint64_t getNextTimer();
    void listExpiredCb(std::vector<std::function<void()>>& cbs);
    bool hasTimer();    //
protected:
    virtual void onTimerInsertedAtFront() = 0;  //timer插入最前时发生什么
    void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);    //添加timer
    
private:
    bool detectClockRollover(uint64_t now_ms);  //检测系统时间变化
private:
    RWMutexType m_mutex;
    std::set<Timer::ptr, Timer::Comparator> m_timers;   //有序
    bool m_tickled = false;
    uint64_t m_previousTime = 0;
};


}






#endif