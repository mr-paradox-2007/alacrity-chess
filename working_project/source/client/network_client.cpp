#include "network_client.hpp"
#include <iostream>

bool NetworkClient::Connect(const char* host, int port) {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        std::cerr << "Socket creation failed\n";
        return false;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return false;
    }
    
    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return false;
    }
    
    connected = true;
    return true;
}

bool NetworkClient::Send(const std::string& message) {
    if (!connected) return false;
    
    if (send(socket_fd, message.c_str(), message.length(), 0) < 0) {
        std::cerr << "Send failed\n";
        return false;
    }
    return true;
}

std::string NetworkClient::Receive(int max_size) {
    if (!connected) return "";
    
    char buffer[4096] = {0};
    int recv_size = recv(socket_fd, buffer, max_size, 0);
    
    if (recv_size < 0) {
        std::cerr << "Receive failed\n";
        return "";
    }
    
    if (recv_size == 0) {
        connected = false;
        return "";
    }
    
    return std::string(buffer, recv_size);
}

void NetworkClient::Disconnect() {
    if (socket_fd >= 0) {
        close(socket_fd);
        socket_fd = -1;
    }
    connected = false;
}
