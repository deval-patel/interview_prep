/**
 * Q12: Fixed-Size Memory Pool Allocator
 *
 * PROBLEM:
 * Design a memory pool allocator that manages fixed-size blocks from a
 * pre-allocated buffer. This avoids heap fragmentation and provides
 * deterministic O(1) allocation/deallocation.
 *
 * REAL-WORLD USES:
 * - Network packet buffers
 * - Message queues
 * - Object pools for frequently created/destroyed objects
 * - Real-time systems requiring deterministic timing
 *
 * REQUIREMENTS:
 * - O(1) allocation and deallocation
 * - No external memory allocation (use pre-allocated buffer)
 * - No memory fragmentation (fixed block sizes)
 * - Track free blocks using a free list
 *
 * DESIGN:
 * - Maintain a singly-linked free list
 * - Each free block stores a pointer to the next free block
 * - Allocate: pop from free list head
 * - Deallocate: push to free list head
 *
 * INTERVIEW DISCUSSION:
 * - Why not use malloc/new? (Fragmentation, non-deterministic time)
 * - How to handle different object sizes? (Multiple pools or size classes)
 * - Thread safety considerations (lock-free free list)
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <set>

// ============ YOUR IMPLEMENTATION ============

/**
 * Fixed-size block memory pool.
 *
 * @tparam BLOCK_SIZE  Size of each block in bytes (minimum sizeof(void*))
 * @tparam NUM_BLOCKS  Number of blocks in the pool
 */
template<size_t BLOCK_SIZE, size_t NUM_BLOCKS>
class MemoryPool {
    static_assert(BLOCK_SIZE >= sizeof(void*),
                  "Block size must be at least sizeof(void*)");

private:
    // Pre-allocated storage
    alignas(alignof(std::max_align_t)) uint8_t storage[BLOCK_SIZE * NUM_BLOCKS];

    // TODO: Add free list head pointer
    // void* free_head;

    // TODO: Add tracking for statistics (optional)
    // size_t allocated_count;

public:
    MemoryPool() {
        // TODO: Initialize free list
        // Link all blocks together, with each block pointing to the next
        //
        // Example for 3 blocks:
        // [Block 0] -> [Block 1] -> [Block 2] -> nullptr
        //     ^
        //  free_head

    }

    /**
     * Allocate a block from the pool.
     *
     * @return Pointer to allocated block, or nullptr if pool is exhausted
     */
    void* allocate() {
        // TODO: Implement
        // 1. Check if free list is empty
        // 2. Pop head of free list
        // 3. Return the block

        return nullptr;  // Placeholder
    }

    /**
     * Return a block to the pool.
     *
     * @param ptr Pointer to block to deallocate (must be from this pool)
     */
    void deallocate(void* ptr) {
        // TODO: Implement
        // 1. Validate ptr is within our storage (optional but good)
        // 2. Push block onto head of free list

    }

    /**
     * Check if a pointer belongs to this pool.
     */
    bool owns(void* ptr) const {
        uint8_t* p = static_cast<uint8_t*>(ptr);
        return p >= storage && p < storage + sizeof(storage);
    }

    /**
     * Get number of free blocks available.
     */
    size_t free_count() const {
        // TODO: Implement (walk free list or track count)
        return 0;  // Placeholder
    }

    /**
     * Get number of allocated blocks.
     */
    size_t allocated_count() const {
        return NUM_BLOCKS - free_count();
    }

    /**
     * Get total capacity.
     */
    size_t capacity() const {
        return NUM_BLOCKS;
    }

    /**
     * Get block size.
     */
    size_t block_size() const {
        return BLOCK_SIZE;
    }

    /**
     * Reset pool to initial state (all blocks free).
     */
    void reset() {
        // TODO: Reinitialize free list
    }
};

/**
 * BONUS: Type-safe object pool.
 *
 * Wraps MemoryPool to provide typed allocation with constructor/destructor.
 */
template<typename T, size_t NUM_OBJECTS>
class ObjectPool {
private:
    // Blocks must hold at least a void* pointer for the free list, even if T is smaller.
    static constexpr size_t BLOCK_SZ = sizeof(T) >= sizeof(void*) ? sizeof(T) : sizeof(void*);
    MemoryPool<BLOCK_SZ, NUM_OBJECTS> pool;

public:
    /**
     * Allocate and construct an object.
     */
    template<typename... Args>
    T* create(Args&&... args) {
        void* mem = pool.allocate();
        if (!mem) return nullptr;

        // Placement new to construct in-place
        return new (mem) T(std::forward<Args>(args)...);
    }

    /**
     * Destroy and deallocate an object.
     */
    void destroy(T* obj) {
        if (!obj) return;

        // Call destructor explicitly
        obj->~T();

        // Return memory to pool
        pool.deallocate(obj);
    }

    size_t free_count() const { return pool.free_count(); }
    size_t capacity() const { return pool.capacity(); }
};

// ============ TEST FRAMEWORK ============

// Must be at file scope: C++ does not allow static data members in local structs.
struct TestObject {
    int value;
    static int construct_count;
    static int destruct_count;

    TestObject(int v) : value(v) { construct_count++; }
    ~TestObject() { destruct_count++; }
};
int TestObject::construct_count = 0;
int TestObject::destruct_count = 0;

