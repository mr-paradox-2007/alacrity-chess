#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string>
#include <functional>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <fstream>
#include <sstream>

#include "request_parser.hpp"
#include "response_builder.hpp"

typedef std::function<std::string(const http_request&)> route_handler;

class http_server {
private:
    int port;
    bool running;
    int server_socket;
    std::unordered_map<std::string, route_handler> routes;
    std::mutex routes_mutex;
    std::string base_path;
    
public:
    http_server(int port_num, const std::string& client_path = "client");
    ~http_server();
    
    void register_route(const std::string& method, const std::string& path, route_handler handler);
    void handle_client(int client_socket);
    void start();
    void stop();
    bool is_running() const;
    
private:
    std::string read_file(const std::string& filepath);
    std::string get_content_type(const std::string& filepath);
    std::string serve_static(const std::string& path);
};

inline http_server::http_server(int port_num, const std::string& client_path) 
    : port(port_num), running(false), server_socket(-1), base_path(client_path) {}

inline http_server::~http_server() {
    stop();
}

inline void http_server::register_route(const std::string& method, const std::string& path, route_handler handler) {
    std::lock_guard<std::mutex> lock(routes_mutex);
    std::string key = method + " " + path;
    routes[key] = handler;
}

inline std::string http_server::read_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

inline std::string http_server::get_content_type(const std::string& filepath) {
    if (filepath.find(".html") != std::string::npos) return "text/html";
    if (filepath.find(".css") != std::string::npos) return "text/css";
    if (filepath.find(".js") != std::string::npos) return "application/javascript";
    if (filepath.find(".json") != std::string::npos) return "application/json";
    if (filepath.find(".png") != std::string::npos) return "image/png";
    if (filepath.find(".jpg") != std::string::npos) return "image/jpeg";
    if (filepath.find(".gif") != std::string::npos) return "image/gif";
    return "text/plain";
}

inline std::string http_server::serve_static(const std::string& path) {
    std::string request_path = path;
    if (request_path == "/") {
        request_path = "/index.html";
    }
    
    std::string filepath = base_path + request_path;
    std::string content = read_file(filepath);
    
    if (content.empty()) {
        return "";
    }
    
    return content;
}

inline void http_server::handle_client(int client_socket) {
    char buffer[8192] = {0};
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        http_request req = request_parser::parse(std::string(buffer));
        
        std::lock_guard<std::mutex> lock(routes_mutex);
        std::string route_key = req.method + " " + req.path;
        
        std::string response_body;
        int status_code = 404;
        std::string content_type = "application/json";
        
        if (routes.find(route_key) != routes.end()) {
            response_body = routes[route_key](req);
            status_code = 200;
        } else if (req.method == "GET") {
            response_body = serve_static(req.path);
            if (!response_body.empty()) {
                status_code = 200;
                content_type = get_content_type(req.path);
            } else {
                response_body = "{\"error\":\"Route not found\"}";
                status_code = 404;
            }
        } else {
            response_body = "{\"error\":\"Route not found\"}";
            status_code = 404;
        }
        
        std::string response = response_builder::build(status_code, response_body, content_type);
        send(client_socket, response.c_str(), response.length(), 0);
    }
    
    close(client_socket);
}

inline void http_server::start() {
    if (running) return;
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) return;
    
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(server_socket);
        return;
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(server_socket);
        return;
    }
    
    listen(server_socket, 10);
    running = true;
    
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket >= 0) {
            std::thread(&http_server::handle_client, this, client_socket).detach();
        }
    }
}

inline void http_server::stop() {
    running = false;
    if (server_socket >= 0) {
        close(server_socket);
        server_socket = -1;
    }
}

inline bool http_server::is_running() const {
    return running;
}

#endif
