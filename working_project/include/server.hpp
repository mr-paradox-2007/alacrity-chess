#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>

struct ClientSession {
    int socket_fd;
    std::string username;
    bool authenticated;
};

class ChessServer {
private:
    int server_socket;
    int port;
    bool running;
    std::map<int, ClientSession> clients;
    std::mutex clients_mutex;
    
public:
    ChessServer(int port = 5000);
    ~ChessServer();
    
    bool Start();
    void Stop();
    bool IsRunning() const { return running; }
    void HandleConnections();
    void HandleClient(int client_socket);
};

#endif // SERVER_HPP
