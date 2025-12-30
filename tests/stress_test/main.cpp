#include "../common/logger/logger.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <random>

// A simple command-line parser
std::string get_cmd_option(int argc, char* argv[], const std::string& option) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == option && i + 1 < argc) {
            return argv[i + 1];
        }
    }
    return "";
}

int main(int argc, char* argv[]) {
    std::string module_name = get_cmd_option(argc, argv, "--module");
    if (module_name.empty()) {
        module_name = "DEFAULT_MODULE";
    }

    // Setup for random sleep times
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(10, 100); // ms

    long long counter = 0;
    std::cout << "Starting stress test for module: " << module_name << ". Press Ctrl+C to stop." << std::endl;

    while (true) {
        counter++;
        std::string message = "Stress test message #" + std::to_string(counter);

        // Log messages with varying levels
        if (counter % 100 == 0) {
             ac::Logger::get_instance().log(ac::LogLevel::error, module_name.c_str(), 500, message, __FILE__, __LINE__);
        } else if (counter % 20 == 0) {
            ac::Logger::get_instance().log(ac::LogLevel::warn, module_name.c_str(), 200, message, __FILE__, __LINE__);
        } else {
            ac::Logger::get_instance().log(ac::LogLevel::info, module_name.c_str(), 1, message, __FILE__, __LINE__);
        }

        // Sleep for a random duration to simulate work and avoid perfect lockstep
        std::this_thread::sleep_for(std::chrono::milliseconds(distrib(gen)));
    }

    return 0;
}
