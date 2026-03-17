/**
 * Q13: LRU Cache with O(1) Operations
 *
 * PROBLEM:
 * Design and implement a Least Recently Used (LRU) cache with:
 * - O(1) time complexity for get() and put()
 * - Fixed capacity
 * - Automatic eviction of least recently used item when full
 *
 * REAL-WORLD USES:
 * - CPU caches
 * - Database query caches
 * - Web browser caches
 * - DNS caches
 *
 * DATA STRUCTURE:
 * Combine a hash map with a doubly-linked list:
 * - Hash map: O(1) lookup by key
 * - Doubly-linked list: O(1) move to front, O(1) remove from back
 *
 * OPERATIONS:
 * - get(key): Return value and mark as recently used
 * - put(key, value): Add/update and mark as recently used
 *   - If full, evict least recently used (tail of list)
 *
 * EMBEDDED CONSIDERATIONS:
 * - Fixed-size implementation (no dynamic allocation)
 * - Consider using intrusive linked list
 * - Memory pool for nodes
 */

#include <cstdint>
#include <cstdio>
#include <unordered_map>
#include <list>

// ============ YOUR IMPLEMENTATION ============

/**
 * LRU Cache using std::list and std::unordered_map.
 *
 * This is the standard implementation. For embedded systems,
 * you would replace these with fixed-size data structures.
 */
class LRUCache {
private:
    // TODO: Add data structures
    // - doubly linked list to maintain order (most recent at front)
    // - hash map from key to list iterator
    // - capacity limit

public:
    /**
     * Initialize LRU cache with given capacity.
     */
    LRUCache(int capacity) {
        // TODO: Initialize
    }

    /**
     * Get value for key.
     *
     * @param key The key to look up
     * @return The value, or -1 if not found
     *
     * If found, mark as recently used (move to front).
     */
    int get(int key) {
        // TODO: Implement
        // 1. Check if key exists in map
        // 2. If not, return -1
        // 3. Move the node to front of list
        // 4. Return the value

        return -1;  // Placeholder
    }

    /**
     * Put key-value pair into cache.
     *
     * @param key   The key
     * @param value The value
     *
     * If key exists, update value and mark as recently used.
     * If key doesn't exist and cache is full, evict LRU item.
     */
    void put(int key, int value) {
        // TODO: Implement
        // 1. If key exists, update value and move to front
        // 2. If key doesn't exist:
        //    a. If at capacity, remove LRU (back of list) and from map
        //    b. Add new node to front of list
        //    c. Add to map

    }

    /**
     * Get current number of items in cache.
     */
    int size() const {
        // TODO: Implement
        return 0;
    }
};

/**
 * BONUS: Fixed-size LRU Cache for embedded systems.
 *
 * No dynamic allocation - uses pre-allocated arrays.
 * Uses array indices instead of pointers for the linked list.
 */
template<int CAPACITY>
class FixedLRUCache {
    static_assert(CAPACITY > 0, "Capacity must be positive");

private:
    struct Node {
        int key;
        int value;
        int prev;  // Index of previous node (-1 if none)
        int next;  // Index of next node (-1 if none)
        bool used; // Is this slot in use?
    };

    Node nodes[CAPACITY];
    int head;  // Index of most recently used (-1 if empty)
    int tail;  // Index of least recently used (-1 if empty)
    int count; // Current number of items

    // Simple hash table for key lookup (open addressing)
    // In production, use better hash table implementation
    static const int HASH_SIZE = CAPACITY * 2;
    int hash_table[HASH_SIZE];  // Stores node index, -1 if empty

    int hash(int key) const {
        return ((key % HASH_SIZE) + HASH_SIZE) % HASH_SIZE;
    }

public:
    FixedLRUCache() : head(-1), tail(-1), count(0) {
        for (int i = 0; i < CAPACITY; i++) {
            nodes[i].used = false;
        }
        for (int i = 0; i < HASH_SIZE; i++) {
            hash_table[i] = -1;
        }
    }

    int get(int key) {
        // TODO: Implement fixed-size version
        return -1;
    }

    void put(int key, int value) {
        // TODO: Implement fixed-size version
    }

    int size() const {
        return count;
    }
};

// ============ TEST FRAMEWORK ============

