#include <iostream>
#include <cassert>
#include <string>
#include <lru_cache/lru_cache.hpp>

using namespace std;
using namespace ds;

int main()
{
    cout << "========== LRU Cache Test ==========\n";
    cout << "Job: Test LRU Cache with generic template<int, string>\n";
    cout << "Purpose: Verify LRU eviction, promotion, and cache operations\n\n";

    int passed = 0;
    int total = 6;

    // Test 1: Put and Get
    cout << "Test 1: Put and Get\n";
    {
        lru_cache<int, string> cache(3);

        cache.put(1, "Alice");
        cache.put(2, "Bob");
        cache.put(3, "Charlie");

        assert(cache.size() == 3);

        auto val1 = cache.get(1);
        assert(val1.has_value());
        assert(val1.value() == "Alice");

        auto val2 = cache.get(2);
        assert(val2.has_value());
        assert(val2.value() == "Bob");

        auto val3 = cache.get(3);
        assert(val3.has_value());
        assert(val3.value() == "Charlie");

        cout << "✓ Inserted 3 items into cache with capacity 3\n";
        cout << "✓ Retrieved all items successfully\n";
        cout << "✓ Cache size: " << cache.size() << "\n";
        passed++;
    }

    // Test 2: Value Update
    cout << "\nTest 2: Value Update\n";
    {
        lru_cache<int, string> cache(3);

        cache.put(1, "Alice");
        cache.put(2, "Bob");
        cache.put(3, "Charlie");

        // Update key 1 with new value
        cache.put(1, "Alice_Updated");

        auto val1 = cache.get(1);
        assert(val1.has_value());
        assert(val1.value() == "Alice_Updated");

        // After getting key 1, it should be the most recently used
        // Put key 4 should evict key 2 (least recently used after updating key 1)
        cache.put(4, "Diana");

        auto val2 = cache.get(2);
        assert(!val2.has_value()); // Key 2 should be evicted

        cout << "✓ Updated key 1 value from Alice to Alice_Updated\n";
        cout << "✓ Verified new value is retrieved\n";
        cout << "✓ Key 2 evicted as least recently used\n";
        cout << "✓ Cache size: " << cache.size() << "\n";
        passed++;
    }

    // Test 3: LRU Eviction
    cout << "\nTest 3: LRU Eviction\n";
    {
        lru_cache<int, string> cache(3);

        cache.put(1, "Alice");
        cache.put(2, "Bob");
        cache.put(3, "Charlie");

        assert(cache.size() == 3);

        // Cache is full, adding key 4 should evict key 1 (least recently used)
        cache.put(4, "Diana");

        assert(cache.size() == 3);

        auto val1 = cache.get(1);
        assert(!val1.has_value()); // Key 1 should be evicted

        auto val4 = cache.get(4);
        assert(val4.has_value());
        assert(val4.value() == "Diana");

        cout << "✓ Created cache with capacity 3\n";
        cout << "✓ Put keys 1, 2, 3 (fills cache)\n";
        cout << "✓ Put key 4 (triggers eviction of key 1)\n";
        cout << "✓ Key 1 confirmed evicted\n";
        cout << "✓ Key 4 confirmed added\n";
        cout << "✓ Cache size remains: " << cache.size() << "\n";
        passed++;
    }

    // Test 4: Get Promotes Usage
    cout << "\nTest 4: Get Promotes Usage\n";
    {
        lru_cache<int, string> cache(3);

        cache.put(1, "Alice");
        cache.put(2, "Bob");
        cache.put(3, "Charlie");

        // Get key 1, making it most recently used
        auto val1 = cache.get(1);
        assert(val1.has_value());

        // Now key 2 is the least recently used
        // Put key 4 should evict key 2, not key 1
        cache.put(4, "Diana");

        assert(cache.size() == 3);

        auto check1 = cache.get(1);
        assert(check1.has_value()); // Key 1 should still exist

        auto check2 = cache.get(2);
        assert(!check2.has_value()); // Key 2 should be evicted

        auto check4 = cache.get(4);
        assert(check4.has_value());

        cout << "✓ Put keys 1, 2, 3 into cache\n";
        cout << "✓ Got key 1 (promoted to most recently used)\n";
        cout << "✓ Put key 4 (should evict key 2, not key 1)\n";
        cout << "✓ Key 1 confirmed present\n";
        cout << "✓ Key 2 confirmed evicted\n";
        cout << "✓ Key 4 confirmed added\n";
        passed++;
    }

    // Test 5: Cache Miss
    cout << "\nTest 5: Cache Miss\n";
    {
        lru_cache<int, string> cache(3);

        cache.put(1, "Alice");
        cache.put(2, "Bob");

        // Try to get a key that doesn't exist
        auto val_miss = cache.get(999);
        assert(!val_miss.has_value()); // Should return nullopt

        cout << "✓ Put keys 1, 2 into cache\n";
        cout << "✓ Tried to get non-existent key 999\n";
        cout << "✓ Correctly returned std::nullopt (no value)\n";
        cout << "✓ Cache size unchanged: " << cache.size() << "\n";
        passed++;
    }

    // Test 6: Clear
    cout << "\nTest 6: Clear\n";
    {
        lru_cache<int, string> cache(3);

        cache.put(1, "Alice");
        cache.put(2, "Bob");
        cache.put(3, "Charlie");

        assert(cache.size() == 3);

        cache.clear();

        assert(cache.size() == 0);
        assert(!cache.exists(1));
        assert(!cache.exists(2));
        assert(!cache.exists(3));

        auto val = cache.get(1);
        assert(!val.has_value());

        cout << "✓ Put 3 items into cache\n";
        cout << "✓ Called clear()\n";
        cout << "✓ Cache size is now 0\n";
        cout << "✓ All keys confirmed removed\n";
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
