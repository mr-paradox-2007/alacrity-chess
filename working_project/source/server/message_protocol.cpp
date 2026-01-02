#include "message_protocol.hpp"
#include <sstream>

std::string Message::ToJSON() const {
    std::stringstream ss;
    ss << "{\"type\":" << (int)type << ",\"data\":{";
    
    bool first = true;
    for (const auto& [key, value] : data) {
        if (!first) ss << ",";
        ss << "\"" << key << "\":\"" << value << "\"";
        first = false;
    }
    
    ss << "}}";
    return ss.str();
}

Message Message::FromJSON(const std::string& json) {
    Message msg;
    msg.type = MSG_ERROR;
    
    size_t type_pos = json.find("\"type\":");
    if (type_pos != std::string::npos) {
        size_t type_end = json.find(",", type_pos);
        std::string type_str = json.substr(type_pos + 8, type_end - type_pos - 8);
        msg.type = (MessageType)std::stoi(type_str);
    }
    
    size_t data_start = json.find("\"data\":{") + 8;
    size_t data_end = json.rfind("}");
    if (data_start < data_end) {
        std::string data_str = json.substr(data_start, data_end - data_start);
        
        size_t pos = 0;
        while (pos < data_str.length()) {
            size_t key_start = data_str.find("\"", pos);
            if (key_start == std::string::npos) break;
            size_t key_end = data_str.find("\"", key_start + 1);
            if (key_end == std::string::npos) break;
            
            std::string key = data_str.substr(key_start + 1, key_end - key_start - 1);
            
            size_t value_start = data_str.find("\"", key_end + 1);
            if (value_start == std::string::npos) break;
            size_t value_end = data_str.find("\"", value_start + 1);
            if (value_end == std::string::npos) break;
            
            std::string value = data_str.substr(value_start + 1, value_end - value_start - 1);
            
            msg.data[key] = value;
            pos = value_end + 1;
        }
    }
    
    return msg;
}
