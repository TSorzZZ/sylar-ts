#include "iomanager.h"
#include "fiber.h"
#include "log.h"
#include "macro.h"
#include "scheduler.h"
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include<fcntl.h>
#include<string>

namespace sylar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

IOManager::FdContext::EventContext& IOManager::FdContext::getContext(IOManager::Event event){
    switch(event){
        case IOManager::READ:
            return read;
        case IOManager::WRITE:
            return read;
        default:
            SYLAR_ASSERT2(false, "getContext");
    }
}

void IOManager::FdContext::resetContext(EventContext& ctx){
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(Event event){
    SYLAR_ASSERT(event & m_events);
    m_events = (Event)(m_events & ~event);
    EventContext& ctx = getContext(event);
    if(ctx.cb){
        ctx.scheduler->schedule(&ctx.cb);
    }else{
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
    return;
}

IOManager::IOManager(size_t threads, bool use_caller, const std::string& name)
:Scheduler(threads, use_caller, name){
    m_epfd = epoll_create(5000);
    SYLAR_ASSERT(m_epfd > 0);

    int ret = pipe(m_tickleFds);
    SYLAR_ASSERT(ret);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    ret = fcntl(m_tickleFds[0],F_SETFL, O_NONBLOCK);
    SYLAR_ASSERT(ret);

    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);

    contextResize(32);

    start();
}

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

void IOManager::contextResize(size_t size){
    m_fdContexts.resize(size);

    for(size_t i = 0; i < m_fdContexts.size(); i++){
        if(!m_fdContexts[i]){
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}


//-1 error 0 retry 1 success
int IOManager::addEvent(int fd, Event event, std::function<void()> cb){
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    //取出fd上下文
    if(m_fdContexts.size() > fd){
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    }else{
        lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        contextResize(m_fdContexts.size() * 1.5);
        fd_ctx = m_fdContexts[fd];
    }

    FdContext::Mutex::Lock lock(fd_ctx->mutex);
    //
    if(fd_ctx->m_events & event){
        SYLAR_LOG_ERROR(g_logger) << "addEvent assert fd=" << fd 
        << "event= " << fd_ctx->m_events;
        SYLAR_ASSERT(!(fd_ctx->m_events & event));
    }

    int op = fd_ctx->m_events ? EPOLL_CTL_MOD: EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET| fd_ctx->m_events | event;
    epevent.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if(ret){
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
            << op << "," << fd << "," << epevent.events << "):"
            << ret << "(" << errno << ") (" << strerror(errno)  <<")";
            return -1;
    }

    ++m_pendingEventCount;
    fd_ctx->m_events = (Event)(fd_ctx->m_events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    SYLAR_ASSERT(!event_ctx.scheduler && !event_ctx.fiber && !event_ctx.cb) ;  //确定有事件

    event_ctx.scheduler = Scheduler::GetThis();
    if(cb){
        context.cb.swap(cb);
    }else{
        context.fiber = Fiber::GetThis();
        SYLAR_ASSERT(context.fiber->getState() == Fiber::EXEC);
    }
    return 0;
        
}
bool IOManager::delEvent(int fd, Event event){
    RWMutexType::ReadLock lock(m_mutex);
    if(m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();
    
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);

    if(!(fd_ctx->m_events & event)){
        return false;
    }

    Event new_events = (Event)(fd_ctx->m_events & ~event);
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

    --m_pendingEventCount;
    fd_ctx->m_events = new_events;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}

bool IOManager::cancelEvent(int fd, Event event){
    RWMutexType::ReadLock lock(m_mutex);
    if(m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();
    
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);

    if(!(fd_ctx->m_events & event)){
        return false;
    }

    Event new_events = (Event)(fd_ctx->m_events & ~event);
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

bool IOManager::cancelAll(int fd){
    RWMutexType::ReadLock lock(m_mutex);
    if(m_fdContexts.size() <= fd){
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();
    
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);

    if(!(fd_ctx->m_events)){
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

    if(fd_ctx->m_events & READ){
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }
    
    if(fd_ctx->m_events & WRITE){
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
    }

    SYLAR_ASSERT(fd_ctx->m_events == 0);
    return true;
}
IOManager* IOManager::GetThis(){    
    return dynamic_cast<IOManager*>(Scheduler::GetThis());
}

void IOManager::tickle(){
    
}
bool IOManager::stopping(){
    
}
void IOManager::idle(){

}


}