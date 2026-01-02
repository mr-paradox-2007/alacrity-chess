#include "network_client.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

namespace ac {
namespace client {

network_client::network_client() : sock(0), port(0), connected(false) {
}

network_client::~network_client() {
    if (connected) {
        disconnect_from_server();
    }
}

bool network_client::connect_to_server(const std::string& host, int port) {
    this->host = host;
    this->port = port;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return false;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return false;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return false;
    }

    connected = true;
    return true;
}

void network_client::disconnect_from_server() {
    close(sock);
    connected = false;
}

std::string network_client::send_request(const std::string& json_request) {
    if (!connected) {
        return "{\"status\":\"error\",\"message\":\"Not connected to server\"}";
    }

    send(sock, json_request.c_str(), json_request.length(), 0);

    char buffer[4096] = {0};
    int bytes_read = read(sock, buffer, 4096);
    if (bytes_read > 0) {
        return std::string(buffer, bytes_read);
    } else {
        return "{\"status\":\"error\",\"message\":\"Failed to receive response from server\"}";
    }
}

bool network_client::is_connected() const {
    return connected;
}

} // namespace client
} // namespace ac