bool test_basic_pool() {
    printf("--- Basic Memory Pool Tests ---\n");
    int passed = 0;
    int total = 0;

    MemoryPool<64, 10> pool;

    // Test initial state
    if (pool.free_count() == 10 && pool.allocated_count() == 0) {
        printf("PASS: Initial state (all free)\n");
        passed++;
    } else {
        printf("FAIL: Initial state (free=%zu, alloc=%zu)\n",
               pool.free_count(), pool.allocated_count());
    }
    total++;

    // Test single allocation
    void* p1 = pool.allocate();
    if (p1 != nullptr && pool.owns(p1) && pool.free_count() == 9) {
        printf("PASS: Single allocation\n");
        passed++;
    } else {
        printf("FAIL: Single allocation\n");
    }
    total++;

    // Test deallocation
    pool.deallocate(p1);
    if (pool.free_count() == 10) {
        printf("PASS: Deallocation\n");
        passed++;
    } else {
        printf("FAIL: Deallocation (free=%zu)\n", pool.free_count());
    }
    total++;

    // Test allocate all blocks
    std::vector<void*> ptrs;
    for (int i = 0; i < 10; i++) {
        void* p = pool.allocate();
        if (p == nullptr) {
            printf("FAIL: Allocation %d returned null\n", i);
            break;
        }
        ptrs.push_back(p);
    }
    if (ptrs.size() == 10 && pool.free_count() == 0) {
        printf("PASS: Allocate all blocks\n");
        passed++;
    } else {
        printf("FAIL: Allocate all blocks\n");
    }
    total++;

    // Test allocation when exhausted
    void* p_extra = pool.allocate();
    if (p_extra == nullptr) {
        printf("PASS: Returns null when exhausted\n");
        passed++;
    } else {
        printf("FAIL: Should return null when exhausted\n");
    }
    total++;

    // Test all pointers are unique
    std::set<void*> unique_ptrs(ptrs.begin(), ptrs.end());
    if (unique_ptrs.size() == ptrs.size()) {
        printf("PASS: All allocations are unique\n");
        passed++;
    } else {
        printf("FAIL: Duplicate pointers returned\n");
    }
    total++;

    // Deallocate all
    for (void* p : ptrs) {
        pool.deallocate(p);
    }
    if (pool.free_count() == 10) {
        printf("PASS: Deallocate all\n");
        passed++;
    } else {
        printf("FAIL: Deallocate all (free=%zu)\n", pool.free_count());
    }
    total++;

    // Test reuse after deallocation
    void* p_reuse = pool.allocate();
    bool found = false;
    for (void* p : ptrs) {
        if (p == p_reuse) { found = true; break; }
    }
    if (found) {
        printf("PASS: Blocks are reused after deallocation\n");
        passed++;
    } else {
        printf("FAIL: Block not reused\n");
    }
    total++;

    printf("Basic pool tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_object_pool() {
    printf("--- Object Pool Tests ---\n");
    int passed = 0;
    int total = 0;

    // Reset counters before this test
    TestObject::construct_count = 0;
    TestObject::destruct_count = 0;

    ObjectPool<TestObject, 5> pool;

    // Create objects
    TestObject* obj1 = pool.create(42);
    TestObject* obj2 = pool.create(100);

    if (obj1 && obj2 && obj1->value == 42 && obj2->value == 100) {
        printf("PASS: Object creation with constructor args\n");
        passed++;
    } else {
        printf("FAIL: Object creation\n");
    }
    total++;

    if (TestObject::construct_count == 2) {
        printf("PASS: Constructors called\n");
        passed++;
    } else {
        printf("FAIL: Constructor count = %d\n", TestObject::construct_count);
    }
    total++;

    // Destroy objects
    pool.destroy(obj1);
    pool.destroy(obj2);

    if (TestObject::destruct_count == 2) {
        printf("PASS: Destructors called\n");
        passed++;
    } else {
        printf("FAIL: Destructor count = %d\n", TestObject::destruct_count);
    }
    total++;

    if (pool.free_count() == 5) {
        printf("PASS: Objects returned to pool\n");
        passed++;
    } else {
        printf("FAIL: Pool state after destroy\n");
    }
    total++;

    printf("Object pool tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_stress() {
    printf("--- Stress Test ---\n");

    MemoryPool<32, 1000> pool;
    std::vector<void*> allocated;

    // Random allocate/deallocate pattern
    for (int i = 0; i < 10000; i++) {
        if (allocated.empty() || (rand() % 2 == 0 && pool.free_count() > 0)) {
            void* p = pool.allocate();
            if (p) allocated.push_back(p);
        } else {
            size_t idx = rand() % allocated.size();
            pool.deallocate(allocated[idx]);
            allocated.erase(allocated.begin() + idx);
        }
    }

    // Verify counts match
    size_t expected_allocated = allocated.size();
    size_t actual_allocated = pool.allocated_count();

    if (expected_allocated == actual_allocated) {
        printf("PASS: Stress test (allocated=%zu matches)\n", actual_allocated);
        return true;
    } else {
        printf("FAIL: Stress test (expected=%zu, actual=%zu)\n",
               expected_allocated, actual_allocated);
        return false;
    }
}

int main() {
    printf("Running Q12: Memory Pool Allocator\n");
    printf("==================================\n\n");

    bool all_passed = true;

    if (!test_basic_pool()) all_passed = false;
    // Note: test_object_pool has a compilation issue with local struct static members
    // Uncomment after fixing or move TestObject outside the function
    // if (!test_object_pool()) all_passed = false;
    if (!test_stress()) all_passed = false;

    printf("==================================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
