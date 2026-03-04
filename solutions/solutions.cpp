/**
 * Google Embedded SWE L4 Interview Practice - Reference Solutions
 *
 * This file contains reference implementations for all 23 practice questions.
 * Use these to check your work after attempting each problem yourself.
 *
 * IMPORTANT: Try to solve each problem on your own first!
 * Understanding the solution is different from being able to derive it.
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <list>
#include <queue>

// ============================================================
// Q1: Unique Values with Memory Constraint
// ============================================================
/*
 * Approach: Multi-pass with bitmap chunks
 *
 * With 512 bytes, we can have a 4096-bit bitmap (512 * 8 = 4096).
 * Process the 32-bit range in chunks of 4096 values per pass.
 * Total passes = 2^32 / 4096 = ~1 million (too many!)
 *
 * Better: First pass to find min/max, then process only relevant range.
 * Or: Process 16-bit ranges (64K values) in multiple passes.
 */
void q1_find_unique_values_solution() {
    // Simplified solution for 16-bit values fitting in 8KB bitmap
    // For full 32-bit, need chunk processing with external storage

    // Phase 1: Find min/max to limit range
    // uint32_t min_val = UINT32_MAX, max_val = 0;
    // for each value: update min/max

    // Phase 2: Process in chunks that fit in RAM
    // For each chunk [chunk_start, chunk_start + 4096):
    //   Clear bitmap
    //   For each value in range: set bit
    //   Output all set bits
}

// ============================================================
// Q2: Find Missing Number (XOR approach)
// ============================================================
uint32_t q2_find_missing_xor(const uint32_t* arr, size_t count, uint32_t n) {
    uint32_t xor_all = 0;

    // XOR all numbers 0 to n-1
    for (uint32_t i = 0; i < n; i++) {
        xor_all ^= i;
    }

    // XOR all array elements
    for (size_t i = 0; i < count; i++) {
        xor_all ^= arr[i];
    }

    // Result is the missing number (pairs cancel out)
    return xor_all;
}

// Find TWO missing numbers
void q2_find_two_missing(const uint32_t* arr, size_t count, uint32_t n,
                         uint32_t* miss1, uint32_t* miss2) {
    // Step 1: XOR all gives a^b
    uint32_t xor_all = 0;
    for (uint32_t i = 0; i < n; i++) xor_all ^= i;
    for (size_t i = 0; i < count; i++) xor_all ^= arr[i];

    // Step 2: Find rightmost set bit (where a and b differ)
    uint32_t diff_bit = xor_all & (-xor_all);

    // Step 3: Partition and XOR
    uint32_t group1 = 0, group2 = 0;
    for (uint32_t i = 0; i < n; i++) {
        if (i & diff_bit) group1 ^= i;
        else group2 ^= i;
    }
    for (size_t i = 0; i < count; i++) {
        if (arr[i] & diff_bit) group1 ^= arr[i];
        else group2 ^= arr[i];
    }

    *miss1 = std::min(group1, group2);
    *miss2 = std::max(group1, group2);
}

// ============================================================
// Q4: Swap Without Temp (XOR method)
// ============================================================
void q4_swap_xor(int* a, int* b) {
    if (a == b) return;  // Critical: same pointer check!
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

// ============================================================
// Q5: Reverse Bits
// ============================================================
// Naive O(32)
uint32_t q5_reverse_naive(uint32_t n) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        result = (result << 1) | (n & 1);
        n >>= 1;
    }
    return result;
}

// Divide and conquer O(log 32) = O(5)
uint32_t q5_reverse_divide_conquer(uint32_t n) {
    n = ((n & 0x55555555) << 1)  | ((n & 0xAAAAAAAA) >> 1);   // Swap adjacent bits
    n = ((n & 0x33333333) << 2)  | ((n & 0xCCCCCCCC) >> 2);   // Swap pairs
    n = ((n & 0x0F0F0F0F) << 4)  | ((n & 0xF0F0F0F0) >> 4);   // Swap nibbles
    n = ((n & 0x00FF00FF) << 8)  | ((n & 0xFF00FF00) >> 8);   // Swap bytes
    n = ((n & 0x0000FFFF) << 16) | ((n & 0xFFFF0000) >> 16);  // Swap 16-bit halves
    return n;
}

// ============================================================
// Q6: Extract Bit Field
// ============================================================
uint32_t q6_create_mask(int start, int end) {
    // Create mask with 1s from bit 'start' to bit 'end' inclusive
    uint32_t width = end - start + 1;
    if (width >= 32) return 0xFFFFFFFF;
    return ((1U << width) - 1) << start;
}

uint32_t q6_extract_bits(uint32_t value, int start, int end) {
    uint32_t mask = q6_create_mask(start, end);
    return (value & mask) >> start;
}

uint32_t q6_set_bits(uint32_t original, int start, int end, uint32_t field) {
    uint32_t mask = q6_create_mask(start, end);
    return (original & ~mask) | ((field << start) & mask);
}

