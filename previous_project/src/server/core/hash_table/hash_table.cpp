#include <hash_table/hash_table.hpp>
#include <iostream>
#include <fstream>
#include <functional>
#include <cstring>

using std::cout;
using std::endl;

namespace ds
{

hash_table::hash_table(size_t capacity) : capacity_(capacity), count_(0)
{
    table_ = new bucket*[capacity_];
    for (size_t i = 0; i < capacity_; ++i)
    {
        table_[i] = nullptr;
    }
}

hash_table::~hash_table()
{
    clear_unlocked();
    delete[] table_;
}

size_t hash_table::hash(const std::string& key) const
{
    std::hash<std::string> hasher;
    return hasher(key) % capacity_;
}

bool hash_table::insert_unlocked(const std::string& username, const user_info& user)
{
    if (count_ >= capacity_ / 2)
    {
        resize_if_needed();
    }

    size_t index = hash(username);
    bucket* current = table_[index];

    while (current != nullptr)
    {
        if (current->key == username)
        {
            current->value = user;
            return true;
        }
        current = current->next;
    }

    bucket* new_bucket = new bucket(username, user);
    new_bucket->next = table_[index];
    table_[index] = new_bucket;
    count_++;

    return true;
}

bool hash_table::insert(const std::string& username, const user_info& user)
{
    std::lock_guard<std::mutex> lock(lock_);
    return insert_unlocked(username, user);
}

user_info* hash_table::get(const std::string& username)
{
    std::lock_guard<std::mutex> lock(lock_);

    size_t index = hash(username);
    bucket* current = table_[index];

    while (current != nullptr)
    {
        if (current->key == username)
        {
            return &(current->value);
        }
        current = current->next;
    }

    return nullptr;
}

bool hash_table::authenticate(const std::string& username, const std::string& password)
{
    std::lock_guard<std::mutex> lock(lock_);

    size_t index = hash(username);
    bucket* current = table_[index];

    while (current != nullptr)
    {
        if (current->key == username)
        {
            if (current->value.password_hash == password)
            {
                current->value.is_online = true;
                current->value.last_active = time(nullptr);
                return true;
            }
            return false;
        }
        current = current->next;
    }

    return false;
}

bool hash_table::remove(const std::string& username)
{
    std::lock_guard<std::mutex> lock(lock_);

    size_t index = hash(username);
    bucket* current = table_[index];
    bucket* prev = nullptr;

    while (current != nullptr)
    {
        if (current->key == username)
        {
            if (prev == nullptr)
            {
                table_[index] = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            delete current;
            count_--;
            return true;
        }
        prev = current;
        current = current->next;
    }

    return false;
}

void hash_table::serialize_to_disk(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(lock_);

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        cout << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }

    file.write(reinterpret_cast<char*>(&count_), sizeof(count_));

    for (size_t i = 0; i < capacity_; ++i)
    {
        bucket* current = table_[i];
        while (current != nullptr)
        {
            size_t key_len = current->key.length();
            file.write(reinterpret_cast<char*>(&key_len), sizeof(key_len));
            file.write(current->key.c_str(), key_len);

            size_t hash_len = current->value.password_hash.length();
            file.write(reinterpret_cast<char*>(&hash_len), sizeof(hash_len));
            file.write(current->value.password_hash.c_str(), hash_len);

            file.write(reinterpret_cast<char*>(&current->value.elo_rating), sizeof(current->value.elo_rating));
            file.write(reinterpret_cast<char*>(&current->value.is_online), sizeof(current->value.is_online));

            size_t token_len = current->value.session_token.length();
            file.write(reinterpret_cast<char*>(&token_len), sizeof(token_len));
            file.write(current->value.session_token.c_str(), token_len);

            file.write(reinterpret_cast<char*>(&current->value.last_active), sizeof(current->value.last_active));

            current = current->next;
        }
    }

    file.close();
    cout << "Hash table serialized to " << filename << "\n";
}

bool hash_table::deserialize_from_disk(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(lock_);

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        cout << "FYI: Could not open file " << filename << " for reading. Starting fresh.\n";
        return false;
    }

    clear_unlocked();

    size_t record_count = 0;
    file.read(reinterpret_cast<char*>(&record_count), sizeof(record_count));

    for (size_t i = 0; i < record_count; ++i)
    {
        size_t key_len;
        file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
        std::string key(key_len, '\0');
        file.read(&key[0], key_len);

        user_info user;
        size_t hash_len;
        file.read(reinterpret_cast<char*>(&hash_len), sizeof(hash_len));
        user.password_hash.resize(hash_len);
        file.read(&user.password_hash[0], hash_len);

        file.read(reinterpret_cast<char*>(&user.elo_rating), sizeof(user.elo_rating));
        file.read(reinterpret_cast<char*>(&user.is_online), sizeof(user.is_online));

        size_t token_len;
        file.read(reinterpret_cast<char*>(&token_len), sizeof(token_len));
        user.session_token.resize(token_len);
        file.read(&user.session_token[0], token_len);

        file.read(reinterpret_cast<char*>(&user.last_active), sizeof(user.last_active));

        insert_unlocked(key, user);
    }

    file.close();
    cout << "Hash table deserialized from " << filename << "\n";
    return true;
}

void hash_table::clear_unlocked()
{
    for (size_t i = 0; i < capacity_; ++i)
    {
        bucket* current = table_[i];
        while (current != nullptr)
        {
            bucket* next = current->next;
            delete current;
            current = next;
        }
        table_[i] = nullptr;
    }
    count_ = 0;
}

bool hash_table::clear()
{
    std::lock_guard<std::mutex> lock(lock_);
    clear_unlocked();
    return true;
}

void hash_table::resize_if_needed()
{
    size_t new_capacity = capacity_ * 2;
    bucket** new_table = new bucket*[new_capacity];

    for (size_t i = 0; i < new_capacity; ++i)
    {
        new_table[i] = nullptr;
    }

    for (size_t i = 0; i < capacity_; ++i)
    {
        bucket* current = table_[i];
        while (current != nullptr)
        {
            bucket* next = current->next;

            std::hash<std::string> hasher;
            size_t new_index = hasher(current->key) % new_capacity;

            current->next = new_table[new_index];
            new_table[new_index] = current;

            current = next;
        }
    }

    delete[] table_;
    table_ = new_table;
    capacity_ = new_capacity;
}

} // namespace ds
