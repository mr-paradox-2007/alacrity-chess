#include "../../includes/hash_table/hash_table.hpp"
#include "../../includes/logger/logger.hpp"
#include <iostream>
#include <string>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define DATA_DIR "alacrity-chess/tests/login_signup_demo_test/test_data"
#define DATA_FILE DATA_DIR "/user_data.bin"
#define LOG_FILE "alacrity-chess/tests/login_signup_demo_test/login_signup_demo.log"
#define LOG(level, module, code, msg) ac::logger::get_instance().log(level, module, code, msg, __FILE__, __LINE__)

void print_menu() {
    std::cout << "\n--- Alacrity Chess Auth Demo ---\n";
    std::cout << "1. Register a new user\n";
    std::cout << "2. Login with a user\n";
    std::cout << "3. Exit\n";
    std::cout << "Enter your choice: ";
}

// Function to lock, load, and unlock
void load_table(ds::hash_table& table) {
    int fd = open(DATA_FILE, O_CREAT | O_RDONLY, 0666);
    if (fd == -1) {
        // This might not be a fatal error if the file just doesn't exist yet.
        // deserialize_from_disk will handle the "file not found" case gracefully.
    } else {
        if (flock(fd, LOCK_SH) == -1) {
            LOG(ac::log_level::fatal, "MAIN", 501, "Failed to get shared lock on data file.");
            close(fd);
            return;
        }
    }
    
    table.deserialize_from_disk(DATA_FILE);

    if (fd != -1) {
        flock(fd, LOCK_UN);
        close(fd);
    }
}

// Function to lock, save, and unlock
void save_table(ds::hash_table& table) {
    int fd = open(DATA_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd == -1) {
        LOG(ac::log_level::fatal, "MAIN", 502, "Failed to open data file for writing.");
        return;
    }
    if (flock(fd, LOCK_EX) == -1) {
        LOG(ac::log_level::fatal, "MAIN", 503, "Failed to get exclusive lock on data file.");
        close(fd);
        return;
    }
    table.serialize_to_disk(DATA_FILE);
    flock(fd, LOCK_UN);
    close(fd);
}


int main() {
    ac::logger::get_instance().set_log_file(LOG_FILE);
    
    // Create test_data directory
    mkdir(DATA_DIR, 0777);

    ds::hash_table user_table;

    int choice;
    do {
        print_menu();
        std::cin >> choice;
        std::cin.ignore(); 

        load_table(user_table);

        if (choice == 1) {
            std::string username, password;
            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            std::cout << "Enter password: ";
            std::getline(std::cin, password);

            if (user_table.get(username) != nullptr) {
                LOG(ac::log_level::warn, "REGISTER", 201, "Username '" + username + "' already exists.");
            } else {
                ds::user_info new_user;
                new_user.password_hash = password; // In a real app, hash this!
                if (user_table.insert(username, new_user)) {
                    LOG(ac::log_level::info, "REGISTER", 3, "User '" + username + "' registered successfully.");
                    save_table(user_table);
                    LOG(ac::log_level::info, "MAIN", 4, "User data saved to " + std::string(DATA_FILE));
                } else {
                    LOG(ac::log_level::error, "REGISTER", 301, "Failed to register user '" + username + "'.");
                }
            }
        } else if (choice == 2) {
            std::string username, password;
            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            std::cout << "Enter password: ";
            std::getline(std::cin, password);
            
            if (user_table.authenticate(username, password)) {
                LOG(ac::log_level::info, "LOGIN", 5, "User '" + username + "' authenticated successfully.");
            } else {
                LOG(ac::log_level::error, "LOGIN", 302, "Authentication failed for user '" + username + "'.");
            }
        }
    } while (choice != 3);

    LOG(ac::log_level::info, "MAIN", 6, "Exiting demo.");
    return 0;
}
