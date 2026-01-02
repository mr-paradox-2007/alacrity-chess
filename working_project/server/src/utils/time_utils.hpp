#ifndef TIME_UTILS_HPP
#define TIME_UTILS_HPP

#include <chrono>
#include <cstdint>

class time_utils {
public:
    static uint64_t get_current_timestamp() {
        return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
    
    static uint64_t get_current_timestamp_ms() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
};

#endif
