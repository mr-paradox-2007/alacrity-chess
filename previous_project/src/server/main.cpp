#include "server/server.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    int port = 8080;  // Default port
    
    // Parse command line arguments
    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid port: " << argv[1] << std::endl;
            return 1;
        }
    }
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║     Alacrity Chess - Server (Phase 1)  ║\n";
    std::cout << "║     Backend: C++ | Protocol: WebSocket ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    try {
        ac::server::Server server;
        server.run(port);
    } catch (const std::exception& e) {
        std::cerr << "[MAIN] Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
