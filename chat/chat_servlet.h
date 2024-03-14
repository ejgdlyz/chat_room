#ifndef __CHAT_CHAT_SERVLET_H__
#define __CHAT_CHAT_SERVLET_H__

#include "sylar/http/ws_servlet.h"

namespace chat {

class ChatServlet : public sylar::http::WSServlet {
public:
    typedef std::shared_ptr<ChatServlet> ptr;
    
    ChatServlet();
    virtual int32_t onConnect(sylar::http::HttpRequest::ptr header  // 连接事件
            , sylar::http::WSSession::ptr session) override;
    virtual int32_t onClose(sylar::http::HttpRequest::ptr header    // 关闭事件
            , sylar::http::WSSession::ptr session) override;
    virtual int32_t handle(sylar::http::HttpRequest::ptr header     // 处理事件
            , sylar::http::WSFrameMessage::ptr msg
            , sylar::http::WSSession::ptr session) override;
};
}

#endif