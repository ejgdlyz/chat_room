# 项目创建

创建：
```shell
git clone https://github.com/ejgdlyz/chat_room.git
cd chat_room
git submodule update --init --recursive
```
编译运行：
``` shell
make
make -j
sh move.sh

bin/chat_room -s
```

# 主要功能

## HTTP Server
为 HttpServer 添加文件 Servlet 以支持文件的传输。即，添加一个 ResourceServlet，继承 Servlet，并重写其 handle() 方法。

## WebSocket Server
基于 websocket 实现消息的发送与接收，服务器可以主动向客户端发送消息。即，为 WebSocket Server 添加一个聊天的 Servlet（ChatServlet），其继承 WSServlet，并重写其回调函数 (onConnect()/onClose()/handle())。

## 协议设计
聊天的消息是 JSON 格式。

具体的协议格式为：
1. login 协议。
    
    Client 请求格式: `{type: “login_request”, ”name”: “昵称”}`
    
    Server 响应格式: `{type: “login_response”, result: 200, msg: “ok”}`
    
2. send 协议
    
    Client: `{type: “send_request”, data: {”msg” : “消息内容”}}`
    
    Server: `{type: “send_response”, result: 200, msg : “ok”}`
    
3. user_enter 通知
    
    有人加入或者离开了聊天室，服务器进行通知，客户端不需要回应。
    
    `{type: “user_enter”, msg: “xxx 加入聊天室”}`
    
4. user_leave 通知
    
    `{type: “user_leave”, msg: “xxx 离开聊天室”}`
    
5. msg 通知
    
    有人聊天时，其他人也有收到这个消息。
    
    `{type: “msg”, msg: “具体的聊天信息”, user: “xxx”, time: xxx}`
    
    将这个消息通知给客户端，客户端拿着 msg, user 和 time 去做展示。