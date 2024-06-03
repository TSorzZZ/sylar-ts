#include "iomanager.h"
#include "fiber.h"
#include "log.h"
#include "macro.h"
#include "scheduler.h"
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <sys/epoll.h>
#include <unistd.h>
#include<fcntl.h>
#include<string>

namespace sylar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

//获取当前Fd对应的事件(读/写事件)的内容
IOManager::FdContext::EventContext& IOManager::FdContext::getContext(IOManager::Event event){
    switch(event){
        case IOManager::READ:
            return read;
        case IOManager::WRITE:
            return write;
        default:
            SYLAR_ASSERT2(false, "getContext");
    }
}

//重置fdContext  复用
void IOManager::FdContext::resetContext(EventContext& ctx){
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}


//触发对应读/写事件
void IOManager::FdContext::triggerEvent(Event event){
    SYLAR_ASSERT(event & events);
    events = (Event)(events & ~event);  //已注册事件 要把当前激活事件去掉
    EventContext& ctx = getContext(event);
    if(ctx.cb){
        ctx.scheduler->schedule(&ctx.cb);
    }else{
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
    return;
}

//创建对应的epoll 以及pipe读写管道
IOManager::IOManager(size_t threads, bool use_caller, const std::string& name)
:Scheduler(threads, use_caller, name){
    m_epfd = epoll_create(5000);
    SYLAR_ASSERT(m_epfd > 0);

    int ret = pipe(m_tickleFds);
    SYLAR_ASSERT(!ret);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    ret = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);//设置pipe为非阻塞
    SYLAR_ASSERT(!ret);

    //将管道的读端加入epoll表
    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    SYLAR_ASSERT(!ret);
    contextResize(32);

    //开始调度
    start();
}

//优雅退出 关闭所有fd   回收fd队列的资源
IOManager::~IOManager(){
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for(size_t i = 0; i < m_fdContexts.size(); ++i){
        if(m_fdContexts[i]){
            delete m_fdContexts[i];
        }
    }
}

