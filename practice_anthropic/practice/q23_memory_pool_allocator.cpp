/*
 * Q23 — Memory Pool Allocator [Medium]
 *
 * Topic: Pure Programming — Systems
 *
 * Problem:
 *   Implement a simple memory pool allocator that manages a fixed-size buffer.
 *   This is a common systems programming pattern used in performance-critical
 *   code where malloc/free overhead is unacceptable.
 *
 *   Implement:
 *   1. MemoryPool(total_size) — initialize with a contiguous block
 *   2. alloc(size) — allocate a block, return offset (or -1 if no space).
 *      Use first-fit strategy.
 *   3. free(offset) — free a previously allocated block
 *   4. coalesce() — merge adjacent free blocks
 *   5. Track: fragmentation ratio, largest free block, number of allocations
 *
 * Constraints:
 *   - All sizes and offsets are in bytes (ints for simulation)
 *   - First-fit allocation: scan free list, pick first block that fits
 *   - Free list is a list of (offset, size) pairs
 *   - Allocated blocks tracked separately for bookkeeping
 *   - alloc(0) should return -1 (invalid)
 *
 * Key Concepts:
 *   - Memory allocation strategies (first-fit, best-fit, worst-fit)
 *   - External fragmentation
 *   - Free list coalescing
 *   - Pool allocators for real-time / embedded systems
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

struct Block {
    int offset;
    int size;
};

// ============ YOUR IMPLEMENTATION ============

/*
 * MemoryPool: Fixed-size memory pool with first-fit allocation.
 *
 * Internal state:
 *   - free_list: sorted list of (offset, size) pairs for free regions
 *   - alloc_map: list of (offset, size) pairs for allocated regions
 *   - total_size: total pool size
 *
 * Initially, the entire pool is one free block: (0, total_size).
 */
struct MemoryPool {
    int total_size;
    vector<Block> free_list;    // sorted by offset
    vector<Block> alloc_map;    // allocated blocks

    MemoryPool(int total_size) : total_size(total_size) {
        // TODO: Initialize free list with one block spanning the entire pool
    }

    /*
     * alloc: Allocate 'size' bytes using first-fit strategy.
     *
     * Scan the free list for the first block with block.size >= size.
     * If found:
     *   - Record the allocation in alloc_map
     *   - If the free block is exactly 'size', remove it from free list
     *   - If larger, shrink the free block (advance offset, reduce size)
     *   - Return the offset of the allocated block
     * If not found, return -1.
     * If size <= 0, return -1.
     */
    int alloc(int size) {
        // TODO: Implement first-fit allocation
        return -1;
    }

    /*
     * free: Free the block at the given offset.
     *
     * Look up 'offset' in alloc_map to find the block size.
     * Remove from alloc_map and add back to free_list (keep sorted by offset).
     * Does nothing if offset is not found in alloc_map.
     */
    void free(int offset) {
        // TODO: Implement free
    }

    /*
     * coalesce: Merge adjacent free blocks.
     *
     * Scan the sorted free list. If free_list[i].offset + free_list[i].size
     * equals free_list[i+1].offset, merge them into one block.
     * Repeat until no more merges are possible.
     */
    void coalesce() {
        // TODO: Implement coalescing of adjacent free blocks
    }

    /*
     * fragmentation_ratio: External fragmentation metric.
     *
     * fragmentation = 1 - (largest_free_block / total_free_space)
     * Returns 0.0 if no free space or only one free block.
     */
    double fragmentation_ratio() {
        // TODO: Compute fragmentation
        return 0.0;
    }

    /*
     * largest_free_block: Size of the largest contiguous free region.
     */
    int largest_free_block() {
        // TODO: Return largest free block size, or 0 if none
        return 0;
    }

    /*
     * num_allocations: Current number of active allocations.
     */
    int num_allocations() {
        return (int)alloc_map.size();
    }
};

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q23: Memory Pool Allocator ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Basic alloc returns valid offset
    {
        total++;
        MemoryPool pool(1024);
        int off = pool.alloc(256);
        bool ok = (off >= 0 && off < 1024);
        printf("%s [Basic alloc returns valid offset: %d]\n", ok ? "PASS" : "FAIL", off);
        if (ok) passed++;
    }

    // Test 2: Sequential allocs don't overlap
    {
        total++;
        MemoryPool pool(1024);
        int a = pool.alloc(256);
        int b = pool.alloc(256);
        int c = pool.alloc(256);
        // All should be valid and non-overlapping
        bool valid = (a >= 0 && b >= 0 && c >= 0);
        bool no_overlap = (b >= a + 256) && (c >= b + 256);
        bool ok = valid && no_overlap;
        printf("%s [Sequential allocs don't overlap: a=%d b=%d c=%d]\n",
               ok ? "PASS" : "FAIL", a, b, c);
        if (ok) passed++;
    }

    // Test 3: Free makes space available for reallocation
    {
        total++;
        MemoryPool pool(512);
        int a = pool.alloc(256);
        int b = pool.alloc(256);
        // Pool is full
        int fail1 = pool.alloc(1);
        pool.free(a);
        int c = pool.alloc(128);  // Should succeed in freed space
        bool ok = (fail1 == -1) && (c >= 0);
        printf("%s [Free makes space available: fail=%d, realloc=%d]\n",
               ok ? "PASS" : "FAIL", fail1, c);
        if (ok) passed++;
    }

    // Test 4: Coalesce merges adjacent free blocks
    {
        total++;
        MemoryPool pool(1024);
        int a = pool.alloc(256);
        int b = pool.alloc(256);
        int c = pool.alloc(256);
        pool.free(a);
        pool.free(b);
        // Before coalesce: two separate free blocks of 256
        int before = pool.largest_free_block();
        pool.coalesce();
        // After coalesce: one free block of 512
        int after = pool.largest_free_block();
        bool ok = (before == 256) && (after == 512);
        printf("%s [Coalesce merges: before=%d, after=%d]\n",
               ok ? "PASS" : "FAIL", before, after);
        if (ok) passed++;
    }

    // Test 5: Allocation fails when pool is full
    {
        total++;
        MemoryPool pool(512);
        pool.alloc(512);
        int fail = pool.alloc(1);
        bool ok = (fail == -1);
        printf("%s [Alloc fails when full: %d]\n", ok ? "PASS" : "FAIL", fail);
        if (ok) passed++;
    }

    // Test 6: Fragmentation after many alloc/free cycles
    {
        total++;
        MemoryPool pool(1024);
        // Allocate 4 blocks of 256
        int a = pool.alloc(256);
        int b = pool.alloc(256);
        int c = pool.alloc(256);
        int d = pool.alloc(256);
        // Free alternating blocks -> creates fragmentation
        pool.free(a);
        pool.free(c);
        double frag = pool.fragmentation_ratio();
        // Two free blocks of 256, total free = 512, largest = 256
        // fragmentation = 1 - 256/512 = 0.5
        bool ok = fabs(frag - 0.5) < 0.01;
        printf("%s [Fragmentation ratio: %.3f, expected ~0.5]\n",
               ok ? "PASS" : "FAIL", frag);
        if (ok) passed++;
    }

    // Test 7: alloc(0) edge case
    {
        total++;
        MemoryPool pool(1024);
        int off = pool.alloc(0);
        bool ok = (off == -1);
        printf("%s [alloc(0) returns -1: %d]\n", ok ? "PASS" : "FAIL", off);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
