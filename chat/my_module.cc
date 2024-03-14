#include "my_module.h"
#include "sylar/config.h"
#include "sylar/log.h"
#include "sylar/application.h"

#include "resource_servlet.h"
#include "chat_servlet.h"
#include "sylar/env.h"
#include "sylar/http/ws_server.h"

namespace chat {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

MyModule::MyModule()
    : sylar::Module("chat_room", "1.0", "") {
}

bool MyModule::onLoad() {
    SYLAR_LOG_INFO(g_logger) << "onLoad";
    return true;
}

bool MyModule::onUnload() {
    SYLAR_LOG_INFO(g_logger) << "onUnload";
    return true;
}

/*
static int32_t handle(sylar::http::HttpRequest::ptr request
            ,sylar::http::HttpResponse::ptr response
            ,sylar::http::HttpSession::ptr session) {
    SYLAR_LOG_INFO(g_logger) << "handle";
    response->setBody("handle");
    return 0;
}
*/

bool MyModule::onServerReady() {
    SYLAR_LOG_INFO(g_logger) << "onServerReady";

    std::vector<sylar::TcpServer::ptr> svrs;
    if (!sylar::Application::GetInstance()->getServer("http", svrs)) {
        SYLAR_LOG_INFO(g_logger) << "no httpserver alived";
        return false;
    }

    for (auto& svr : svrs) {
        sylar::http::HttpServer::ptr http_server = 
                std::dynamic_pointer_cast<sylar::http::HttpServer>(svr);
        if (!http_server) {
            continue;
        }
        // 为 http server 的 /html/* 路径添加对应的处理 dispatch
        auto slt_dispatch = http_server->getServletDispatch();
        sylar::http::ResourceServlet::ptr slt(new sylar::http::ResourceServlet(
                sylar::EnvMgr::GetInstance()->getCwd()
        ));
        slt_dispatch->addGlobServlet("/html/*", slt);
        SYLAR_LOG_INFO(g_logger) << "addServlet";
    }

    svrs.clear();
    if (!sylar::Application::GetInstance()->getServer("ws", svrs)) {
        SYLAR_LOG_INFO(g_logger) << "no ws server alived";
        return false;
    }
    for (auto& svr : svrs) {
        sylar::http::WSServer::ptr ws_server = 
                std::dynamic_pointer_cast<sylar::http::WSServer>(svr);
        if (!ws_server) {
            continue;
        }
        // 为 ws server 添加 WSServerDispatch
        sylar::http::ServletDispatch::ptr slt_dispatch = ws_server->getWSServerDispatch();
        ChatServlet::ptr slt(new ChatServlet);
        slt_dispatch->addServlet("/sylar/chat", slt);
    }

    return true;
}

bool MyModule::onServerUp() {
    SYLAR_LOG_INFO(g_logger) << "onServerUp";
    return true;
}

extern "C" {

sylar::Module* CreateModule() {
    sylar::Module* module = new chat::MyModule;
    SYLAR_LOG_INFO(chat::g_logger) << "CreateModule " << module;
    return module;
}

void DestroyModule(sylar::Module* module) {
    SYLAR_LOG_INFO(chat::g_logger) << "DestroyModule " << module;
    delete module;
}

}

}