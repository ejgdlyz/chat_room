#include "chat_servlet.h"
#include "sylar/log.h"
#include "protocol.h"

namespace chat {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

sylar::RWMutex m_mutex;
// name -> session(连接)
std::map<std::string, sylar::http::WSSession::ptr> m_sessions;

bool SessionExists(const std::string& id) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_sessions.find(id);
    return it != m_sessions.end();
}

void SessionAdd(const std::string& id, sylar::http::WSSession::ptr session) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_sessions[id] = session;
}

void SessionDel(const std::string& id) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_sessions.erase(id);
}

bool SendMessage(sylar::http::WSSession::ptr session, ChatMessage::ptr chat_rsp) {
    SYLAR_LOG_INFO(g_logger) << chat_rsp->toString() << " - " << session;
    return session->sendMessage(chat_rsp->toString()) > 0 ? 0 : 1;
}

void SessionNotify(ChatMessage::ptr msg, sylar::http::WSSession::ptr session = nullptr) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto sessions = m_sessions;
    lock.unlock();

    for(auto& s: sessions) {
        if (s.second == session) {
            // 跳过自己
            continue;
        }
        SendMessage(s.second, msg);
    }
}

ChatServlet::ChatServlet()
    :sylar::http::WSServlet("chat_servlet") {
}

int32_t ChatServlet::onConnect(sylar::http::HttpRequest::ptr header  // 连接事件
        , sylar::http::WSSession::ptr session) {
    
    SYLAR_LOG_INFO(g_logger) << "onConnect " << session;
    return 0;
}

int32_t ChatServlet::onClose(sylar::http::HttpRequest::ptr header    // 关闭事件
        , sylar::http::WSSession::ptr session) {

    auto id = header->getHeader("$id");
    SYLAR_LOG_INFO(g_logger) << "onClose " << session << " id = " << id;
    if (!id.empty()) {
        SessionDel(id);
        // notify
        ChatMessage::ptr notify(new ChatMessage);
        notify->set("type", "user_leave");
        notify->set("time", sylar::Time2Str());
        notify->set("name", id);
        SessionNotify(notify, nullptr);
    }
    return 0;
}

int32_t ChatServlet::handle(sylar::http::HttpRequest::ptr header     // 处理事件
        , sylar::http::WSFrameMessage::ptr ws_msg
        , sylar::http::WSSession::ptr session) {
    
    SYLAR_LOG_INFO(g_logger) << "handle " << session
             << " opcode = " << ws_msg->getOpcode()
             << " data = " <<  ws_msg->getData();
    
    auto chat_msg = ChatMessage::Create(ws_msg->getData());
    auto id = header->getHeader("$id");
    if (!chat_msg) {
        if (!id.empty()) {
            sylar::RWMutex::WriteLock lock(m_mutex);
            m_sessions.erase(id);
        }
        return 1;  // 非 0 就是有问题的
    }

    ChatMessage::ptr chat_rsp(new ChatMessage);
    auto type = chat_msg->get("type");
    if (type == "login_request") {  // 登录 (客户端主动请求)
        chat_rsp->set("type", "login_response");
        auto name = chat_msg->get("name");
        if (name.empty()) {
            chat_rsp->set("result", "400");
            chat_rsp->set("msg", "name is null");
            return SendMessage(session, chat_rsp);
        }
        if (!id.empty()) {
            // id 判断是否已经登录
            chat_rsp->set("result", "401");
            chat_rsp->set("msg", "logined");
            return SendMessage(session, chat_rsp);
        }
        if (SessionExists(id)) {
            chat_rsp->set("result", "402");
            chat_rsp->set("msg", "name existed");
            return SendMessage(session, chat_rsp);
        }
        id = name;
        header->setHeader("$id", id);
        chat_rsp->set("result", "200");
        chat_rsp->set("msg", "ok");
        SessionAdd(id, session);
        
        // notify
        ChatMessage::ptr notify(new ChatMessage);
        notify->set("type", "user_enter");
        notify->set("time", sylar::Time2Str());
        notify->set("name", name);
        SessionNotify(notify, session);

        return SendMessage(session, chat_rsp);

    } else if (type == "send_request") {        // 发消息 (客户端主动请求)
        chat_rsp->set("type", "send_response");
        auto m = chat_msg->get("msg");
        if (m.empty()) {
            chat_rsp->set("result", "500");
            chat_rsp->set("msg", "msg is null");
            return SendMessage(session, chat_rsp);
        }
        if (id.empty()) {
            chat_rsp->set("type", "501");
            chat_rsp->set("type", "not login");
            return SendMessage(session, chat_rsp);
        }
        chat_rsp->set("result", "200");
        chat_rsp->set("msg", "ok");
        
        // notify
        ChatMessage::ptr notify(new ChatMessage);
        notify->set("type", "msg");
        notify->set("time", sylar::Time2Str());
        notify->set("name", id);
        notify->set("msg", m);
        SessionNotify(notify, nullptr);

        return SendMessage(session, chat_rsp);
    } else if (type == "user_leave") {      // 被动请求

    } else if (type == "msg") {             // 被动请求

    } else {

    }
    return 0;
}

}