//Fd队列扩容
void IOManager::contextResize(size_t size){
    m_fdContexts.resize(size);

    for(size_t i = 0; i < m_fdContexts.size(); i++){
        if(!m_fdContexts[i]){
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}


//添加事件 -1 error 0 success
int IOManager::addEvent(int fd, Event event, std::function<void()> cb){
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    //当前fd 存在  取出fd上下文
    if((int)m_fdContexts.size() > fd){
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    }
    else    //当前fd不存在 扩容并操作新的fd
    {
        lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    //当前fd的事件已经存在了
    if(fd_ctx->events & event){
        SYLAR_LOG_ERROR(g_logger) << "addEvent assert fd=" << fd 
        << "event= " << fd_ctx->events;
        SYLAR_ASSERT(!(fd_ctx->events & event));
    }

    
    int op = fd_ctx->events ? EPOLL_CTL_MOD: EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET| fd_ctx->events | event;
    epevent.data.ptr = fd_ctx;

    //增删epoll中的fd
    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if(ret){
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << "," << fd << "," << epevent.events << "):"
            << ret << "(" << errno << ") (" << strerror(errno)  <<")";
            return -1;
    }

    ++m_pendingEventCount;
    //配置fd事件的调度器以及回调
    fd_ctx->events = (Event)(fd_ctx->events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    SYLAR_ASSERT(!event_ctx.scheduler && !event_ctx.fiber && !event_ctx.cb) ;  //确定有事件

    event_ctx.scheduler = Scheduler::GetThis();
    if(cb){
        event_ctx.cb.swap(cb);
    }else{
        event_ctx.fiber = Fiber::GetThis();
        SYLAR_ASSERT(event_ctx.fiber->getState() == Fiber::EXEC);
    }
    return 0;
        
}

// 删除事件  不执行回调
bool IOManager::delEvent(int fd, Event event){
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();
    
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);

    //存在事件
    if(!(fd_ctx->events & event)){
        return false;
    }

    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events? EPOLL_CTL_MOD: EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    //修改epoll表
    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if(ret){
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << "," << fd << "," << epevent.events << "):"
            << ret << "(" << errno << ") (" << strerror(errno)  <<")";
            return false;
    }

    --m_pendingEventCount;
    fd_ctx->events = new_events;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}

//取消事件 如果该事件被注册过回调，那就触发一次回调事件
bool IOManager::cancelEvent(int fd, Event event){
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();
    
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);

    if(!(fd_ctx->events & event)){
        return false;
    }

    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events? EPOLL_CTL_MOD: EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if(ret){
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << "," << fd << "," << epevent.events << "):"
            << ret << "(" << errno << ") (" << strerror(errno)  <<")";
            return false;
    }

    
    fd_ctx->triggerEvent(event);
    --m_pendingEventCount;
    return true;
}

// 取消所有事件 所有被注册的回调事件在cancel之前都会被执行一次
bool IOManager::cancelAll(int fd){
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();
    
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);

    if(!(fd_ctx->events)){
        return false;
    }

    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if(ret){
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << "," << fd << "," << epevent.events << "):"
            << ret << "(" << errno << ") (" << strerror(errno)  <<")";
            return false;
    }

    if(fd_ctx->events & READ){
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }
    
    if(fd_ctx->events & WRITE){
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
    }

    SYLAR_ASSERT(fd_ctx->events == 0);
    return true;
}

IOManager* IOManager::GetThis(){    
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}

//当io调度器空闲时，idle协程通过epoll_wait阻塞在管道的读描述符上，等管道的可读事件。
//添加新任务时，tickle方法写管道，idle协程检测到管道可读后退出，调度器执行调度。
void IOManager::tickle(){
    if(!hasIdleThreads()){
        return;
    }
    int ret = write(m_tickleFds[1], "T", 1);
    SYLAR_ASSERT(ret == 1);
}

// 获取超时时间  增加额外条件  定时任务都执行完成   且没有等待事件
bool IOManager::stopping(uint64_t& timeout){
    timeout = getNextTimer();
    return timeout == ~0ull && m_pendingEventCount == 0 && Scheduler::stopping();
}

bool IOManager::stopping(){
    uint64_t timeout = 0;
    return stopping(timeout);
}

void IOManager::idle(){
    epoll_event* events = new epoll_event[64]();
    //重载删除器
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
        delete [] ptr;
    });

    while(true){
        uint64_t next_timeout = 0;
        if(SYLAR_UNLIKELY(stopping(next_timeout))){
            SYLAR_LOG_INFO(g_logger) << "name=" << getName() << " idle stopping exit";
            break;
        }

        //epoll 有事件发生
        int ret = 0;
        do {
            static const int MAX_TIMEOUT = 3000;
            if(next_timeout != ~0ull) {
                next_timeout = (int)next_timeout > MAX_TIMEOUT
                                ? MAX_TIMEOUT : next_timeout;
            } else {
                next_timeout = MAX_TIMEOUT;
            }
            ret = epoll_wait(m_epfd, events, 64, (int)next_timeout);
            if(ret < 0 && errno == EINTR) {
            } else {
                break;
            }
        } while(true);

        // 收集所有已超时的定时器，执行回调函数
        std::vector<std::function<void()>> cbs;
        listExpiredCb(cbs);
        if(!cbs.empty()){
            SYLAR_LOG_DEBUG(g_logger) << "on timer size=" << cbs.size();
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        for(int i = 0; i < ret; i++){
            epoll_event&  event = events[i];
            if(event.data.fd == m_tickleFds[0]){
                uint8_t dummy;
                while(read(m_tickleFds[0], &dummy, 1) == 1); //et触发 所以要while
                continue;
            }

            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            FdContext::MutexType::Lock lock(fd_ctx->mutex);
            //确定epoll当前监听道德事件
            if(event.events & (EPOLLERR | EPOLLHUP)){
                event.events |= EPOLLIN | EPOLLOUT;
            }
            int real_events = NONE;
            if(event.events & EPOLLIN){
                real_events |= READ;
            }
            if(event.events & EPOLLOUT){
                real_events |= WRITE;
            }

            if((fd_ctx->events & real_events) == NONE)continue;

            //修改epoll表中对应fd
            int left_events = (fd_ctx->events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;
            int ret2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if(ret2){
                SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
                << op << "," << fd_ctx->fd << "," << event.events << "):"
                << ret2 << "(" << errno << ") (" << strerror(errno)  <<")";
                continue;
            }

            //触发当前事件
            if(real_events & READ){
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if(real_events & WRITE){
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }

        //从idle协程返回调度协程
        Fiber::ptr cur = Fiber::GetThis();
        auto raw_ptr = cur.get();
        cur.reset();
        raw_ptr->swapOut();
    }

}

void IOManager::onTimerInsertedAtFront(){
    tickle();
}

}






