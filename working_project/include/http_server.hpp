#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string>
#include <map>
#include <functional>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

typedef std::function<std::string(const std::string&)> RequestHandler;

class HTTPServer {
private:
    int server_socket;
    int port;
    bool running;
    std::map<std::string, RequestHandler> routes;
    
public:
    HTTPServer(int port = 8080);
    ~HTTPServer();
    
    bool Start();
    void Stop();
    void RegisterRoute(const std::string& path, RequestHandler handler);
    void HandleConnections();
    void HandleClient(int client_socket);
    
private:
    std::string ParseRequest(const std::string& request);
    std::string BuildResponse(const std::string& body, int status_code = 200);
};

#endif
