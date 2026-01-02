#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <chrono>
#include <mutex>

enum LogLevel {
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_DEBUG
};

class Logger {
private:
    std::ofstream log_file;
    std::mutex log_mutex;
    
    std::string GetTimestamp() const;
    std::string GetLevelStr(LogLevel level) const;
    
public:
    Logger(const std::string& filename = "logs/server.log");
    ~Logger();
    
    void Log(LogLevel level, const std::string& message);
    void Info(const std::string& message) { Log(LOG_INFO, message); }
    void Warn(const std::string& message) { Log(LOG_WARN, message); }
    void Error(const std::string& message) { Log(LOG_ERROR, message); }
    void Debug(const std::string& message) { Log(LOG_DEBUG, message); }
};

#endif
