#include "../common/logger/logger.hpp"
#include <iostream>

// Define a module name for this test file
#define MODULE_NAME "LOGGER_TEST"

int main() {
    // Since the logger is a singleton, we can just call it
    // The first call will instantiate it.
    std::cout << "--- Logger Test Start ---" << std::endl;

    ac::Logger::get_instance().log(ac::LogLevel::info, MODULE_NAME, 1, "Logger test message 1.", __FILE__, __LINE__);
    ac::Logger::get_instance().log(ac::LogLevel::warn, MODULE_NAME, 2, "Logger test message 2.", __FILE__, __LINE__);
    ac::Logger::get_instance().log(ac::LogLevel::error, MODULE_NAME, 3, "Logger test message 3.", __FILE__, __LINE__);

    std::cout << "--- Logger Test End ---" << std::endl;
    std::cout << "Check console output and the contents of 'alacrity-chess/logs/alacrity-chess.log'" << std::endl;
    
    return 0;
}
