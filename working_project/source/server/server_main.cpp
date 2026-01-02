#include "server.hpp"
#include "http_server.hpp"
#include "logger.hpp"
#include "user_manager.hpp"
#include "matchmaking_queue.hpp"
#include <iostream>
#include <thread>
#include <signal.h>

Logger g_logger("logs/server.log");
UserManager g_user_manager;
MatchmakingQueue g_matchmaking_queue;

ChessServer* g_server = nullptr;
HTTPServer* g_http_server = nullptr;

std::string HandleSignup(const std::string& body);
std::string HandleLogin(const std::string& body);

void signal_handler(int sig) {
    if (g_server || g_http_server) {
        std::cout << "\nShutting down servers...\n";
        if (g_server) g_server->Stop();
        if (g_http_server) g_http_server->Stop();
    }
}

int main() {
    g_logger.Info("Starting Chess Server");
    
    ChessServer server(5000);
    HTTPServer http_server(8080);
    
    g_server = &server;
    g_http_server = &http_server;
    
    signal(SIGINT, signal_handler);
    
    http_server.RegisterRoute("/api/signup", HandleSignup);
    http_server.RegisterRoute("/api/login", HandleLogin);
    
    if (!server.Start()) {
        g_logger.Error("Failed to start chess server");
        return 1;
    }
    
    if (!http_server.Start()) {
        g_logger.Error("Failed to start HTTP server");
        return 1;
    }
    
    std::thread chess_thread(&ChessServer::HandleConnections, &server);
    std::thread http_thread(&HTTPServer::HandleConnections, &http_server);
    
    chess_thread.join();
    http_thread.join();
    
    return 0;
}