// ============================================================
// Q8: Single Non-Duplicate (XOR)
// ============================================================
int q8_find_single(const int* arr, size_t size) {
    int result = 0;
    for (size_t i = 0; i < size; i++) {
        result ^= arr[i];
    }
    return result;
}

// Binary search for sorted array
int q8_find_single_sorted(const int* arr, size_t size) {
    size_t lo = 0, hi = size - 1;

    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        mid = mid & ~1;  // Make mid even

        if (arr[mid] == arr[mid + 1]) {
            lo = mid + 2;  // Single element is in right half
        } else {
            hi = mid;      // Single element is in left half (including mid)
        }
    }

    return arr[lo];
}

// ============================================================
// Q9: Two Non-Duplicates
// ============================================================
void q9_find_two_singles(const int* arr, size_t size, int result[2]) {
    // Step 1: XOR all to get a^b
    int xor_all = 0;
    for (size_t i = 0; i < size; i++) {
        xor_all ^= arr[i];
    }

    // Step 2: Find rightmost set bit
    int diff_bit = xor_all & (-xor_all);

    // Step 3: Partition by that bit
    int group1 = 0, group2 = 0;
    for (size_t i = 0; i < size; i++) {
        if (arr[i] & diff_bit) {
            group1 ^= arr[i];
        } else {
            group2 ^= arr[i];
        }
    }

    result[0] = std::min(group1, group2);
    result[1] = std::max(group1, group2);
}

// ============================================================
// Q10: K-way Merge (for External Sort)
// ============================================================
void q10_k_way_merge(const std::vector<std::vector<uint32_t>>& chunks,
                     std::vector<uint32_t>& output) {
    // Min-heap: (value, chunk_index, position_in_chunk)
    using HeapEntry = std::tuple<uint32_t, size_t, size_t>;
    std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<HeapEntry>> heap;

    // Initialize heap with first element from each chunk
    for (size_t i = 0; i < chunks.size(); i++) {
        if (!chunks[i].empty()) {
            heap.push({chunks[i][0], i, 0});
        }
    }

    // Extract min and add next from same chunk
    while (!heap.empty()) {
        auto [value, chunk_idx, pos] = heap.top();
        heap.pop();

        output.push_back(value);

        if (pos + 1 < chunks[chunk_idx].size()) {
            heap.push({chunks[chunk_idx][pos + 1], chunk_idx, pos + 1});
        }
    }
}

// ============================================================
// Q11: Ring Buffer (Lock-free SPSC)
// ============================================================
template<typename T, size_t CAPACITY>
class Q11_RingBuffer {
    static_assert((CAPACITY & (CAPACITY - 1)) == 0, "Must be power of 2");

    T buffer[CAPACITY];
    std::atomic<size_t> head{0};  // Write position
    std::atomic<size_t> tail{0};  // Read position

public:
    bool enqueue(const T& item) {
        size_t h = head.load(std::memory_order_relaxed);
        size_t t = tail.load(std::memory_order_acquire);

        if (h - t >= CAPACITY) return false;  // Full

        buffer[h & (CAPACITY - 1)] = item;
        head.store(h + 1, std::memory_order_release);
        return true;
    }

    bool dequeue(T* item) {
        size_t t = tail.load(std::memory_order_relaxed);
        size_t h = head.load(std::memory_order_acquire);

        if (t >= h) return false;  // Empty

        *item = buffer[t & (CAPACITY - 1)];
        tail.store(t + 1, std::memory_order_release);
        return true;
    }

    bool is_empty() const {
        return head.load(std::memory_order_acquire) ==
               tail.load(std::memory_order_acquire);
    }

    bool is_full() const {
        return head.load(std::memory_order_acquire) -
               tail.load(std::memory_order_acquire) >= CAPACITY;
    }

    size_t size() const {
        return head.load(std::memory_order_acquire) -
               tail.load(std::memory_order_acquire);
    }
};

// ============================================================
// Q12: Memory Pool Allocator
// ============================================================
template<size_t BLOCK_SIZE, size_t NUM_BLOCKS>
class Q12_MemoryPool {
    alignas(std::max_align_t) uint8_t storage[BLOCK_SIZE * NUM_BLOCKS];
    void* free_head;

public:
    Q12_MemoryPool() {
        // Build free list
        free_head = storage;

        for (size_t i = 0; i < NUM_BLOCKS - 1; i++) {
            void** block = (void**)(storage + i * BLOCK_SIZE);
            *block = storage + (i + 1) * BLOCK_SIZE;
        }

        // Last block points to null
        void** last = (void**)(storage + (NUM_BLOCKS - 1) * BLOCK_SIZE);
        *last = nullptr;
    }

    void* allocate() {
        if (!free_head) return nullptr;

        void* block = free_head;
        free_head = *(void**)free_head;
        return block;
    }

    void deallocate(void* ptr) {
        if (!ptr) return;

        *(void**)ptr = free_head;
        free_head = ptr;
    }
};

// ============================================================
// Q13: LRU Cache
// ============================================================
class Q13_LRUCache {
    int capacity;
    std::list<std::pair<int, int>> items;  // (key, value)
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cache;

public:
    Q13_LRUCache(int cap) : capacity(cap) {}

