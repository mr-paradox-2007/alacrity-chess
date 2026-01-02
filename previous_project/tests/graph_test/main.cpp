#include <iostream>
#include <cassert>
#include <algorithm>
#include <logger/logger.hpp>
#include <graph/graph.hpp>

using namespace std;
using namespace ds;
using namespace ac;

int main()
{
    cout << "========== Graph Test ==========\n";
    cout << "Job: Test friends system using Graph (Adjacency List)\n";
    cout << "Purpose: Verify add/remove friends, BFS for recommendations, and persistence\n\n";

    int passed = 0;
    int total = 6;

    // Test 1: Add Friends
    cout << "Test 1: Add Friends and Verify Bidirectional Links\n";
    {
        graph g;

        bool result1 = g.add_friend(1, 2);
        assert(result1 == true);
        assert(g.are_friends(1, 2) == true);
        assert(g.are_friends(2, 1) == true);

        bool result2 = g.add_friend(2, 3);
        assert(result2 == true);

        // Try adding same friendship again (should return false)
        bool result3 = g.add_friend(1, 2);
        assert(result3 == false);

        // Try adding self-friendship (should return false)
        bool result4 = g.add_friend(1, 1);
        assert(result4 == false);

        cout << "✓ Successfully added bidirectional friendships\n";
        cout << "✓ Verified are_friends works both directions\n";
        cout << "✓ Duplicate and self-friendships rejected\n";
        passed++;
    }

    // Test 2: Get Friends
    cout << "\nTest 2: Get Friends List\n";
    {
        graph g;

        g.add_friend(1, 2);
        g.add_friend(1, 3);
        g.add_friend(1, 4);

        auto friends = g.get_friends(1);
        assert(friends.size() == 3);
        assert(find(friends.begin(), friends.end(), 2) != friends.end());
        assert(find(friends.begin(), friends.end(), 3) != friends.end());
        assert(find(friends.begin(), friends.end(), 4) != friends.end());

        // Test getting friends for non-existent player
        auto no_friends = g.get_friends(999);
        assert(no_friends.size() == 0);

        cout << "✓ Retrieved correct friend list for player 1 (3 friends)\n";
        cout << "✓ Non-existent player returns empty list\n";
        passed++;
    }

    // Test 3: Remove Friends
    cout << "\nTest 3: Remove Friends\n";
    {
        graph g;

        g.add_friend(1, 2);
        g.add_friend(1, 3);

        assert(g.are_friends(1, 2) == true);

        bool result = g.remove_friend(1, 2);
        assert(result == true);
        assert(g.are_friends(1, 2) == false);
        assert(g.are_friends(2, 1) == false);

        // Try removing non-existent friendship
        bool result2 = g.remove_friend(1, 2);
        assert(result2 == false);

        // Verify other friendships intact
        assert(g.are_friends(1, 3) == true);

        cout << "✓ Successfully removed bidirectional friendship\n";
        cout << "✓ Verified removal is bidirectional\n";
        cout << "✓ Cannot remove non-existent friendship\n";
        cout << "✓ Other friendships remain intact\n";
        passed++;
    }

    // Test 4: Friends of Friends (BFS)
    cout << "\nTest 4: Friends of Friends (BFS)\n";
    {
        graph g;

        // Create network: 1 -- 2 -- 3, 1 -- 4 -- 5, 6 -- 3
        // Friends of 1: 2, 4
        // Friends of friends of 1: 3, 5 (not 1, 2, 4)
        g.add_friend(1, 2);
        g.add_friend(1, 4);
        g.add_friend(2, 3);
        g.add_friend(4, 5);
        g.add_friend(6, 3);

        auto fof = g.get_friends_of_friends(1);

        // Should include 3 and 5, not 1, 2, 4
        assert(fof.size() >= 2);
        assert(find(fof.begin(), fof.end(), 3) != fof.end());
        assert(find(fof.begin(), fof.end(), 5) != fof.end());
        assert(find(fof.begin(), fof.end(), 1) == fof.end()); // Not self
        assert(find(fof.begin(), fof.end(), 2) == fof.end()); // Not direct friend
        assert(find(fof.begin(), fof.end(), 4) == fof.end()); // Not direct friend

        // 6 should be in FOF of 1 via 3
        if(find(fof.begin(), fof.end(), 6) != fof.end())
        {
            cout << "✓ Found 6 in friends of friends (3-hop connection)\n";
        }

        cout << "✓ BFS correctly found 2-hop connections (3, 5)\n";
        cout << "✓ Excluded direct friends and self\n";
        passed++;
    }

    // Test 5: Edge Cases
    cout << "\nTest 5: Edge Cases\n";
    {
        graph g;

        // Player with no friends
        auto empty_friends = g.get_friends(1);
        assert(empty_friends.size() == 0);

        // Non-existent player
        auto no_fof = g.get_friends_of_friends(999);
        assert(no_fof.size() == 0);

        // Player with only one friend (no FOF)
        g.add_friend(1, 2);
        auto fof_single = g.get_friends_of_friends(1);
        assert(fof_single.size() == 0);

        cout << "✓ Player with no friends returns empty list\n";
        cout << "✓ Non-existent player returns empty friends of friends\n";
        cout << "✓ Player with single friend has no friends of friends\n";
        passed++;
    }

    // Test 6: Persistence (Serialization/Deserialization)
    cout << "\nTest 6: Persistence (Serialize/Deserialize)\n";
    {
        const string filename = "./tests/graph_test/test_data/graph_test.bin";

        // Create and populate graph
        {
            graph g1;
            g1.add_friend(1, 2);
            g1.add_friend(1, 3);
            g1.add_friend(2, 4);
            g1.add_friend(3, 5);

            assert(g1.size() == 5);
            cout << "✓ Created graph with 5 nodes and 4 edges\n";

            g1.serialize_to_disk(filename);
            cout << "✓ Serialized to disk\n";
        }

        // Load and verify
        {
            graph g2;
            bool loaded = g2.deserialize_from_disk(filename);
            assert(loaded == true);

            assert(g2.size() == 5);
            cout << "✓ Deserialized from disk\n";

            // Verify all friendships
            assert(g2.are_friends(1, 2) == true);
            assert(g2.are_friends(1, 3) == true);
            assert(g2.are_friends(2, 4) == true);
            assert(g2.are_friends(3, 5) == true);

            cout << "✓ All friendships correctly restored\n";

            auto friends_1 = g2.get_friends(1);
            assert(friends_1.size() == 2);
            cout << "✓ Friend lists intact after deserialization\n";
        }

        passed++;
    }

    // Summary
    cout << "\n========== Test Summary ==========\n";
    cout << "Passed: " << passed << "/" << total << "\n";

    if(passed == total)
    {
        cout << "========== All Tests Passed ==========\n";
    }
    else
    {
        cout << "========== Some Tests Failed ==========\n";
    }

    return 0;
}
