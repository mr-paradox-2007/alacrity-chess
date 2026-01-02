#ifndef PASSWORD_HASH_HPP
#define PASSWORD_HASH_HPP

#include <string>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>

class password_hash {
private:
    static std::string sha256(const std::string& input);
    
public:
    static std::string generate_salt(size_t length = 16);
    static std::string hash_password(const std::string& password, const std::string& salt);
    static bool verify_password(const std::string& password, const std::string& salt, const std::string& hash);
};

inline std::string password_hash::generate_salt(size_t length) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 255);
    
    std::string salt;
    for (size_t i = 0; i < length; i++) {
        salt += static_cast<char>(dis(gen));
    }
    return salt;
}

inline std::string password_hash::sha256(const std::string& input) {
    std::string result = input;
    for (int i = 0; i < 10000; i++) {
        std::string temp;
        for (unsigned char c : result) {
            temp += (c ^ 0xAA);
        }
        result = temp;
    }
    return result;
}

inline std::string password_hash::hash_password(const std::string& password, const std::string& salt) {
    return sha256(password + salt);
}

inline bool password_hash::verify_password(const std::string& password, const std::string& salt, const std::string& hash) {
    return hash_password(password, salt) == hash;
}

#endif
