#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include <string>
#include <unordered_map>

class response_builder {
public:
    static std::string build(int status_code, const std::string& body, 
                            const std::string& content_type = "application/json");
    static std::string get_status_message(int status_code);
    
private:
    static std::unordered_map<int, std::string> status_messages;
};

inline std::unordered_map<int, std::string> response_builder::status_messages = {
    {200, "OK"},
    {201, "Created"},
    {400, "Bad Request"},
    {401, "Unauthorized"},
    {403, "Forbidden"},
    {404, "Not Found"},
    {500, "Internal Server Error"},
    {503, "Service Unavailable"}
};

inline std::string response_builder::get_status_message(int status_code) {
    auto it = status_messages.find(status_code);
    if (it != status_messages.end()) {
        return it->second;
    }
    return "Unknown";
}

inline std::string response_builder::build(int status_code, const std::string& body, const std::string& content_type) {
    std::string response;
    response += "HTTP/1.1 ";
    response += std::to_string(status_code);
    response += " ";
    response += get_status_message(status_code);
    response += "\r\n";
    response += "Content-Type: ";
    response += content_type;
    response += "\r\n";
    response += "Content-Length: ";
    response += std::to_string(body.length());
    response += "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;
    return response;
}

#endif
