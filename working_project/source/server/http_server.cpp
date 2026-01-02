#include "http_server.hpp"
#include <iostream>
#include <sstream>

HTTPServer::HTTPServer(int port) : server_socket(-1), port(port), running(false) {}

HTTPServer::~HTTPServer() {
    Stop();
}

bool HTTPServer::Start() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "HTTP Socket creation failed\n";
        return false;
    }
    
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Setsockopt failed\n";
        return false;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "HTTP Bind failed\n";
        return false;
    }
    
    if (listen(server_socket, 5) < 0) {
        std::cerr << "HTTP Listen failed\n";
        return false;
    }
    
    running = true;
    std::cout << "HTTP Server started on port " << port << "\n";
    return true;
}

void HTTPServer::Stop() {
    running = false;
    if (server_socket >= 0) {
        close(server_socket);
        server_socket = -1;
    }
}

void HTTPServer::RegisterRoute(const std::string& path, RequestHandler handler) {
    routes[path] = handler;
}

void HTTPServer::HandleConnections() {
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            if (running) std::cerr << "HTTP Accept failed\n";
            continue;
        }
        
        std::thread(&HTTPServer::HandleClient, this, client_socket).detach();
    }
}

void HTTPServer::HandleClient(int client_socket) {
    char buffer[4096] = {0};
    int recv_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (recv_size > 0) {
        buffer[recv_size] = '\0';
        std::string request(buffer);
        
        std::string path;
        size_t path_start = request.find(" ");
        size_t path_end = request.find(" ", path_start + 1);
        if (path_start != std::string::npos && path_end != std::string::npos) {
            path = request.substr(path_start + 1, path_end - path_start - 1);
        }
        
        std::string response;
        if (routes.count(path)) {
            size_t body_start = request.find("\r\n\r\n");
            std::string body = (body_start != std::string::npos) ? 
                              request.substr(body_start + 4) : "";
            response = BuildResponse(routes[path](body));
        } else if (path == "/") {
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
            response += "<html><body>Chess Server HTTP API</body></html>";
        } else {
            response = BuildResponse("{\"error\": \"Not found\"}", 404);
        }
        
        send(client_socket, response.c_str(), response.length(), 0);
    }
    
    close(client_socket);
}

std::string HTTPServer::BuildResponse(const std::string& body, int status_code) {
    std::stringstream ss;
    ss << "HTTP/1.1 " << status_code << " OK\r\n";
    ss << "Content-Type: application/json\r\n";
    ss << "Access-Control-Allow-Origin: *\r\n";
    ss << "Content-Length: " << body.length() << "\r\n";
    ss << "\r\n";
    ss << body;
    return ss.str();
}
