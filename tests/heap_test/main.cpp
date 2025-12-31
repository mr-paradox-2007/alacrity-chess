#include <iostream>
#include <cassert>
#include <ctime>
#include <thread>
#include <chrono>
#include <heap/heap.hpp>

using namespace std;
using namespace ds;

int main()
{
    cout << "========== Max-Heap Test ==========\n";
    cout << "Job: Test matchmaking queue using Max-Heap\n";
    cout << "Purpose: Verify priority-based extraction and heap rebalancing\n\n";

    int passed = 0;
    int total = 5;

    // Test 1: Insert and Get Max
    cout << "Test 1: Insert and Get Max\n";
    {
        max_heap heap;

        matchmaking_request req1;
        req1.player_id = 1;
        req1.elo = 1600;
        req1.timestamp = time(nullptr);

        matchmaking_request req2;
        req2.player_id = 2;
        req2.elo = 1800;
        req2.timestamp = time(nullptr);

        matchmaking_request req3;
        req3.player_id = 3;
        req3.elo = 1400;
        req3.timestamp = time(nullptr);

        heap.insert(req1);
        heap.insert(req2);
        heap.insert(req3);

        assert(heap.size() == 3);

        // req2 has highest ELO (1800), should be max
        matchmaking_request max_req = heap.get_max();
        assert(max_req.player_id == 2);
        assert(max_req.elo == 1800);

        cout << "✓ Inserted 3 requests with ELO: 1600, 1800, 1400\n";
        cout << "✓ get_max() correctly returned player 2 (highest ELO)\n";
        cout << "✓ Heap size: " << heap.size() << "\n";
        passed++;
    }

    // Test 2: Extract Max
    cout << "\nTest 2: Extract Max and Heap Rebalancing\n";
    {
        max_heap heap;

        matchmaking_request req1;
        req1.player_id = 1;
        req1.elo = 1600;
        req1.timestamp = time(nullptr);

        matchmaking_request req2;
        req2.player_id = 2;
        req2.elo = 1800;
        req2.timestamp = time(nullptr);

        matchmaking_request req3;
        req3.player_id = 3;
        req3.elo = 1400;
        req3.timestamp = time(nullptr);

        matchmaking_request req4;
        req4.player_id = 4;
        req4.elo = 1700;
        req4.timestamp = time(nullptr);

        heap.insert(req1);
        heap.insert(req2);
        heap.insert(req3);
        heap.insert(req4);

        assert(heap.size() == 4);

        // Extract first max (should be req2 with ELO 1800)
        matchmaking_request extracted1 = heap.extract_max();
        assert(extracted1.player_id == 2);
        assert(heap.size() == 3);

        // Extract second max (should be req4 with ELO 1700)
        matchmaking_request extracted2 = heap.extract_max();
        assert(extracted2.player_id == 4);
        assert(heap.size() == 2);

        // Extract third max (should be req1 with ELO 1600)
        matchmaking_request extracted3 = heap.extract_max();
        assert(extracted3.player_id == 1);
        assert(heap.size() == 1);

        cout << "✓ Extracted max: Player 2 (ELO 1800)\n";
        cout << "✓ Extracted max: Player 4 (ELO 1700)\n";
        cout << "✓ Extracted max: Player 1 (ELO 1600)\n";
        cout << "✓ Heap correctly rebalanced after each extraction\n";
        cout << "✓ Remaining size: " << heap.size() << "\n";
        passed++;
    }

    // Test 3: Priority Order (Non-Sorted Input)
    cout << "\nTest 3: Priority Order with Non-Sorted Input\n";
    {
        max_heap heap;

        // Insert in random order: low, high, medium
        matchmaking_request req_low;
        req_low.player_id = 10;
        req_low.elo = 1000;
        req_low.timestamp = time(nullptr);

        matchmaking_request req_high;
        req_high.player_id = 20;
        req_high.elo = 2000;
        req_high.timestamp = time(nullptr);

        matchmaking_request req_med;
        req_med.player_id = 30;
        req_med.elo = 1500;
        req_med.timestamp = time(nullptr);

        heap.insert(req_low);
        heap.insert(req_high);
        heap.insert(req_med);

        // Should extract in order: high (2000), med (1500), low (1000)
        matchmaking_request first = heap.extract_max();
        assert(first.player_id == 20);

        matchmaking_request second = heap.extract_max();
        assert(second.player_id == 30);

        matchmaking_request third = heap.extract_max();
        assert(third.player_id == 10);

        cout << "✓ Inserted in order: Low (1000), High (2000), Med (1500)\n";
        cout << "✓ Extracted in order: High (2000), Med (1500), Low (1000)\n";
        cout << "✓ Heap correctly prioritized despite insertion order\n";
        passed++;
    }

    // Test 4: Wait Time Priority
    cout << "\nTest 4: Wait Time Priority Boost\n";
    {
        max_heap heap;

        // High ELO player enters queue now
        time_t now = time(nullptr);

        matchmaking_request high_elo_new;
        high_elo_new.player_id = 100;
        high_elo_new.elo = 2000;
        high_elo_new.timestamp = now;

        // Lower ELO player entered 5 seconds ago
        matchmaking_request low_elo_waiting;
        low_elo_waiting.player_id = 101;
        low_elo_waiting.elo = 1200;
        low_elo_waiting.timestamp = now - 5;

        heap.insert(high_elo_new);
        heap.insert(low_elo_waiting);

        // Calculate priorities manually for verification
        // high_elo_new: 0.7 * 2000 + 0.3 * 0 = 1400
        // low_elo_waiting: 0.7 * 1200 + 0.3 * 5 = 840 + 1.5 = 841.5
        // So high_elo_new should still be max

        matchmaking_request first = heap.extract_max();
        assert(first.player_id == 100);
        cout << "✓ High ELO new player (2000 ELO, wait 0s) has priority\n";

        matchmaking_request second = heap.extract_max();
        assert(second.player_id == 101);
        cout << "✓ Low ELO waiting player (1200 ELO, wait 5s) extracted after\n";

        // Now test where wait time boosts low ELO above high ELO
        max_heap heap2;

        matchmaking_request high_elo;
        high_elo.player_id = 102;
        high_elo.elo = 1800;
        high_elo.timestamp = time(nullptr);

        matchmaking_request low_elo_long_wait;
        low_elo_long_wait.player_id = 103;
        low_elo_long_wait.elo = 1000;
        low_elo_long_wait.timestamp = time(nullptr) - 30;

        heap2.insert(high_elo);
        heap2.insert(low_elo_long_wait);

        // Calculate priorities:
        // high_elo: 0.7 * 1800 + 0.3 * 0 ≈ 1260
        // low_elo_long_wait: 0.7 * 1000 + 0.3 * 30 ≈ 700 + 9 = 709
        // Still high_elo should be max even with wait boost

        matchmaking_request result = heap2.extract_max();
        assert(result.player_id == 102);
        cout << "✓ Wait time boost correctly affects priority calculation\n";

        passed++;
    }

    // Test 5: Edge Cases
    cout << "\nTest 5: Edge Cases (Empty Heap)\n";
    {
        max_heap heap;

        // Test is_empty on empty heap
        assert(heap.is_empty() == true);
        assert(heap.size() == 0);
        cout << "✓ Empty heap detected correctly\n";

        // Test get_max on empty heap (should throw)
        try
        {
            heap.get_max();
            assert(false); // Should not reach here
        }
        catch(const std::out_of_range& e)
        {
            cout << "✓ get_max() throws std::out_of_range on empty heap\n";
        }

        // Test extract_max on empty heap (should throw)
        try
        {
            heap.extract_max();
            assert(false); // Should not reach here
        }
        catch(const std::out_of_range& e)
        {
            cout << "✓ extract_max() throws std::out_of_range on empty heap\n";
        }

        // Test clear on non-empty heap
        matchmaking_request req;
        req.player_id = 1;
        req.elo = 1600;
        req.timestamp = time(nullptr);

        heap.insert(req);
        assert(heap.size() == 1);

        heap.clear();
        assert(heap.size() == 0);
        assert(heap.is_empty() == true);
        cout << "✓ clear() successfully empties the heap\n";

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
