#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <string>
#include <vector>
#include <mutex>
#include <ctime>

namespace ds
{

struct user_info
{
    std::string password_hash;
    int elo_rating;
    bool is_online;
    std::string session_token;
    time_t last_active;

    user_info() : elo_rating(1200), is_online(false), last_active(0)
    {
    }
};

class hash_table
{
private:
    struct bucket
    {
        std::string key;
        user_info value;
        bucket* next;

        bucket(const std::string& k, const user_info& v)
            : key(k), value(v), next(nullptr)
        {
        }
    };

    bucket** table_;
    size_t capacity_;
    size_t count_;
    std::mutex lock_;

    size_t hash(const std::string& key) const;
    void resize_if_needed();
    bool insert_unlocked(const std::string& username, const user_info& user);
    void clear_unlocked();

public:
    hash_table(size_t capacity = 1024);
    ~hash_table();

    bool insert(const std::string& username, const user_info& user);
    user_info* get(const std::string& username);
    bool authenticate(const std::string& username, const std::string& password);
    void serialize_to_disk(const std::string& filename);
    bool deserialize_from_disk(const std::string& filename);
    bool remove(const std::string& username);
    bool clear();

    hash_table(const hash_table&) = delete;
    hash_table& operator=(const hash_table&) = delete;
};

} // namespace ds

#endif // HASH_TABLE_HPP
