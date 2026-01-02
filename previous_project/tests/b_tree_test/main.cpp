#include "../../includes/b_tree/b_tree.hpp"
#include "../../includes/logger/logger.hpp"
#include <cassert>
#include <iostream>
#include <ctime>

using std::cout;
using std::endl;

#define LOG(level, module, code, msg) ac::logger::get_instance().log(level, module, code, msg, __FILE__, __LINE__)

int main()
{
    ac::logger::get_instance().set_log_file("tests/b_tree_test/test_data/b_tree_test.log");

    cout << "========== B-Tree Test ==========\n";
    cout << "Job: Test game history and leaderboard system using B-Tree\n";
    cout << "Purpose: Verify insert, range queries, serialization functionality\n\n";

    ds::b_tree tree;

    int passed = 0;
    int total = 12;

    try
    {
        cout << "Test 1: Insert game records\n";
        {
            ds::game_record game1;
            game1.player_id = 1;
            game1.opponent_id = 2;
            game1.result = "WIN";
            game1.pgn_moves = "e2e4 e7e5 g1f3";
            game1.timestamp = time(nullptr) - 86400;
            game1.duration_seconds = 600;
            game1.elo_change = 32;

            tree.insert(game1);

            ds::game_record game2;
            game2.player_id = 1;
            game2.opponent_id = 3;
            game2.result = "LOSS";
            game2.pgn_moves = "d2d4 d7d5";
            game2.timestamp = time(nullptr) - 43200;
            game2.duration_seconds = 800;
            game2.elo_change = -16;

            tree.insert(game2);

            assert(tree.size() == 2);
            cout << "✓ Inserted 2 game records\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 1, "Test 1 passed: Insert game records");
            passed++;
        }

        cout << "\nTest 2: Insert multiple records (10 games)\n";
        {
            for(int i = 4; i <= 10; i++)
            {
                ds::game_record game;
                game.player_id = 1;
                game.opponent_id = i;
                game.result = (i % 2 == 0) ? "WIN" : "LOSS";
                game.pgn_moves = "moves...";
                game.timestamp = time(nullptr) - (86400 * (10 - i));
                game.duration_seconds = 600 + (i * 50);
                game.elo_change = (i % 2 == 0) ? 32 : -16;

                tree.insert(game);
            }

            assert(tree.size() == 9);
            cout << "✓ Inserted 7 more games (total 9)\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 2, "Test 2 passed: Multiple inserts");
            passed++;
        }

        cout << "\nTest 3: Get all records\n";
        {
            auto all_records = tree.get_all_records();
            assert(all_records.size() == 9);
            cout << "✓ Retrieved all " << all_records.size() << " records\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 3, "Test 3 passed: Get all records");
            passed++;
        }

        cout << "\nTest 4: Range query by time\n";
        {
            time_t now = time(nullptr);
            time_t start = now - 172800;
            time_t end = now - 43200;

            auto range_records = tree.get_range(start, end);
            assert(range_records.size() > 0);
            cout << "✓ Found " << range_records.size() << " records in time range\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 4, "Test 4 passed: Range query by time");
            passed++;
        }

        cout << "\nTest 5: Range query for specific player\n";
        {
            time_t now = time(nullptr);
            time_t start = now - 864000;
            time_t end = now;

            auto player_records = tree.get_range(start, end, 1);
            assert(player_records.size() == 9);
            cout << "✓ Found " << player_records.size() << " records for player 1\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 5, "Test 5 passed: Range query for player");
            passed++;
        }

        cout << "\nTest 6: Get top records by ELO\n";
        {
            auto top_records = tree.get_top_by_elo(5);
            assert(top_records.size() <= 5);
            cout << "✓ Retrieved top " << top_records.size() << " records by ELO change\n";
            if(top_records.size() > 0)
            {
                cout << "  Top record ELO change: " << top_records[0].elo_change << "\n";
            }
            LOG(ac::log_level::info, "B_TREE_TEST", 6, "Test 6 passed: Top by ELO");
            passed++;
        }

        cout << "\nTest 7: Get top records for specific player\n";
        {
            auto player_top = tree.get_top_by_elo(3, 1);
            assert(player_top.size() <= 3);
            cout << "✓ Retrieved top " << player_top.size() << " records for player 1\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 7, "Test 7 passed: Top for player");
            passed++;
        }

        cout << "\nTest 8: Serialize B-tree to disk\n";
        {
            tree.serialize_to_disk("tests/b_tree_test/test_data/game_history.btree");

            FILE* file = fopen("tests/b_tree_test/test_data/game_history.btree", "rb");
            assert(file != nullptr);
            fclose(file);

            cout << "✓ B-tree serialized successfully\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 8, "Test 8 passed: Serialization");
            passed++;
        }

        cout << "\nTest 9: Deserialize B-tree from disk\n";
        {
            ds::b_tree tree_loaded;
            bool success = tree_loaded.deserialize_from_disk("tests/b_tree_test/test_data/game_history.btree");
            assert(success);
            assert(tree_loaded.size() == 9);

            auto loaded_records = tree_loaded.get_all_records();
            assert(loaded_records.size() == 9);

            cout << "✓ B-tree deserialized successfully with " << loaded_records.size() << " records\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 9, "Test 9 passed: Deserialization");
            passed++;
        }

        cout << "\nTest 10: Insert 50+ records and verify scalability\n";
        {
            ds::b_tree large_tree;

            for(int i = 0; i < 50; i++)
            {
                ds::game_record game;
                game.player_id = (i % 5) + 1;
                game.opponent_id = ((i + 1) % 5) + 1;
                game.result = (i % 3 == 0) ? "WIN" : (i % 3 == 1) ? "LOSS" : "DRAW";
                game.pgn_moves = "game_" + std::to_string(i);
                game.timestamp = time(nullptr) - (3600 * i);
                game.duration_seconds = 300 + (i % 500);
                game.elo_change = ((i % 2 == 0) ? 1 : -1) * (16 + (i % 32));

                large_tree.insert(game);
            }

            assert(large_tree.size() == 50);

            auto all_50 = large_tree.get_all_records();
            assert(all_50.size() == 50);

            auto top_10 = large_tree.get_top_by_elo(10);
            assert(top_10.size() == 10);

            cout << "✓ Inserted and managed 50 records successfully\n";
            cout << "✓ Retrieved top 10 from 50 records\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 10, "Test 10 passed: Scalability");
            passed++;
        }

        cout << "\nTest 11: Remove a record\n";
        {
            ds::b_tree remove_tree;
            ds::game_record g1, g2, g3;

            g1.player_id = 1; g1.opponent_id = 2; g1.timestamp = 100; g1.result = "WIN";
            g2.player_id = 1; g2.opponent_id = 3; g2.timestamp = 200; g2.result = "LOSS";
            g3.player_id = 2; g3.opponent_id = 1; g3.timestamp = 150; g3.result = "WIN";

            remove_tree.insert(g1);
            remove_tree.insert(g2);
            remove_tree.insert(g3);

            assert(remove_tree.size() == 3);
            cout << "✓ Inserted 3 records\n";

            bool removed = remove_tree.remove(g2);
            assert(removed);
            assert(remove_tree.size() == 2);
            cout << "✓ Removed 1 record\n";

            auto records = remove_tree.get_all_records();
            bool found_g2 = false;
            for(const auto& rec : records) {
                if (rec == g2) {
                    found_g2 = true;
                    break;
                }
            }
            assert(!found_g2);
            cout << "✓ Verified record is gone from tree\n";

            // Also verify that remove() returns false for a non-existent key
            bool removed_again = remove_tree.remove(g2);
            assert(!removed_again);
            assert(remove_tree.size() == 2);
            cout << "✓ Verified removing non-existent record fails gracefully\n";

            LOG(ac::log_level::info, "B_TREE_TEST", 11, "Test 11 passed: Remove record");
            passed++;
        }

        cout << "\nTest 12: Remove with node merge\n";
        {
            ds::b_tree merge_tree;
            std::vector<ds::game_record> records_to_insert;
            for (int i = 1; i <= 6; ++i) {
                ds::game_record r;
                r.player_id = i;
                r.opponent_id = i + 10;
                r.timestamp = i * 10;
                records_to_insert.push_back(r);
                merge_tree.insert(r);
            }
            assert(merge_tree.size() == 6);
            cout << "✓ Inserted 6 records, forcing a root split\n";

            // With b_order_ = 3, root should be {3}, left child {1,2}, right {4,5,6}
            // Remove 1, left child {2} is now underflow
            // Right sibling {4,5,6} has 3 keys, can't borrow
            // So, merge must happen
            bool removed_1 = merge_tree.remove(records_to_insert[0]); // remove record with player_id 1
            assert(removed_1);
            bool removed_4 = merge_tree.remove(records_to_insert[3]); // remove record with player_id 4
            assert(removed_4);
            
            assert(merge_tree.size() == 4);
            cout << "✓ Removed 2 records, triggering merge\n";

            auto remaining_records = merge_tree.get_all_records();
            assert(remaining_records.size() == 4);

            bool found_1 = false, found_4 = false;
            for (const auto& rec : remaining_records) {
                if (rec.player_id == 1) found_1 = true;
                if (rec.player_id == 4) found_4 = true;
            }
            assert(!found_1 && !found_4);
            cout << "✓ Verified correct records were removed and merged\n";
            LOG(ac::log_level::info, "B_TREE_TEST", 12, "Test 12 passed: Remove with node merge");
            passed++;
        }
    }
    catch(const std::exception& e)
    {
        cout << "Test failed with exception: " << e.what() << "\n";
        LOG(ac::log_level::error, "B_TREE_TEST", 500, std::string("Exception: ") + e.what());
    }

    cout << "\n========== Test Summary ==========\n";
    cout << "Passed: " << passed << "/" << total << "\n";

    if(passed == total)
    {
        cout << "========== All Tests Passed ==========\n";
        LOG(ac::log_level::info, "B_TREE_TEST", 0, "All 11 tests passed successfully");
        return 0;
    }
    else
    {
        cout << "========== Some Tests Failed ==========\n";
        LOG(ac::log_level::error, "B_TREE_TEST", 501, "Some tests failed");
        return 1;
    }
}
