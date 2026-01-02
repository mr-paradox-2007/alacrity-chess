#include <logger/logger.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <sys/stat.h>

#if defined(_WIN32)
    #include <windows.h>
    #include <processthreadsapi.h>
#else
    #include <unistd.h>
    #include <limits.h>
#endif

using std::cout;
using std::cerr;
using std::endl;

namespace ac
{

std::string get_project_root()
{
    std::filesystem::path exe_dir = std::filesystem::absolute(".");
    while (!exe_dir.empty() && exe_dir != exe_dir.parent_path())
    {
        if (std::filesystem::exists(exe_dir / "bin"))
        {
            return exe_dir.string();
        }
        exe_dir = exe_dir.parent_path();
    }
    return ".";
}

logger::logger()
{
    std::string project_root = get_project_root();
    log_file_path_ = project_root + "/logs/alacrity-chess.log";

    initialize_app_identifier();

    std::filesystem::create_directories(std::filesystem::path(log_file_path_).parent_path());

    file_stream_.open(log_file_path_, std::ios::app);
    if (!file_stream_.is_open())
    {
        cerr << "fatal: logger failed to open log file: " << log_file_path_ << endl;
    }
}

logger::~logger()
{
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
}

logger& logger::get_instance()
{
    static logger instance;
    return instance;
}

void logger::initialize_app_identifier()
{
#if defined(_WIN32)
    process_id_ = GetCurrentProcessId();

    char path[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
    if (length > 0) {
        std::filesystem::path exe_path(path);
        app_identifier_ = exe_path.filename().string();
    } else {
        app_identifier_ = "unknown_app";
    }
#else
    process_id_ = getpid();

    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        std::filesystem::path exe_path(path);
        app_identifier_ = exe_path.filename().string();
    } else {
        app_identifier_ = "unknown_app";
    }
#endif
}

void logger::set_app_name(const std::string& name)
{
    std::lock_guard<std::mutex> lock(mtx_);
    app_identifier_ = name;
}

void logger::set_log_file(const std::string& path)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (file_stream_.is_open()) {
        file_stream_.close();
    }
    log_file_path_ = path;
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    file_stream_.open(log_file_path_, std::ios::app);
    if (!file_stream_.is_open()) {
        cerr << "CRITICAL: Failed to open new log file at " << log_file_path_ << endl;
    }
}

std::string logger::level_to_string(log_level level)
{
    switch (level)
    {
        case log_level::debug: return "DEBUG";
        case log_level::info:  return "INFO";
        case log_level::warn:  return "WARN";
        case log_level::error: return "ERROR";
        case log_level::fatal: return "FATAL";
    }
    return "UNKNOWN";
}

std::string logger::get_timestamp_utc()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm utc {};

#if defined(_WIN32)
    gmtime_s(&utc, &time);
#else
    if (gmtime_r(&time, &utc) == nullptr) {}
#endif

    std::ostringstream oss;
    oss << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

bool logger::file_was_rotated()
{
    if (!std::filesystem::exists(log_file_path_)) {
        return true;
    }

#if defined(_WIN32)
    try {
        size_t current_size = std::filesystem::file_size(log_file_path_);
        auto current_pos = file_stream_.tellp();
        if (current_pos > 0 && current_size < (size_t)current_pos / 2) {
            return true;
        }
    } catch (...) {
        return true;
    }
#else
    struct stat current_stat;
    if (stat(log_file_path_.c_str(), &current_stat) != 0) {
        return true;
    }

    try {
        size_t disk_size = std::filesystem::file_size(log_file_path_);
        auto stream_pos = file_stream_.tellp();

        if (stream_pos > 0 && disk_size < (std::streampos)stream_pos / 2) {
            return true;
        }
    } catch (...) {
        return true;
    }
#endif

    return false;
}

void logger::rotate_if_needed()
{
    if (!file_stream_.is_open()) {
        return;
    }

    if (!std::filesystem::exists(log_file_path_)) return;
    size_t current_size = 0;
    try {
        current_size = std::filesystem::file_size(log_file_path_);
    } catch (const std::filesystem::filesystem_error& e) {
        cerr << "LOG ERROR: Failed to check file size for rotation: " << e.what() << endl;
        return;
    }

    if (current_size < max_file_size_bytes_) return;

    try {
        std::string new_path = log_file_path_ + "." + get_timestamp_utc() + ".log";

        file_stream_.close();

        std::error_code ec;
        std::filesystem::rename(log_file_path_, new_path, ec);

        if (ec) {
            cerr << "LOG WARN: Rename failed (code: " << ec.value() << ", msg: " << ec.message() << "). File possibly locked by another process." << endl;
        }

        file_stream_.open(log_file_path_, std::ios::app);
    } catch (const std::exception& e) {
        cerr << "LOG ERROR: Catastrophic rotation failure: " << e.what() << endl;
    }
}

void logger::write_internal(log_level level,
                            const std::string& module,
                            int code,
                            const std::string& msg,
                            const std::string& src_file,
                            int line)
{
    std::lock_guard<std::mutex> lock(mtx_);

    std::string level_str = level_to_string(level);

    if (file_stream_.is_open() && file_was_rotated()) {
        file_stream_.close();
    }

    if (!file_stream_.is_open()) {
        file_stream_.open(log_file_path_, std::ios::app);
    }

    if (!file_stream_.is_open()) {
        std::string fallback_msg = "[" + level_str + "] " + msg + " (File stream permanently closed)";
        if (level == log_level::error || level == log_level::fatal) {
            cerr << fallback_msg << endl;
        } else {
            cout << fallback_msg << endl;
        }

        if (level == log_level::fatal) {
            std::exit(code);
        }
        return;
    }

    rotate_if_needed();

    std::string timestamp = get_timestamp_utc();

    std::ostringstream file_msg;
    file_msg << "[" << timestamp << "]"
            << " level=" << level_str
            << " module=" << module
            << " code=" << code
            << " msg=\"" << msg << "\""
            << " file=\"" << src_file << "\""
            << " line=" << line;

    if (file_stream_.is_open()) {
        file_stream_ << file_msg.str() << "\n";
        file_stream_.flush();
    }

    std::ostringstream console_msg;
    console_msg << "[" << timestamp << "]"
                << " [" << level_str << "]"
                << " [" << module << "]"
                << " " << msg;

    if (level == log_level::error || level == log_level::fatal) {
        cerr << console_msg.str() << endl;
    } else {
        cout << console_msg.str() << endl;
    }

    if (level == log_level::fatal)
    {
        if (file_stream_.is_open()) {
            file_stream_.close();
        }
        std::exit(code);
    }
}

} // namespace ac