bool test_basic_operations() {
    printf("--- Basic LRU Operations ---\n");
    int passed = 0;
    int total = 0;

    LRUCache cache(2);  // Capacity 2

    // Test put and get
    cache.put(1, 1);
    cache.put(2, 2);

    if (cache.get(1) == 1) {
        printf("PASS: get(1) = 1\n");
        passed++;
    } else {
        printf("FAIL: get(1) != 1\n");
    }
    total++;

    // Add new item, should evict key 2 (LRU)
    cache.put(3, 3);

    if (cache.get(2) == -1) {
        printf("PASS: key 2 was evicted (LRU)\n");
        passed++;
    } else {
        printf("FAIL: key 2 should have been evicted\n");
    }
    total++;

    if (cache.get(3) == 3) {
        printf("PASS: get(3) = 3\n");
        passed++;
    } else {
        printf("FAIL: get(3) != 3\n");
    }
    total++;

    // Update existing key
    cache.put(1, 10);
    if (cache.get(1) == 10) {
        printf("PASS: Updated value for key 1\n");
        passed++;
    } else {
        printf("FAIL: Update didn't work\n");
    }
    total++;

    printf("Basic tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_lru_order() {
    printf("--- LRU Eviction Order ---\n");
    int passed = 0;
    int total = 0;

    LRUCache cache(3);

    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);  // Order: 3, 2, 1 (3 is MRU)

    cache.get(1);      // Order: 1, 3, 2 (1 is now MRU)
    cache.put(4, 4);   // Evict 2 (LRU), Order: 4, 1, 3

    if (cache.get(2) == -1) {
        printf("PASS: Key 2 evicted after access pattern\n");
        passed++;
    } else {
        printf("FAIL: Key 2 should have been evicted\n");
    }
    total++;

    if (cache.get(1) == 1 && cache.get(3) == 3 && cache.get(4) == 4) {
        printf("PASS: Keys 1, 3, 4 still present\n");
        passed++;
    } else {
        printf("FAIL: Wrong keys present\n");
    }
    total++;

    // Verify correct LRU on continued use
    cache.get(3);      // Order: 3, 4, 1
    cache.put(5, 5);   // Evict 1, Order: 5, 3, 4

    if (cache.get(1) == -1) {
        printf("PASS: Key 1 evicted correctly\n");
        passed++;
    } else {
        printf("FAIL: Key 1 should have been evicted\n");
    }
    total++;

    printf("LRU order tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_edge_cases() {
    printf("--- Edge Cases ---\n");
    int passed = 0;
    int total = 0;

    // Capacity 1
    LRUCache cache1(1);
    cache1.put(1, 1);
    cache1.put(2, 2);  // Should evict 1

    if (cache1.get(1) == -1 && cache1.get(2) == 2) {
        printf("PASS: Capacity 1 cache works\n");
        passed++;
    } else {
        printf("FAIL: Capacity 1 cache\n");
    }
    total++;

    // Get non-existent key
    LRUCache cache2(2);
    if (cache2.get(999) == -1) {
        printf("PASS: Get non-existent returns -1\n");
        passed++;
    } else {
        printf("FAIL: Should return -1 for non-existent\n");
    }
    total++;

    // Overwrite same key multiple times
    LRUCache cache3(2);
    cache3.put(1, 1);
    cache3.put(1, 10);
    cache3.put(1, 100);

    if (cache3.get(1) == 100 && cache3.size() == 1) {
        printf("PASS: Multiple overwrites work correctly\n");
        passed++;
    } else {
        printf("FAIL: Multiple overwrites (val=%d, size=%d)\n",
               cache3.get(1), cache3.size());
    }
    total++;

    // Negative keys
    LRUCache cache4(2);
    cache4.put(-1, -100);
    if (cache4.get(-1) == -100) {
        printf("PASS: Negative keys work\n");
        passed++;
    } else {
        printf("FAIL: Negative keys\n");
    }
    total++;

    printf("Edge case tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_leetcode_example() {
    printf("--- LeetCode Example ---\n");

    // From LeetCode problem 146
    LRUCache cache(2);

    cache.put(1, 1);
    cache.put(2, 2);
    int v1 = cache.get(1);       // returns 1

    cache.put(3, 3);             // evicts key 2
    int v2 = cache.get(2);       // returns -1 (not found)

    cache.put(4, 4);             // evicts key 1
    int v3 = cache.get(1);       // returns -1 (not found)
    int v4 = cache.get(3);       // returns 3
    int v5 = cache.get(4);       // returns 4

    bool passed = (v1 == 1) && (v2 == -1) && (v3 == -1) &&
                  (v4 == 3) && (v5 == 4);

    if (passed) {
        printf("PASS: LeetCode example\n");
    } else {
        printf("FAIL: LeetCode example\n");
        printf("  v1=%d (exp 1), v2=%d (exp -1), v3=%d (exp -1)\n", v1, v2, v3);
        printf("  v4=%d (exp 3), v5=%d (exp 4)\n", v4, v5);
    }

    return passed;
}

int main() {
    printf("Running Q13: LRU Cache\n");
    printf("======================\n\n");

    bool all_passed = true;

    if (!test_basic_operations()) all_passed = false;
    if (!test_lru_order()) all_passed = false;
    if (!test_edge_cases()) all_passed = false;
    if (!test_leetcode_example()) all_passed = false;

    printf("\n======================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
