#include "protocol.h"
#include "sylar/util.h"

namespace chat {

ChatMessage::ptr ChatMessage::Create(const std::string& v) {
    Json::Value json;
    if (!sylar::JsonUtil::FromString(json, v)) {  // string -> json
        return nullptr;
    }
    ChatMessage::ptr rt(new ChatMessage);
    auto keys = json.getMemberNames();
    for (auto& key : keys) {
        rt->m_data[key] = json[key].asString();
    }
    return rt;
}   

ChatMessage::ChatMessage() {
}

std::string ChatMessage::get(const std::string& name) {
    auto it = m_data.find(name);
    return it == m_data.end() ? "" : it->second;
}

void ChatMessage::set(const std::string& name, const std::string& val) {
    m_data[name] = val;
}

std::string ChatMessage::toString() const {
    Json::Value json;
    // map -> json
    for(auto& pair : m_data) {
        json[pair.first] = pair.second;
    }
    return sylar::JsonUtil::ToString(json);
}
}