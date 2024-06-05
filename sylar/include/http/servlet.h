#ifndef __SYLAR_HTTP_SERVLET_H__
#define __SYLAR_HTTP_SERVLET_H__

#include "http.h"
#include "http_session.h"
#include "mythread.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include"mythread.h"
namespace sylar {

namespace http {

class Servlet{
public:
    using ptr = std::shared_ptr<Servlet>;
    Servlet(const std::string& name):m_name(name){}
    virtual ~Servlet() {}
    virtual int32_t handle(sylar::http::HttpRequest::ptr request,
                   sylar::http::HttpResponse::ptr response,
                   sylar::http::HttpSession::ptr session) = 0;
    const std::string& getName() const { return m_name;}
private:
    std::string m_name;
};

class FunctionServlet: public Servlet{
public:
    using ptr = std::shared_ptr<FunctionServlet>;
    using callback = std::function<int32_t (sylar::http::HttpRequest::ptr,
                   sylar::http::HttpResponse::ptr,
                   sylar::http::HttpSession::ptr)>;
    FunctionServlet(callback cb);
    virtual int32_t handle(sylar::http::HttpRequest::ptr request,
                   sylar::http::HttpResponse::ptr response,
                   sylar::http::HttpSession::ptr session) override;
private:
    callback m_cb;
};


class ServletDispatch: public Servlet{
public:
    using ptr = std::shared_ptr<ServletDispatch>;
    using RWMutex_t = RWMutex;

    ServletDispatch();

    int32_t handle(sylar::http::HttpRequest::ptr request,
                   sylar::http::HttpResponse::ptr response,
                   sylar::http::HttpSession::ptr session) override;

    void addServlet(const std::string& uri, Servlet::ptr slt);
    void addServlet(const std::string& uri, FunctionServlet::callback cb);
    void addGlobServlet(const std::string& uri, Servlet::ptr slt);
    void addGlobServlet(const std::string& uri, FunctionServlet::callback cb);

    Servlet::ptr getServlet(const std::string& uri);
    Servlet::ptr getGlobServlet(const std::string& uri);
    Servlet::ptr getMatchedServlet(const std::string& uri);
    Servlet::ptr getDefault() const {return m_default;};
    void setDefault(Servlet::ptr v){ m_default = v;}


    void delServlet(const std::string& uri);
    void delGlobServlet(const std::string& uri);

private:
    RWMutex_t m_mutex;
    //uri(/sylar/xxx) -> servlet 精准匹配
    std::unordered_map<std::string, Servlet::ptr> m_datas;
    //uri(/sylar/*) -> servlet 模糊匹配
    std::vector<std::pair<std::string, Servlet::ptr>> m_globs;
    //默认servlet， 没匹配到uri使用
    Servlet::ptr m_default;
};


class NotFoundServlet: public Servlet{
public:
    using ptr = std::shared_ptr<NotFoundServlet>;
    NotFoundServlet(const std::string& name);
    virtual int32_t handle(sylar::http::HttpRequest::ptr request,
                   sylar::http::HttpResponse::ptr response,
                   sylar::http::HttpSession::ptr session) override;
private:
    std::string m_name;
    std::string m_content;            
};


}

}


#endif