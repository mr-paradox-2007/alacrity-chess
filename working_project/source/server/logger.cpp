#include "logger.hpp"
#include <iostream>

std::string Logger::GetTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::string timestamp = std::ctime(&time);
    timestamp.pop_back();
    return timestamp;
}

std::string Logger::GetLevelStr(LogLevel level) const {
    switch (level) {
        case LOG_INFO: return "INFO";
        case LOG_WARN: return "WARN";
        case LOG_ERROR: return "ERROR";
        case LOG_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

Logger::Logger(const std::string& filename) {
    log_file.open(filename, std::ios::app);
}

Logger::~Logger() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

void Logger::Log(LogLevel level, const std::string& message) {
    std::unique_lock<std::mutex> lock(log_mutex);
    
    std::string log_line = "[" + GetTimestamp() + "] [" + GetLevelStr(level) + "] " + message;
    
    if (log_file.is_open()) {
        log_file << log_line << "\n";
        log_file.flush();
    }
    
    std::cout << log_line << "\n";
}
