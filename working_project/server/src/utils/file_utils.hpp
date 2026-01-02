#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <fstream>
#include <string>
#include <vector>

class file_utils {
public:
    static bool file_exists(const std::string& filename);
    static std::string read_file(const std::string& filename);
    static bool write_file(const std::string& filename, const std::string& content);
    static bool append_file(const std::string& filename, const std::string& content);
    static bool delete_file(const std::string& filename);
};

inline bool file_utils::file_exists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

inline std::string file_utils::read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return "";
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return content;
}

inline bool file_utils::write_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) return false;
    file << content;
    file.close();
    return true;
}

inline bool file_utils::append_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::app);
    if (!file) return false;
    file << content;
    file.close();
    return true;
}

inline bool file_utils::delete_file(const std::string& filename) {
    return std::remove(filename.c_str()) == 0;
}

#endif
