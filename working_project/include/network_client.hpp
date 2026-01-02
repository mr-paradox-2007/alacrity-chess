#ifndef NETWORK_CLIENT_HPP
#define NETWORK_CLIENT_HPP

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class NetworkClient {
private:
    int socket_fd;
    struct sockaddr_in server_addr;
    bool connected;

public:
    NetworkClient() : socket_fd(-1), connected(false) {}
    
    ~NetworkClient() {
        Disconnect();
    }
    
    bool Connect(const char* host, int port);
    bool Send(const std::string& message);
    std::string Receive(int max_size = 4096);
    void Disconnect();
    bool IsConnected() const { return connected; }
};

#endif // NETWORK_CLIENT_HPP
