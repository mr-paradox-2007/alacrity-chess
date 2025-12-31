#ifndef AUTH_API_HPP
#define AUTH_API_HPP

#include <string>
#include <memory>
#include "../hash_table/hash_table.hpp"

namespace ac {
namespace server {
namespace api {

class AuthAPI {
public:
    AuthAPI(ds::hash_table* user_table);
    
    // Parse request and route to appropriate handler
    std::string handle_request(const std::string& raw_message);
    
private:
    ds::hash_table* user_table_;
    
    // API Handlers
    std::string handle_register(const std::string& username, const std::string& password);
    std::string handle_login(const std::string& username, const std::string& password);
    
    // JSON parsing helpers
    std::string extract_field(const std::string& json, const std::string& field);
    bool is_valid_credentials(const std::string& username, const std::string& password);
};

}  // namespace api
}  // namespace server
}  // namespace ac

#endif // AUTH_API_HPP
