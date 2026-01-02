#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>

namespace ac
{

enum class log_level
{
    debug,
    info,
    warn,
    error,
    fatal
};

class logger
{
private:
    std::ofstream file_stream_;
    std::mutex mtx_;
    std::string log_file_path_;
    static constexpr size_t max_file_size_bytes_ = 1024 * 1024;
    std::string app_identifier_;
    int process_id_;

    logger();
    void rotate_if_needed();
    bool file_was_rotated();
    std::string get_timestamp_utc();
    std::string level_to_string(log_level level);
    void initialize_app_identifier();

    void write_internal(log_level level,
                        const std::string& module,
                        int code,
                        const std::string& msg,
                        const std::string& src_file,
                        int line);

public:

    ~logger();

    static logger& get_instance();

    void set_log_file(const std::string& path);
    void set_app_name(const std::string& name);

    void log(log_level level,
             const std::string& module,
             int code,
             const std::string& msg,
             const std::string& src_file,
             int line)
    {
        write_internal(level, module, code, msg, src_file, line);
    }

    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;
};

} // namespace ac

#endif // LOGGER_HPP
