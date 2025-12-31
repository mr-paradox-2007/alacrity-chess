#include <hash_table/hash_table.hpp>
#include <logger/logger.hpp>
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;

int main()
{
    cout << "========== Hash Table Test ==========" << endl;
    cout << "Job: Test user authentication system using hash table" << endl;
    cout << "Purpose: Verify insert, get, authenticate, remove, and serialize functionality" << endl << endl;

    ac::logger::get_instance().set_log_file("tests/hash_table_test/test_data/hash_table_test.log");
    ac::logger::get_instance().set_app_name("hash_table_test");

    ds::hash_table users;

    cout << "Test 1: Insert users" << endl;
    ds::user_info user1;
    user1.password_hash = "salt:abc123hash";
    user1.elo_rating = 1500;
    user1.session_token = "sess_user1_token";

    ds::user_info user2;
    user2.password_hash = "salt:def456hash";
    user2.elo_rating = 1200;
    user2.session_token = "sess_user2_token";

    bool inserted1 = users.insert("alice", user1);
    bool inserted2 = users.insert("bob", user2);

    assert(inserted1 && "Failed to insert user1");
    assert(inserted2 && "Failed to insert user2");

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 1, "Successfully inserted 2 users", __FILE__, __LINE__);
    cout << "✓ Inserted alice and bob successfully" << endl << endl;

    cout << "Test 2: Get users by username" << endl;
    ds::user_info* retrieved = users.get("alice");
    assert(retrieved != nullptr && "Failed to get alice");
    assert(retrieved->elo_rating == 1500 && "ELO rating mismatch for alice");

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 2, "Successfully retrieved alice with correct ELO", __FILE__, __LINE__);
    cout << "✓ Retrieved alice with ELO: " << retrieved->elo_rating << endl << endl;

    cout << "Test 3: Authenticate user" << endl;
    bool auth_success = users.authenticate("bob", "salt:def456hash");
    assert(auth_success && "Authentication failed for bob");

    ds::user_info* bob_info = users.get("bob");
    assert(bob_info->is_online && "Bob should be marked online after authentication");

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 3, "Successfully authenticated bob and marked as online", __FILE__, __LINE__);
    cout << "✓ Bob authenticated and marked online" << endl << endl;

    cout << "Test 4: Authentication with wrong password" << endl;
    bool auth_fail = users.authenticate("alice", "wrong_password");
    assert(!auth_fail && "Should reject wrong password");

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 4, "Correctly rejected wrong password", __FILE__, __LINE__);
    cout << "✓ Correctly rejected wrong password" << endl << endl;

    cout << "Test 5: Get non-existent user" << endl;
    ds::user_info* not_found = users.get("charlie");
    assert(not_found == nullptr && "Should return nullptr for non-existent user");

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 5, "Correctly returned nullptr for non-existent user", __FILE__, __LINE__);
    cout << "✓ Returned nullptr for non-existent user" << endl << endl;

    cout << "Test 6: Insert more users to test collision handling" << endl;
    for (int i = 0; i < 50; i++)
    {
        ds::user_info new_user;
        new_user.password_hash = "hash_" + std::to_string(i);
        new_user.elo_rating = 1200 + i;
        new_user.is_online = false;

        std::string username = "user_" + std::to_string(i);
        users.insert(username, new_user);
    }

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 6, "Successfully inserted 50 additional users", __FILE__, __LINE__);
    cout << "✓ Inserted 50 additional users" << endl << endl;

    cout << "Test 7: Verify hash table resizing (should have resized)" << endl;
    ds::user_info* user_40 = users.get("user_40");
    assert(user_40 != nullptr && "Failed to retrieve user_40 after resizing");
    assert(user_40->elo_rating == 1240 && "ELO rating incorrect for user_40");

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 7, "Hash table resized and users retrieved successfully", __FILE__, __LINE__);
    cout << "✓ Hash table resized correctly" << endl << endl;

    cout << "Test 8: Remove user" << endl;
    bool removed = users.remove("alice");
    assert(removed && "Failed to remove alice");

    ds::user_info* alice_after = users.get("alice");
    assert(alice_after == nullptr && "Alice should not exist after removal");

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 8, "Successfully removed alice from hash table", __FILE__, __LINE__);
    cout << "✓ Removed alice successfully" << endl << endl;

    cout << "Test 9: Serialize hash table to disk" << endl;
    users.serialize_to_disk("tests/hash_table_test/test_data/hash_table_backup.bin");

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 9, "Serialized hash table to tests/hash_table_test/test_data/hash_table_backup.bin", __FILE__, __LINE__);
    cout << "✓ Serialized hash table to disk" << endl << endl;

    cout << "Test 10: Update user (insert with existing key)" << endl;
    ds::user_info bob_updated;
    bob_updated.password_hash = "salt:def456hash";
    bob_updated.elo_rating = 1800;
    bob_updated.is_online = true;
    bob_updated.session_token = "sess_bob_new_token";

    bool updated = users.insert("bob", bob_updated);
    assert(updated && "Failed to update bob");

    ds::user_info* bob_check = users.get("bob");
    assert(bob_check->elo_rating == 1800 && "Bob's ELO should be updated to 1800");

    ac::logger::get_instance().log(ac::log_level::info, "TEST", 10, "Successfully updated bob's information", __FILE__, __LINE__);
    cout << "✓ Updated bob's ELO to 1800" << endl << endl;

    cout << "========== All Tests Passed ==========" << endl;
    ac::logger::get_instance().log(ac::log_level::info, "TEST", 99, "All hash table tests passed successfully", __FILE__, __LINE__);

    return 0;
}