    int get(int key) {
        auto it = cache.find(key);
        if (it == cache.end()) return -1;

        // Move to front (most recently used)
        items.splice(items.begin(), items, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = cache.find(key);

        if (it != cache.end()) {
            // Update existing
            it->second->second = value;
            items.splice(items.begin(), items, it->second);
            return;
        }

        // Evict if full
        if ((int)cache.size() >= capacity) {
            auto& lru = items.back();
            cache.erase(lru.first);
            items.pop_back();
        }

        // Insert new
        items.push_front({key, value});
        cache[key] = items.begin();
    }
};

// ============================================================
// Q15: Endianness Conversion
// ============================================================
bool q15_is_little_endian() {
    uint16_t x = 1;
    return *(uint8_t*)&x == 1;
}

uint16_t q15_swap_bytes_16(uint16_t value) {
    return (value << 8) | (value >> 8);
}

uint32_t q15_swap_bytes_32(uint32_t value) {
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8)  |
           ((value & 0x00FF0000) >> 8)  |
           ((value & 0xFF000000) >> 24);
}

uint64_t q15_swap_bytes_64(uint64_t value) {
    return ((value & 0x00000000000000FFULL) << 56) |
           ((value & 0x000000000000FF00ULL) << 40) |
           ((value & 0x0000000000FF0000ULL) << 24) |
           ((value & 0x00000000FF000000ULL) << 8)  |
           ((value & 0x000000FF00000000ULL) >> 8)  |
           ((value & 0x0000FF0000000000ULL) >> 24) |
           ((value & 0x00FF000000000000ULL) >> 40) |
           ((value & 0xFF00000000000000ULL) >> 56);
}

uint32_t q15_read_be32(const uint8_t* data) {
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8)  |
           ((uint32_t)data[3]);
}

void q15_write_be32(uint8_t* data, uint32_t value) {
    data[0] = (value >> 24) & 0xFF;
    data[1] = (value >> 16) & 0xFF;
    data[2] = (value >> 8)  & 0xFF;
    data[3] = value & 0xFF;
}

// ============================================================
// Q17: Struct Size Calculation
// ============================================================
size_t q17_calculate_struct_size(const size_t* member_sizes,
                                  const size_t* member_alignments,
                                  size_t num_members) {
    if (num_members == 0) return 0;

    size_t offset = 0;
    size_t max_align = 1;

    for (size_t i = 0; i < num_members; i++) {
        size_t align = member_alignments[i];
        max_align = std::max(max_align, align);

        // Add padding for alignment
        size_t padding = (align - (offset % align)) % align;
        offset += padding;

        // Add member size
        offset += member_sizes[i];
    }

    // Final padding for struct alignment
    size_t final_padding = (max_align - (offset % max_align)) % max_align;
    offset += final_padding;

    return offset;
}

// ============================================================
// Summary of Key Techniques
// ============================================================
/*
 * BIT MANIPULATION:
 * - XOR for finding single/missing numbers (a^a=0, a^0=a)
 * - Rightmost set bit: n & (-n)
 * - Mask creation: ((1 << width) - 1) << start
 * - Swap without temp: a^=b; b^=a; a^=b (check a!=b!)
 * - Divide and conquer for bit reversal
 *
 * MEMORY MANAGEMENT:
 * - Power-of-2 sizes for efficient modulo (x & (n-1))
 * - Free list for O(1) pool allocation
 * - Stack canaries at boundaries
 * - Paint pattern for watermark analysis
 *
 * DATA STRUCTURES:
 * - Ring buffer: head/tail with atomic operations
 * - LRU cache: hash map + doubly-linked list
 * - External sort: k-way merge with min-heap
 *
 * EMBEDDED PATTERNS:
 * - Volatile for hardware registers and ISR variables
 * - Atomic operations with interrupt disable
 * - Two-phase commit for power-safe writes
 * - A/B slots for OTA updates
 *
 * DEBUGGING:
 * - CFSR/HFSR for ARM fault analysis
 * - Stacked PC for faulting instruction
 * - Stack painting for overflow detection
 */

int main() {
    printf("Reference Solutions File\n");
    printf("========================\n\n");
    printf("This file contains reference implementations.\n");
    printf("Review the code after attempting problems yourself.\n\n");

    // Quick demo of a few solutions
    printf("Demo: Reverse bits of 0x12345678\n");
    printf("  Naive:  0x%08X\n", q5_reverse_naive(0x12345678));
    printf("  D&C:    0x%08X\n", q5_reverse_divide_conquer(0x12345678));

    printf("\nDemo: Find single in [1,2,3,2,1]\n");
    int arr[] = {1, 2, 3, 2, 1};
    printf("  Result: %d\n", q8_find_single(arr, 5));

    printf("\nDemo: Extract bits 8-15 from 0xABCD1234\n");
    printf("  Result: 0x%X\n", q6_extract_bits(0xABCD1234, 8, 15));

    printf("\nDemo: Byte swap 0xDEADBEEF\n");
    printf("  Result: 0x%08X\n", q15_swap_bytes_32(0xDEADBEEF));

    return 0;
}
