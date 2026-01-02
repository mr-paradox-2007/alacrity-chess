#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

struct http_request {
    std::string method;
    std::string path;
    std::string http_version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

class request_parser {
public:
    static http_request parse(const std::string& raw_request);
    
private:
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

inline std::vector<std::string> request_parser::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

inline http_request request_parser::parse(const std::string& raw_request) {
    http_request req;
    std::stringstream ss(raw_request);
    std::string line;
    
    if (std::getline(ss, line)) {
        auto parts = split(line, ' ');
        if (parts.size() >= 3) {
            req.method = parts[0];
            req.path = parts[1];
            req.http_version = parts[2];
        }
    }
    
    bool in_body = false;
    std::string body_part;
    while (std::getline(ss, line)) {
        if (in_body) {
            if (!req.body.empty()) req.body += "\n";
            req.body += line;
            continue;
        }
        
        if (line.empty() || line == "\r") {
            in_body = true;
            continue;
        }
        
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 2);
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }
            req.headers[key] = value;
        }
    }
    
    // Remove trailing \r if present
    if (!req.body.empty() && req.body.back() == '\r') {
        req.body.pop_back();
    }
    
    return req;
}

#endif
