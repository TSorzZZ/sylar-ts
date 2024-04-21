#ifndef __SYLAR_IOMANAGER_H__
#define __SYLAR_IOMANAGER_H__

#include "fiber.h"
#include "mythread.h"
#include "scheduler.h"
#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <vector>

namespace sylar{

class IOManager : public Scheduler{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event{
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x4
    };

private:
    struct FdContext{
        typedef Mutex MutexType;
        int fd = 0;                                     //事件关联句柄
        struct EventContext{
            Scheduler* scheduler = nullptr;         //事件执行的scheduler
            Fiber::ptr fiber;                       //事件协程
            std::function<void()> cb;               //事件回调
        };
        EventContext& getContext(Event event);
        void resetContext(EventContext& ctx);
        void triggerEvent(Event event);

        EventContext read;                          //读事件
        EventContext write;                         //写事件
        Event m_events = NONE;                      //已注册事件
        MutexType mutex;
    };

public:
    IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    // 0 success 1 error
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);
    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd);
    static IOManager* GetThis();    //获取当前IOManager

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void contextResize(size_t size);
private:
    int m_epfd = 0;
    int m_tickleFds[2]; //
    std::atomic<size_t> m_pendingEventCount = {0};
    RWMutexType m_mutex;
    std::vector<FdContext*> m_fdContexts;
};



}

#endif