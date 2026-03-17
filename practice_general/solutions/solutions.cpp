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
#include <cmath>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <list>
#include <queue>
#include <atomic>

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

// Q1 uses function pointers so it's self-contained and testable without the
// practice file's simulated hardware. Callers inject the I/O operations.
void q1_find_unique_values_solution(
    uint64_t (*get_count)(),
    void (*read_val)(uint64_t index, uint32_t* out),
    void (*write_output)(uint32_t value))
{
    // With 512 bytes = 4096 bits, we can track 4096 values per pass
    constexpr size_t BITMAP_BYTES = 512;
    constexpr size_t BITMAP_BITS = BITMAP_BYTES * 8;  // 4096
    uint8_t bitmap[BITMAP_BYTES];

    uint64_t count = get_count();
    if (count == 0) return;

    // Phase 1: Find min and max values to limit the range we need to process
    uint32_t min_val = UINT32_MAX;
    uint32_t max_val = 0;
    uint32_t value;

    for (uint64_t i = 0; i < count; i++) {
        read_val(i, &value);
        if (value < min_val) min_val = value;
        if (value > max_val) max_val = value;
    }

    // Phase 2: Process the range [min_val, max_val] in chunks of BITMAP_BITS
    // For each chunk, use bitmap to track which values exist
    uint32_t chunk_start = min_val;

    while (chunk_start <= max_val) {
        // Calculate chunk end (exclusive), handle overflow at UINT32_MAX
        uint32_t chunk_end;
        if (max_val - chunk_start < BITMAP_BITS) {
            chunk_end = max_val + 1;
        } else {
            chunk_end = chunk_start + BITMAP_BITS;
        }

        // Clear bitmap
        memset(bitmap, 0, BITMAP_BYTES);

        // Scan all values, mark those that fall in this chunk
        for (uint64_t i = 0; i < count; i++) {
            read_val(i, &value);
            if (value >= chunk_start && value < chunk_end) {
                uint32_t bit_index = value - chunk_start;
                bitmap[bit_index / 8] |= (1 << (bit_index % 8));
            }
        }

        // Output all set bits in sorted order
        for (uint32_t bit = 0; bit < (chunk_end - chunk_start); bit++) {
            if (bitmap[bit / 8] & (1 << (bit % 8))) {
                write_output(chunk_start + bit);
            }
        }

        // Move to next chunk; stop if chunk_end wrapped or exceeded max_val
        if (chunk_end == 0 || chunk_end > max_val) break;
        chunk_start = chunk_end;
    }
}

// ============================================================
// Q2: Find Missing Number (XOR approach)
// ============================================================
//
// The stream interface matches the practice file:
//   get_count()       -> number of elements
//   read_next(&val)   -> read next value (single-direction)
//   reset()           -> rewind stream to beginning
//
// Single missing: one pass is enough.
// Two missing:    two passes are required (XOR partition needs the diff_bit
//                 from pass 1 before it can partition in pass 2).

uint32_t q2_find_missing_xor(
    bool (*read_next)(uint32_t*),
    uint32_t n)
{
    uint32_t xor_all = 0;

    // XOR all numbers 0 to n-1
    for (uint32_t i = 0; i < n; i++) {
        xor_all ^= i;
    }

    // XOR all stream values — each present number cancels with its pair,
    // leaving only the missing number.
    uint32_t val;
    while (read_next(&val)) {
        xor_all ^= val;
    }

    return xor_all;
}

// Find TWO missing numbers using the stream interface.
// Requires two passes: call reset() between them.
void q2_find_two_missing(
    bool (*read_next)(uint32_t*),
    void (*reset)(),
    uint32_t n,
    uint32_t* miss1, uint32_t* miss2)
{
    // Pass 1: XOR everything to get a ^ b.
    uint32_t xor_all = 0;
    for (uint32_t i = 0; i < n; i++) xor_all ^= i;
    uint32_t val;
    while (read_next(&val)) xor_all ^= val;

    // Find a bit where a and b differ — use it to partition.
    // The two missing numbers must differ at this bit, so they fall into
    // separate groups and can be recovered independently.
    uint32_t diff_bit = xor_all & (uint32_t)(-(int32_t)xor_all);

    // Pass 2: rewind and partition stream values by diff_bit.
    // Also partition the full range 0..n-1 the same way.
    // Each group's XOR cancels all present numbers, leaving the missing one.
    uint32_t group1 = 0, group2 = 0;

    for (uint32_t i = 0; i < n; i++) {
        if (i & diff_bit) group1 ^= i;
        else              group2 ^= i;
    }

    reset();  // <-- rewind stream for second pass
    while (read_next(&val)) {
        if (val & diff_bit) group1 ^= val;
        else                group2 ^= val;
    }

    *miss1 = std::min(group1, group2);
    *miss2 = std::max(group1, group2);
}

// ============================================================
// Q3: Count Distinct Values (Approximate)
// ============================================================
/*
 * Linear Counting Algorithm:
 *
 * Uses a bitmap smaller than the full value range. Hash each value
 * to a bit position. At the end, estimate cardinality based on the
 * number of empty (unset) bits.
 *
 * Formula: n = -m * ln(V/m)
 * where m = bitmap size in bits, V = number of zero bits
 *
 * This works because collisions increase predictably with cardinality.
 */

// Simple hash function for 16-bit values
uint32_t q3_hash(uint16_t value, uint32_t seed) {
    // FNV-1a inspired hash
    uint32_t hash = 2166136261u ^ seed;
    hash ^= value & 0xFF;
    hash *= 16777619u;
    hash ^= (value >> 8) & 0xFF;
    hash *= 16777619u;
    return hash;
}

class Q3_LinearCounter {
    // With 1KB = 8192 bits, can estimate up to ~40K distinct values
    static const size_t BITMAP_BYTES = 1024;
    static const size_t BITMAP_BITS = BITMAP_BYTES * 8;
    uint8_t bitmap[BITMAP_BYTES];

public:
    Q3_LinearCounter() {
        reset();
    }

    void reset() {
        memset(bitmap, 0, BITMAP_BYTES);
    }

    // Add a value to the counter
    void add(uint16_t value) {
        // Hash value to bit position
        uint32_t hash = q3_hash(value, 0);
        uint32_t bit_pos = hash % BITMAP_BITS;

        // Set the bit
        bitmap[bit_pos / 8] |= (1 << (bit_pos % 8));
    }

    // Estimate the number of distinct values
    uint32_t estimate() const {
        // Count zero bits
        size_t zero_count = 0;
        for (size_t i = 0; i < BITMAP_BYTES; i++) {
            uint8_t byte = bitmap[i];
            // Count zeros in this byte
            for (int j = 0; j < 8; j++) {
                if (!(byte & (1 << j))) {
                    zero_count++;
                }
            }
        }

        // If all bits are set, return bitmap size as lower bound
        if (zero_count == 0) {
            return BITMAP_BITS;
        }

        // Linear counting formula: n = -m * ln(V/m)
        // where m = total bits, V = zero bits
        double m = (double)BITMAP_BITS;
        double V = (double)zero_count;
        double estimate = -m * log(V / m);

        return (uint32_t)(estimate + 0.5);  // Round to nearest
    }
};

// ============================================================
// Q4: Swap Without Temp (XOR method)
// ============================================================
void q4_swap_xor(int* a, int* b) {
    if (a == b) return;  // Critical: same pointer check!
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

// BONUS: 64-bit XOR swap - identical logic, different type
void q4_swap_xor_64(uint64_t* a, uint64_t* b) {
    if (a == b) return;
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
// Q7: Atomic Bit Operations
// ============================================================
/*
 * On bare-metal embedded systems without OS support for atomics,
 * we achieve atomicity by disabling interrupts during RMW operations.
 *
 * This prevents ISRs from interrupting between read-modify-write steps,
 * which could cause lost updates.
 *
 * IMPORTANT: Keep critical sections short to minimize interrupt latency!
 */

// Simulated interrupt control (in real code, use platform-specific intrinsics)
// ARM Cortex-M: __disable_irq() / __enable_irq()
// AVR: cli() / sei()
namespace Q7_Atomics {

// In real embedded code, these would be:
// ARM: uint32_t primask = __get_PRIMASK(); __disable_irq();
// AVR: uint8_t sreg = SREG; cli();
static volatile bool g_interrupts_enabled = true;

inline bool disable_interrupts() {
    bool prev = g_interrupts_enabled;
    g_interrupts_enabled = false;
    return prev;
}

inline void restore_interrupts(bool prev_state) {
    g_interrupts_enabled = prev_state;
}

// Atomic set bit operation
void atomic_set_bit(volatile uint32_t* reg, int bit) {
    // Save interrupt state and disable
    bool prev = disable_interrupts();

    // Critical section: read-modify-write
    *reg |= (1U << bit);

    // Restore interrupt state
    restore_interrupts(prev);
}

// Atomic clear bit operation
void atomic_clear_bit(volatile uint32_t* reg, int bit) {
    bool prev = disable_interrupts();
    *reg &= ~(1U << bit);
    restore_interrupts(prev);
}

// Atomic toggle bit operation
void atomic_toggle_bit(volatile uint32_t* reg, int bit) {
    bool prev = disable_interrupts();
    *reg ^= (1U << bit);
    restore_interrupts(prev);
}

// Atomic set multiple bits using mask
void atomic_set_mask(volatile uint32_t* reg, uint32_t mask) {
    bool prev = disable_interrupts();
    *reg |= mask;
    restore_interrupts(prev);
}

// Atomic clear multiple bits using mask
void atomic_clear_mask(volatile uint32_t* reg, uint32_t mask) {
    bool prev = disable_interrupts();
    *reg &= ~mask;
    restore_interrupts(prev);
}

// Atomic write to a bit field
void atomic_write_field(volatile uint32_t* reg, int start, int end, uint32_t value) {
    // Create mask for the field
    uint32_t width = end - start + 1;
    uint32_t mask = ((1U << width) - 1) << start;

    bool prev = disable_interrupts();

    // Clear field, then set new value
    uint32_t temp = *reg;
    temp &= ~mask;                    // Clear the field
    temp |= (value << start) & mask;  // Set new value
    *reg = temp;

    restore_interrupts(prev);
}

} // namespace Q7_Atomics

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


// Find THREE elements that appear only once (all others appear twice).
// Uses bit-by-bit partitioning to isolate singles.
//
// Approach:
// 1. XOR all elements → a^b^c
// 2. For each set bit of xor_all, XOR elements with that bit set.
//    If result != xor_all, we isolated one single (exactly 1 of 3 has that bit).
// 3. Edge case: if all set bits of xor_all are shared by all 3 singles,
//    use zero bits of xor_all to find a pair XOR, then derive the third.
// 4. Once one single is known, reduce to the two-singles problem.
//
// Time: O(n), Space: O(1)
void q9_find_three_singles(const int* arr, size_t size, int result[3]) {
    // Step 1: XOR all elements to get a^b^c
    int xor_all = 0;
    for (size_t i = 0; i < size; i++) xor_all ^= arr[i];

    // Step 2: Find one of the three singles
    int first = 0;
    bool found = false;

    // Try bits where xor_all has a 1:
    // If exactly 1 of {a,b,c} has this bit, group XOR = that single.
    // If all 3 have it, group XOR = xor_all.
    for (int bit = 0; bit < 32 && !found; bit++) {
        if (!(xor_all & (1 << bit))) continue;
        int group_xor = 0;
        for (size_t i = 0; i < size; i++) {
            if (arr[i] & (1 << bit)) group_xor ^= arr[i];
        }
        if (group_xor != xor_all) {
            first = group_xor;
            found = true;
        }
    }

    if (!found) {
        // All set bits of xor_all are shared by all 3 singles.
        // Use bits where xor_all=0: exactly 0 or 2 singles have each bit.
        // If 2 have it, group XOR = their XOR. Third = xor_all ^ pair_xor.
        for (int bit = 0; bit < 32 && !found; bit++) {
            if (xor_all & (1 << bit)) continue;
            int group_xor = 0;
            for (size_t i = 0; i < size; i++) {
                if (arr[i] & (1 << bit)) group_xor ^= arr[i];
            }
            if (group_xor != 0) {
                first = xor_all ^ group_xor;
                found = true;
            }
        }
    }

    // Step 3: Find the other two using the standard two-singles technique
    int xor_remaining = xor_all ^ first;  // = b ^ c
    int diff_bit = xor_remaining & (-xor_remaining);

    int group1 = 0, group2 = 0;
    for (size_t i = 0; i < size; i++) {
        if (arr[i] & diff_bit) group1 ^= arr[i];
        else                   group2 ^= arr[i];
    }

    // Remove first's contribution from whichever group it fell into
    if (first & diff_bit) group1 ^= first;
    else                  group2 ^= first;

    result[0] = first;
    result[1] = group1;
    result[2] = group2;
    std::sort(result, result + 3);
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

// BONUS: External sort with actual file I/O
// Splits input file into sorted chunks that fit in RAM, writes each to a
// temp file, then performs a k-way merge of all temp files into the output.
#include <fstream>
void q10_external_sort_with_files(const char* input_file, const char* output_file,
                                   size_t ram_limit) {
    size_t chunk_capacity = ram_limit / sizeof(uint32_t);
    if (chunk_capacity == 0) return;

    std::vector<uint32_t> buffer(chunk_capacity);
    std::vector<std::string> temp_files;

    // Phase 1: Read input in chunks, sort each, write to temp file
    std::ifstream input(input_file, std::ios::binary);
    if (!input) return;

    while (input) {
        size_t count = 0;
        while (count < chunk_capacity) {
            uint32_t val;
            if (!input.read(reinterpret_cast<char*>(&val), sizeof(val))) break;
            buffer[count++] = val;
        }
        if (count == 0) break;

        std::sort(buffer.begin(), buffer.begin() + count);

        char tmp_name[64];
        snprintf(tmp_name, sizeof(tmp_name), "/tmp/extsort_%zu.tmp", temp_files.size());
        std::ofstream tmp(tmp_name, std::ios::binary);
        tmp.write(reinterpret_cast<const char*>(buffer.data()), count * sizeof(uint32_t));
        temp_files.push_back(tmp_name);
    }
    input.close();

    // Phase 2: K-way merge of temp files using min-heap
    struct MergeEntry {
        uint32_t value;
        size_t file_idx;
        bool operator>(const MergeEntry& o) const { return value > o.value; }
    };
    std::priority_queue<MergeEntry, std::vector<MergeEntry>, std::greater<MergeEntry>> heap;

    std::vector<std::ifstream> readers(temp_files.size());
    for (size_t i = 0; i < temp_files.size(); i++) {
        readers[i].open(temp_files[i], std::ios::binary);
        uint32_t val;
        if (readers[i].read(reinterpret_cast<char*>(&val), sizeof(val))) {
            heap.push({val, i});
        }
    }

    std::ofstream output_f(output_file, std::ios::binary);
    while (!heap.empty()) {
        auto [val, idx] = heap.top();
        heap.pop();
        output_f.write(reinterpret_cast<const char*>(&val), sizeof(val));

        uint32_t next_val;
        if (readers[idx].read(reinterpret_cast<char*>(&next_val), sizeof(next_val))) {
            heap.push({next_val, idx});
        }
    }

    // Cleanup temp files
    for (auto& f : temp_files) std::remove(f.c_str());
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

// BONUS: Overwriting ring buffer for logging systems.
// When full, enqueue overwrites the oldest entry instead of failing.
template<typename T, size_t CAPACITY>
class Q11_OverwritingRingBuffer {
    static_assert((CAPACITY & (CAPACITY - 1)) == 0, "Must be power of 2");

    T buffer[CAPACITY];
    size_t head;  // Next write position
    size_t count; // Number of valid elements

public:
    Q11_OverwritingRingBuffer() : head(0), count(0) {}

    // Always succeeds. Overwrites oldest entry when full.
    void enqueue(const T& item) {
        buffer[head & (CAPACITY - 1)] = item;
        head++;
        if (count < CAPACITY) {
            count++;
        }
        // If count was already CAPACITY, oldest is implicitly overwritten
        // because tail effectively advances.
    }

    bool dequeue(T* item) {
        if (count == 0) return false;
        // Oldest element is at (head - count)
        size_t tail = head - count;
        *item = buffer[tail & (CAPACITY - 1)];
        count--;
        return true;
    }

    size_t size() const { return count; }
    bool is_empty() const { return count == 0; }
    bool is_full() const { return count >= CAPACITY; }
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

// BONUS: Type-safe object pool wrapping MemoryPool.
// Uses placement new/explicit destructor for proper construction/destruction.
template<typename T, size_t NUM_OBJECTS>
class Q12_ObjectPool {
    // Blocks must be large enough for both T and the free-list pointer
    static constexpr size_t BLOCK_SZ = sizeof(T) >= sizeof(void*) ? sizeof(T) : sizeof(void*);
    Q12_MemoryPool<BLOCK_SZ, NUM_OBJECTS> pool;

public:
    // Allocate and construct with forwarded arguments
    template<typename... Args>
    T* create(Args&&... args) {
        void* mem = pool.allocate();
        if (!mem) return nullptr;
        return new (mem) T(std::forward<Args>(args)...);
    }

    // Destroy object and return memory to pool
    void destroy(T* obj) {
        if (!obj) return;
        obj->~T();
        pool.deallocate(obj);
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

// BONUS: Fixed-size LRU Cache for embedded systems.
// No dynamic allocation. Uses array-based doubly-linked list with open-addressing hash.
template<int CAPACITY>
class Q13_FixedLRUCache {
    static_assert(CAPACITY > 0, "Capacity must be positive");

    struct Node {
        int key;
        int value;
        int prev;  // Index (-1 = none)
        int next;  // Index (-1 = none)
        bool used;
    };

    Node nodes[CAPACITY];
    int head;   // MRU
    int tail;   // LRU
    int count;

    // Open-addressing hash table (2x capacity for low collision rate)
    static const int HASH_SIZE = CAPACITY * 2;
    int hash_table[HASH_SIZE];

    int hash_fn(int key) const {
        return ((key % HASH_SIZE) + HASH_SIZE) % HASH_SIZE;
    }

    // Linear probe to find key or empty slot
    int hash_find(int key) const {
        int h = hash_fn(key);
        for (int i = 0; i < HASH_SIZE; i++) {
            int idx = (h + i) % HASH_SIZE;
            if (hash_table[idx] == -1) return -1;
            if (nodes[hash_table[idx]].key == key && nodes[hash_table[idx]].used)
                return hash_table[idx];
        }
        return -1;
    }

    void hash_insert(int key, int node_idx) {
        int h = hash_fn(key);
        for (int i = 0; i < HASH_SIZE; i++) {
            int idx = (h + i) % HASH_SIZE;
            if (hash_table[idx] == -1) {
                hash_table[idx] = node_idx;
                return;
            }
        }
    }

    void hash_remove(int key) {
        int h = hash_fn(key);
        for (int i = 0; i < HASH_SIZE; i++) {
            int idx = (h + i) % HASH_SIZE;
            if (hash_table[idx] == -1) return;
            if (nodes[hash_table[idx]].key == key) {
                hash_table[idx] = -1;
                // Rehash subsequent entries to fill the gap
                int j = 1;
                while (true) {
                    int next_idx = (idx + j) % HASH_SIZE;
                    if (hash_table[next_idx] == -1) break;
                    int ni = hash_table[next_idx];
                    hash_table[next_idx] = -1;
                    hash_insert(nodes[ni].key, ni);
                    j++;
                }
                return;
            }
        }
    }

    // Move node to head of LRU list
    void move_to_head(int idx) {
        if (idx == head) return;
        // Unlink
        if (nodes[idx].prev != -1) nodes[nodes[idx].prev].next = nodes[idx].next;
        if (nodes[idx].next != -1) nodes[nodes[idx].next].prev = nodes[idx].prev;
        if (idx == tail) tail = nodes[idx].prev;
        // Link at head
        nodes[idx].prev = -1;
        nodes[idx].next = head;
        if (head != -1) nodes[head].prev = idx;
        head = idx;
        if (tail == -1) tail = idx;
    }

    int alloc_node() {
        for (int i = 0; i < CAPACITY; i++) {
            if (!nodes[i].used) return i;
        }
        return -1;
    }

public:
    Q13_FixedLRUCache() : head(-1), tail(-1), count(0) {
        for (int i = 0; i < CAPACITY; i++) nodes[i].used = false;
        for (int i = 0; i < HASH_SIZE; i++) hash_table[i] = -1;
    }

    int get(int key) {
        int idx = hash_find(key);
        if (idx == -1) return -1;
        move_to_head(idx);
        return nodes[idx].value;
    }

    void put(int key, int value) {
        int idx = hash_find(key);
        if (idx != -1) {
            nodes[idx].value = value;
            move_to_head(idx);
            return;
        }

        // Evict LRU if full
        if (count >= CAPACITY) {
            int evict = tail;
            hash_remove(nodes[evict].key);
            // Unlink tail
            tail = nodes[evict].prev;
            if (tail != -1) nodes[tail].next = -1;
            else head = -1;
            nodes[evict].used = false;
            count--;
        }

        idx = alloc_node();
        nodes[idx].key = key;
        nodes[idx].value = value;
        nodes[idx].used = true;
        nodes[idx].prev = -1;
        nodes[idx].next = head;
        if (head != -1) nodes[head].prev = idx;
        head = idx;
        if (tail == -1) tail = idx;
        count++;
        hash_insert(key, idx);
    }

    int size() const { return count; }
};

// ============================================================
// Q14: Button Debounce
// ============================================================
/*
 * Debouncing filters out the rapid on/off transitions (bounces) that
 * occur when mechanical switches are pressed or released.
 *
 * Counter-based approach: Require N consecutive same readings before
 * accepting a state change. Simple, efficient, and non-blocking.
 *
 * Integrator approach: Use a counter that increments on HIGH, decrements
 * on LOW. Output only changes when counter hits limits. More noise-immune.
 */

class Q14_PollingDebouncer {
    static const int REQUIRED_COUNT = 5;  // Consecutive readings needed

    bool stable_state;      // Current debounced state
    bool last_raw_state;    // Last raw reading
    int consecutive_count;  // Count of consecutive same readings
    bool pressed_flag;      // Edge: just pressed
    bool released_flag;     // Edge: just released

public:
    Q14_PollingDebouncer()
        : stable_state(false), last_raw_state(false),
          consecutive_count(0), pressed_flag(false), released_flag(false) {}

    // Call this regularly (e.g., every 5ms from main loop)
    // raw_input: current GPIO reading (true = pressed)
    void update(bool raw_input) {
        if (raw_input == last_raw_state) {
            // Same as last reading, increment counter
            if (consecutive_count < REQUIRED_COUNT) {
                consecutive_count++;
            }

            // If we've seen enough consecutive readings, update stable state
            if (consecutive_count >= REQUIRED_COUNT && raw_input != stable_state) {
                // State change confirmed
                bool old_state = stable_state;
                stable_state = raw_input;

                // Set edge flags
                if (stable_state && !old_state) {
                    pressed_flag = true;  // Rising edge
                } else if (!stable_state && old_state) {
                    released_flag = true; // Falling edge
                }
            }
        } else {
            // Different from last reading, reset counter
            consecutive_count = 1;
            last_raw_state = raw_input;
        }
    }

    // Get current debounced state
    bool get_state() const {
        return stable_state;
    }

    // Check if button was just pressed (clears flag after reading)
    bool was_pressed() {
        bool result = pressed_flag;
        pressed_flag = false;
        return result;
    }

    // Check if button was just released (clears flag after reading)
    bool was_released() {
        bool result = released_flag;
        released_flag = false;
        return result;
    }
};

class Q14_IntegratorDebouncer {
    static const int MAX_COUNT = 10;
    static const int THRESHOLD_HIGH = 8;  // Go HIGH above this
    static const int THRESHOLD_LOW = 2;   // Go LOW below this

    int integrator;    // Current integrator value
    bool output_state; // Current output state

public:
    Q14_IntegratorDebouncer() : integrator(0), output_state(false) {}

    // Call regularly with current raw input
    void update(bool raw_input) {
        // Increment or decrement integrator based on input
        if (raw_input) {
            if (integrator < MAX_COUNT) {
                integrator++;
            }
        } else {
            if (integrator > 0) {
                integrator--;
            }
        }

        // Update output with hysteresis
        if (integrator >= THRESHOLD_HIGH) {
            output_state = true;
        } else if (integrator <= THRESHOLD_LOW) {
            output_state = false;
        }
        // Between thresholds: keep previous state (hysteresis)
    }

    bool get_state() const {
        return output_state;
    }

    int get_integrator() const {
        return integrator;
    }
};

// BONUS: Multi-button manager with press/release/long-press/double-click detection.
// Each button is debounced independently. A single callback receives all events.
#include <functional>
class Q14_ButtonManager {
public:
    enum Event { NONE, PRESSED, RELEASED, LONG_PRESS, DOUBLE_CLICK };
    using EventCallback = std::function<void(int button_id, Event event)>;

private:
    static const int MAX_BUTTONS = 8;
    static const int DEBOUNCE_COUNT = 5;
    static const uint32_t LONG_PRESS_MS = 1000;
    static const uint32_t DOUBLE_CLICK_MS = 300;

    struct ButtonState {
        int id;
        bool (*gpio_read)();
        bool stable;         // Debounced state
        bool last_raw;
        int consec;          // Consecutive same readings
        uint32_t press_time; // Tick when press was confirmed
        uint32_t release_time;
        bool waiting_double;  // Waiting to see if a second click follows
        bool active;
    };

    ButtonState buttons[MAX_BUTTONS];
    int button_count;
    EventCallback callback;
    uint32_t current_tick;

public:
    Q14_ButtonManager() : button_count(0), current_tick(0) {}

    void register_button(int button_id, bool (*gpio_read_func)()) {
        if (button_count >= MAX_BUTTONS) return;
        ButtonState& b = buttons[button_count++];
        b.id = button_id;
        b.gpio_read = gpio_read_func;
        b.stable = false;
        b.last_raw = false;
        b.consec = 0;
        b.press_time = 0;
        b.release_time = 0;
        b.waiting_double = false;
        b.active = true;
    }

    void set_callback(EventCallback cb) { callback = cb; }

    // Call periodically (e.g., every 5ms). tick_ms = current time in ms.
    void update(uint32_t tick_ms) {
        current_tick = tick_ms;
        for (int i = 0; i < button_count; i++) {
            ButtonState& b = buttons[i];
            if (!b.active) continue;

            bool raw = b.gpio_read();

            // Debounce
            if (raw == b.last_raw) {
                if (b.consec < DEBOUNCE_COUNT) b.consec++;
            } else {
                b.consec = 1;
                b.last_raw = raw;
                continue;
            }

            if (b.consec < DEBOUNCE_COUNT || raw == b.stable) {
                // Check long press while held
                if (b.stable && (current_tick - b.press_time >= LONG_PRESS_MS)) {
                    if (callback) callback(b.id, LONG_PRESS);
                    b.press_time = current_tick;  // Reset to avoid repeat
                }
                // Check double-click timeout
                if (b.waiting_double && !b.stable &&
                    (current_tick - b.release_time > DOUBLE_CLICK_MS)) {
                    b.waiting_double = false;
                    // Single click confirmed (no second press came)
                }
                continue;
            }

            // State changed
            bool old = b.stable;
            b.stable = raw;

            if (b.stable && !old) {
                // Press
                if (b.waiting_double &&
                    (current_tick - b.release_time <= DOUBLE_CLICK_MS)) {
                    if (callback) callback(b.id, DOUBLE_CLICK);
                    b.waiting_double = false;
                } else {
                    if (callback) callback(b.id, PRESSED);
                }
                b.press_time = current_tick;
            } else if (!b.stable && old) {
                // Release
                if (callback) callback(b.id, RELEASED);
                b.release_time = current_tick;
                b.waiting_double = true;
            }
        }
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
// Q16: Volatile Usage
// ============================================================
/*
 * volatile tells the compiler:
 * 1. Don't cache this value in a register - always read from memory
 * 2. Don't optimize away reads/writes
 * 3. Don't reorder volatile accesses
 *
 * Essential for:
 * - Memory-mapped hardware registers
 * - Variables modified by ISRs
 * - Variables shared with DMA
 */

// Wait for a register to reach expected value, with timeout
bool q16_wait_for_value(volatile uint32_t* reg_addr, uint32_t expected, uint32_t timeout) {
    // Each iteration reads from actual memory due to volatile
    for (uint32_t i = 0; i < timeout; i++) {
        if (*reg_addr == expected) {
            return true;  // Value reached
        }
        // In real code, might add a small delay here
    }
    return false;  // Timeout
}

// Safe read of 64-bit value from two 32-bit registers
// Handles case where value might change between reads
uint64_t q16_read_double_register(volatile uint32_t* reg_high, volatile uint32_t* reg_low) {
    uint32_t high1, high2, low;

    // Read high-low-high pattern to detect changes
    do {
        high1 = *reg_high;
        low = *reg_low;
        high2 = *reg_high;
    } while (high1 != high2);  // Retry if high word changed

    return ((uint64_t)high1 << 32) | low;
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
// Q18: Data Logger Design
// ============================================================
/*
 * Key concepts for power-safe flash data logging:
 *
 * 1. Two-phase commit: Write data first, then write commit marker.
 *    If power fails between steps, uncommitted data is ignored on restart.
 *
 * 2. Sequence numbers: Each entry has a sequence number for ordering.
 *    Allows finding newest/oldest entries after restart.
 *
 * 3. Circular buffer: When flash is full, erase oldest sector and reuse.
 *
 * 4. Wear leveling: Distribute writes across all sectors to extend lifetime.
 */

// Simplified data logger demonstrating key concepts
class Q18_DataLogger {
    static const size_t SECTOR_SIZE = 4096;
    static const size_t NUM_SECTORS = 4;
    static const size_t ENTRY_SIZE = 32;
    static const size_t ENTRIES_PER_SECTOR = SECTOR_SIZE / ENTRY_SIZE;

    // Simulated flash storage
    uint8_t flash[NUM_SECTORS * SECTOR_SIZE];

    // Current write position
    size_t current_sector;
    size_t current_entry;
    uint32_t next_sequence;

    struct LogEntry {
        uint32_t sequence;      // Entry sequence number
        uint32_t timestamp;     // Entry timestamp
        uint8_t data[20];       // Payload data
        uint32_t crc;           // Data integrity check
        uint8_t committed;      // 0x00 = committed, 0xFF = uncommitted
        uint8_t padding[3];
    };

public:
    Q18_DataLogger() {
        // Initialize flash to erased state (all 0xFF)
        memset(flash, 0xFF, sizeof(flash));
        current_sector = 0;
        current_entry = 0;
        next_sequence = 0;
    }

    // Initialize logger, scan flash for existing data
    bool init() {
        // Scan all sectors to find current write position
        uint32_t max_sequence = 0;
        bool found_any = false;

        for (size_t s = 0; s < NUM_SECTORS; s++) {
            for (size_t e = 0; e < ENTRIES_PER_SECTOR; e++) {
                LogEntry* entry = get_entry_ptr(s, e);

                // Check if entry is committed and valid
                if (entry->committed == 0x00 && verify_crc(entry)) {
                    found_any = true;
                    if (entry->sequence >= max_sequence) {
                        max_sequence = entry->sequence;
                        current_sector = s;
                        current_entry = e + 1;
                    }
                }
            }
        }

        // Handle wraparound within sector
        if (current_entry >= ENTRIES_PER_SECTOR) {
            current_sector = (current_sector + 1) % NUM_SECTORS;
            current_entry = 0;
        }

        next_sequence = found_any ? max_sequence + 1 : 0;
        return true;
    }

    // Write a log entry (power-safe)
    bool write(const uint8_t* data, size_t len, uint32_t timestamp) {
        if (len > 20) len = 20;

        // Check if we need to move to next sector
        if (current_entry >= ENTRIES_PER_SECTOR) {
            current_sector = (current_sector + 1) % NUM_SECTORS;
            current_entry = 0;
            erase_sector(current_sector);
        }

        LogEntry* entry = get_entry_ptr(current_sector, current_entry);

        // Phase 1: Write data with committed = 0xFF (uncommitted)
        entry->sequence = next_sequence;
        entry->timestamp = timestamp;
        memcpy(entry->data, data, len);
        if (len < 20) memset(entry->data + len, 0, 20 - len);
        entry->crc = calculate_crc(entry);
        // committed is already 0xFF (erased state)

        // Phase 2: Write commit marker
        // In real flash, this writes 0x00 over 0xFF (valid operation)
        entry->committed = 0x00;

        // Advance position
        current_entry++;
        next_sequence++;

        return true;
    }

    // Get entry count
    size_t get_entry_count() const {
        size_t count = 0;
        for (size_t s = 0; s < NUM_SECTORS; s++) {
            for (size_t e = 0; e < ENTRIES_PER_SECTOR; e++) {
                const LogEntry* entry = get_entry_ptr_const(s, e);
                if (entry->committed == 0x00) {
                    count++;
                }
            }
        }
        return count;
    }

    uint32_t get_next_sequence() const {
        return next_sequence;
    }

private:
    LogEntry* get_entry_ptr(size_t sector, size_t entry) {
        return (LogEntry*)&flash[sector * SECTOR_SIZE + entry * ENTRY_SIZE];
    }

    const LogEntry* get_entry_ptr_const(size_t sector, size_t entry) const {
        return (const LogEntry*)&flash[sector * SECTOR_SIZE + entry * ENTRY_SIZE];
    }

    void erase_sector(size_t sector) {
        memset(&flash[sector * SECTOR_SIZE], 0xFF, SECTOR_SIZE);
    }

    uint32_t calculate_crc(const LogEntry* entry) {
        // Simple checksum (use CRC32 in real implementation)
        uint32_t sum = entry->sequence + entry->timestamp;
        for (int i = 0; i < 20; i++) sum += entry->data[i];
        return sum;
    }

    bool verify_crc(const LogEntry* entry) {
        return calculate_crc(entry) == entry->crc;
    }
};

// ============================================================
// Q19: OTA Firmware Update
// ============================================================
/*
 * A/B slot update strategy:
 *
 * 1. Download new firmware to inactive slot (B if A is active)
 * 2. Verify firmware integrity (CRC, optional signature)
 * 3. Mark new slot as "pending"
 * 4. Reboot
 * 5. Bootloader sees pending slot, tries to boot it
 * 6. If boot succeeds, application calls confirm_boot()
 * 7. If boot fails N times, bootloader falls back to other slot
 *
 * This ensures the device always has a working firmware to boot.
 */

class Q19_OTAManager {
    static const uint8_t MAX_BOOT_ATTEMPTS = 3;

    enum SlotState : uint8_t {
        STATE_EMPTY = 0xFF,
        STATE_VALID = 0x01,
        STATE_PENDING = 0x02,
        STATE_ACTIVE = 0x03
    };

    struct BootConfig {
        uint8_t active_slot;      // Currently active slot (0 or 1)
        uint8_t pending_slot;     // Slot pending activation (0, 1, or 0xFF)
        uint8_t boot_attempts;    // Failed boot counter
        uint8_t slot_states[2];   // State of each slot
        uint32_t slot_versions[2];// Version in each slot
    };

    BootConfig config;
    bool downloading;
    uint8_t download_slot;

public:
    Q19_OTAManager() : downloading(false), download_slot(0xFF) {
        // Default config: slot 0 active, nothing pending
        config.active_slot = 0;
        config.pending_slot = 0xFF;
        config.boot_attempts = 0;
        config.slot_states[0] = STATE_ACTIVE;
        config.slot_states[1] = STATE_EMPTY;
        config.slot_versions[0] = 1;
        config.slot_versions[1] = 0;
    }

    // Begin downloading new firmware to inactive slot
    bool begin_update() {
        download_slot = (config.active_slot == 0) ? 1 : 0;
        // In real code: erase the inactive slot here
        downloading = true;
        return true;
    }

    // Write chunk of firmware data
    bool write_chunk(uint32_t offset, const uint8_t* data, size_t length) {
        if (!downloading) return false;
        // In real code: write to flash at inactive slot + offset
        (void)offset; (void)data; (void)length;
        return true;
    }

    // Finalize download, verify firmware
    bool finalize_update(uint32_t new_version) {
        if (!downloading) return false;

        // In real code: verify CRC/signature of downloaded firmware
        // Mark slot as valid
        config.slot_states[download_slot] = STATE_VALID;
        config.slot_versions[download_slot] = new_version;
        downloading = false;

        return true;
    }

    // Mark new firmware for boot on next reset
    bool activate_update() {
        if (config.slot_states[download_slot] != STATE_VALID) {
            return false;
        }

        config.slot_states[download_slot] = STATE_PENDING;
        config.pending_slot = download_slot;
        config.boot_attempts = 0;

        return true;
    }

    // Called by bootloader to decide which slot to boot
    uint8_t get_boot_slot() {
        // If there's a pending slot, try it
        if (config.pending_slot != 0xFF) {
            if (config.boot_attempts < MAX_BOOT_ATTEMPTS) {
                config.boot_attempts++;
                return config.pending_slot;
            } else {
                // Too many failed attempts, rollback
                config.pending_slot = 0xFF;
                config.boot_attempts = 0;
                return config.active_slot;
            }
        }

        return config.active_slot;
    }

    // Called by application after successful boot
    bool confirm_boot() {
        uint8_t booted_slot = (config.pending_slot != 0xFF) ?
                              config.pending_slot : config.active_slot;

        // Mark as active, clear pending
        config.slot_states[booted_slot] = STATE_ACTIVE;
        config.slot_states[1 - booted_slot] = STATE_VALID; // Keep as fallback
        config.active_slot = booted_slot;
        config.pending_slot = 0xFF;
        config.boot_attempts = 0;

        return true;
    }

    uint32_t get_current_version() const {
        return config.slot_versions[config.active_slot];
    }
};

// ============================================================
// Q20: Task Scheduler
// ============================================================
/*
 * Cooperative priority-based scheduler for bare-metal systems.
 *
 * Tasks are sorted by priority. Each call to run() executes one
 * ready task (highest priority that's due to run).
 *
 * For periodic tasks: next_run_tick determines when task is eligible.
 * For event-driven tasks: signal_task() sets pending flag.
 */

class Q20_Scheduler {
    static const size_t MAX_TASKS = 16;

    struct Task {
        const char* name;
        void (*function)(void*);
        void* context;
        uint8_t priority;    // Lower = higher priority
        bool enabled;
        bool pending;        // Event-triggered
        uint32_t period_ms;  // 0 = one-shot/event-driven
        uint32_t next_run;   // Next eligible run time
        uint32_t run_count;
    };

    Task tasks[MAX_TASKS];
    size_t task_count;
    bool running;

    // Simulated tick count (in real system, from hardware timer)
    uint32_t current_tick;

public:
    Q20_Scheduler() : task_count(0), running(false), current_tick(0) {
        memset(tasks, 0, sizeof(tasks));
    }

    // Create a new task
    int create_task(const char* name, void (*func)(void*), void* ctx,
                    uint8_t priority, uint32_t period_ms = 0) {
        if (task_count >= MAX_TASKS) return -1;

        Task& t = tasks[task_count];
        t.name = name;
        t.function = func;
        t.context = ctx;
        t.priority = priority;
        t.enabled = true;
        t.pending = (period_ms == 0);  // One-shot tasks start pending
        t.period_ms = period_ms;
        t.next_run = current_tick;
        t.run_count = 0;

        return task_count++;
    }

    // Signal a task to run (for event-driven tasks)
    bool signal_task(int id) {
        if (id < 0 || (size_t)id >= task_count) return false;
        tasks[id].pending = true;
        return true;
    }

    // Suspend a task
    bool suspend_task(int id) {
        if (id < 0 || (size_t)id >= task_count) return false;
        tasks[id].enabled = false;
        return true;
    }

    // Resume a task
    bool resume_task(int id) {
        if (id < 0 || (size_t)id >= task_count) return false;
        tasks[id].enabled = true;
        return true;
    }

    void start() { running = true; }
    void stop() { running = false; }

    // Run one iteration of scheduler
    void run() {
        if (!running) return;

        // Find highest priority ready task
        int best_task = -1;
        uint8_t best_priority = 255;

        for (size_t i = 0; i < task_count; i++) {
            Task& t = tasks[i];
            if (!t.enabled) continue;

            // Check if task is ready to run
            bool ready = false;
            if (t.pending) {
                ready = true;  // Event-triggered
            } else if (t.period_ms > 0 && current_tick >= t.next_run) {
                ready = true;  // Periodic and time has come
            }

            if (ready && t.priority < best_priority) {
                best_task = i;
                best_priority = t.priority;
            }
        }

        // Execute the best task
        if (best_task >= 0) {
            Task& t = tasks[best_task];

            // Call task function
            t.function(t.context);
            t.run_count++;

            // Update scheduling state
            t.pending = false;
            if (t.period_ms > 0) {
                t.next_run = current_tick + t.period_ms;
            }
        }
    }

    // Advance tick count (called from timer ISR in real system)
    void tick(uint32_t ticks = 1) {
        current_tick += ticks;
    }

    const Task* get_task_info(int id) const {
        if (id < 0 || (size_t)id >= task_count) return nullptr;
        return &tasks[id];
    }

    size_t get_task_count() const { return task_count; }
};

// ============================================================
// Q21: Stack Overflow Detection
// ============================================================
/*
 * Without an MMU, stack overflow silently corrupts memory.
 *
 * Canary method: Place known value at stack bottom, check periodically.
 * If value changed, overflow occurred.
 *
 * Painting method: Fill stack with pattern at startup, later check
 * how much pattern remains to measure high-water mark.
 */

namespace Q21_StackOverflow {

const size_t STACK_SIZE = 1024;
const uint32_t CANARY_VALUE = 0xDEADBEEF;
const uint8_t PAINT_PATTERN = 0xCD;

// Simulated stack
uint8_t stack[STACK_SIZE];
uint8_t* stack_bottom = stack;
uint8_t* stack_top = stack + STACK_SIZE;

// Initialize canary at bottom of stack
void init_stack_canary() {
    // Place canary value at stack bottom
    // (In real code, this goes at the lowest stack address)
    uint32_t* canary_ptr = (uint32_t*)stack_bottom;
    *canary_ptr = CANARY_VALUE;
}

// Check if canary is intact
bool check_stack_canary() {
    uint32_t* canary_ptr = (uint32_t*)stack_bottom;
    return *canary_ptr == CANARY_VALUE;
}

// Paint entire stack with known pattern
void paint_stack() {
    memset(stack, PAINT_PATTERN, STACK_SIZE);
}

// Measure stack usage by checking paint watermark
size_t measure_stack_usage() {
    // Scan from bottom (low address) upward
    // Count bytes that are still painted
    size_t unpainted = 0;

    for (size_t i = 0; i < STACK_SIZE; i++) {
        if (stack[i] != PAINT_PATTERN) {
            // Found used area, stack usage is rest of stack
            unpainted = STACK_SIZE - i;
            break;
        }
    }

    return unpainted;
}

// Get free stack space
size_t get_free_stack(uint8_t* current_sp) {
    if (current_sp >= stack_bottom && current_sp <= stack_top) {
        return current_sp - stack_bottom;
    }
    return 0;
}

// Call periodically to check for overflow
void stack_monitor() {
    if (!check_stack_canary()) {
        // Stack overflow detected!
        // In real code: log error, save state, reset
        printf("!!! STACK OVERFLOW DETECTED !!!\n");
    }
}

} // namespace Q21_StackOverflow

// BONUS: Function-level stack protector (simulates -fstack-protector).
// A random canary is placed on the stack at function entry and checked at exit.
// If a buffer overflow corrupts the stack, the canary changes and is detected.
namespace Q21_StackProtector {

// Global canary value, randomized at init (use HWRNG in real system)
static uint32_t __stack_chk_guard = 0;

void init_stack_protector() {
    // In production, seed from hardware RNG or /dev/urandom
    __stack_chk_guard = 0x12345678 ^ (uint32_t)(uintptr_t)&__stack_chk_guard;
}

// Called when stack smashing is detected - should not return
void __stack_chk_fail() {
    printf("*** Stack smashing detected ***\n");
    // In real system: log, dump registers, reboot
}

// Macros for instrumenting functions:
//   STACK_PROTECT_ENTER() at function start places canary on stack.
//   STACK_PROTECT_EXIT()  at function end verifies canary is intact.
#define Q21_STACK_PROTECT_ENTER() \
    volatile uint32_t __stack_canary = Q21_StackProtector::__stack_chk_guard

#define Q21_STACK_PROTECT_EXIT() \
    do { \
        if (__stack_canary != Q21_StackProtector::__stack_chk_guard) { \
            Q21_StackProtector::__stack_chk_fail(); \
        } \
    } while(0)

// Example usage:
// void sensitive_function() {
//     Q21_STACK_PROTECT_ENTER();
//     char buffer[64];
//     // ... work with buffer ...
//     Q21_STACK_PROTECT_EXIT();
// }

} // namespace Q21_StackProtector

// ============================================================
// Q22: Memory Leak Detection
// ============================================================
/*
 * In resource-constrained systems, track allocations vs frees.
 * If they don't match, there's a leak.
 *
 * Checkpoint method: Record allocation count before an operation,
 * verify same number of frees after. Pinpoints leaky operations.
 *
 * Trend monitoring: Track heap usage over time. Steady increase
 * indicates a leak even if you don't know the source.
 */

class Q22_MemoryMonitor {
    static const size_t HISTORY_SIZE = 10;

    size_t usage_history[HISTORY_SIZE];
    size_t history_index;
    size_t sample_count;

    // Simulated heap stats
    size_t current_usage;
    size_t total_allocs;
    size_t total_frees;

public:
    Q22_MemoryMonitor() : history_index(0), sample_count(0),
                          current_usage(0), total_allocs(0), total_frees(0) {
        memset(usage_history, 0, sizeof(usage_history));
    }

    // Record an allocation
    void record_alloc(size_t size) {
        current_usage += size;
        total_allocs++;
    }

    // Record a free
    void record_free(size_t size) {
        if (size <= current_usage) current_usage -= size;
        total_frees++;
    }

    // Take a sample of current usage
    void sample() {
        usage_history[history_index] = current_usage;
        history_index = (history_index + 1) % HISTORY_SIZE;
        sample_count++;
    }

    // Check if allocations match frees
    bool check_balance() const {
        return total_allocs == total_frees;
    }

    // Detect steadily increasing memory usage (likely leak)
    bool detect_leak_trend() const {
        if (sample_count < HISTORY_SIZE) return false;

        // Check if each sample is >= previous
        // (Simple heuristic - real code would use linear regression)
        size_t increases = 0;
        for (size_t i = 1; i < HISTORY_SIZE; i++) {
            size_t prev_idx = (history_index + i - 1) % HISTORY_SIZE;
            size_t curr_idx = (history_index + i) % HISTORY_SIZE;
            if (usage_history[curr_idx] > usage_history[prev_idx]) {
                increases++;
            }
        }

        // If most samples show increase, likely leak
        return increases >= HISTORY_SIZE - 2;
    }

    size_t get_current_usage() const { return current_usage; }
    size_t get_alloc_count() const { return total_allocs; }
    size_t get_free_count() const { return total_frees; }
};

// Checkpoint for detecting leaks in specific code sections
class Q22_MemoryCheckpoint {
    size_t start_allocs;
    size_t start_frees;
    size_t end_allocs;
    size_t end_frees;
    Q22_MemoryMonitor* monitor;

public:
    Q22_MemoryCheckpoint(Q22_MemoryMonitor* mon) : monitor(mon) {}

    void start() {
        start_allocs = monitor->get_alloc_count();
        start_frees = monitor->get_free_count();
    }

    void end() {
        end_allocs = monitor->get_alloc_count();
        end_frees = monitor->get_free_count();
    }

    bool leaked() const {
        size_t allocs = end_allocs - start_allocs;
        size_t frees = end_frees - start_frees;
        return allocs != frees;
    }

    int bytes_delta() const {
        // Simplified - would need size tracking for accurate byte count
        return (int)(end_allocs - start_allocs) - (int)(end_frees - start_frees);
    }
};

// ============================================================
// Q23: ARM Cortex-M Hard Fault Analysis
// ============================================================
/*
 * When a hard fault occurs on ARM Cortex-M, key registers tell you why:
 *
 * HFSR: Hard Fault Status Register
 *   - FORCED bit: Fault escalated from another fault type
 *
 * CFSR: Configurable Fault Status Register (contains UFSR, BFSR, MMFSR)
 *   - UFSR: Usage faults (undefined instruction, div by zero, etc.)
 *   - BFSR: Bus faults (invalid memory access)
 *   - MMFSR: Memory management faults (MPU violations)
 *
 * BFAR/MMFAR: Fault address registers (when valid)
 *
 * Stacked PC: The instruction that caused the fault
 */

namespace Q23_HardFault {

// Fault status bits (same as Cortex-M definitions)
const uint32_t HFSR_FORCED = (1 << 30);
const uint32_t HFSR_VECTTBL = (1 << 1);

const uint32_t UFSR_DIVBYZERO = (1 << 25);
const uint32_t UFSR_UNALIGNED = (1 << 24);
const uint32_t UFSR_UNDEFINSTR = (1 << 16);

const uint32_t BFSR_BFARVALID = (1 << 15);
const uint32_t BFSR_STKERR = (1 << 12);
const uint32_t BFSR_PRECISERR = (1 << 9);
const uint32_t BFSR_IBUSERR = (1 << 8);

const uint32_t MMFSR_MMARVALID = (1 << 7);
const uint32_t MMFSR_DACCVIOL = (1 << 1);
const uint32_t MMFSR_IACCVIOL = (1 << 0);

// Simulated fault registers
struct FaultRegisters {
    uint32_t hfsr;
    uint32_t cfsr;
    uint32_t bfar;
    uint32_t mmfar;
    uint32_t pc;      // Faulting instruction
    uint32_t lr;      // Return address
    uint32_t sp;      // Stack pointer
};

// Analyze fault and return description
const char* get_fault_type(uint32_t cfsr) {
    // Check Usage Faults (UFSR - bits 16-25)
    if (cfsr & UFSR_DIVBYZERO) return "Division by zero";
    if (cfsr & UFSR_UNALIGNED) return "Unaligned memory access";
    if (cfsr & UFSR_UNDEFINSTR) return "Undefined instruction";

    // Check Bus Faults (BFSR - bits 8-15)
    if (cfsr & BFSR_STKERR) return "Stack push bus error (likely overflow)";
    if (cfsr & BFSR_PRECISERR) return "Precise bus error (invalid address)";
    if (cfsr & BFSR_IBUSERR) return "Instruction bus error";

    // Check Memory Management Faults (MMFSR - bits 0-7)
    if (cfsr & MMFSR_DACCVIOL) return "MPU data access violation";
    if (cfsr & MMFSR_IACCVIOL) return "MPU instruction access violation";

    return "Unknown fault";
}

// Get fault address if available
bool get_fault_address(uint32_t cfsr, uint32_t bfar, uint32_t mmfar, uint32_t* addr) {
    if (cfsr & BFSR_BFARVALID) {
        *addr = bfar;
        return true;
    }
    if (cfsr & MMFSR_MMARVALID) {
        *addr = mmfar;
        return true;
    }
    return false;
}

// Check if fault is likely stack overflow
bool is_stack_overflow(const FaultRegisters& regs, uint32_t stack_bottom) {
    // Check for stack error bit
    if (regs.cfsr & BFSR_STKERR) return true;

    // Check if SP is near stack bottom
    if (regs.sp < stack_bottom + 64) return true;

    return false;
}

// Print fault analysis
void analyze_hard_fault(const FaultRegisters& regs) {
    printf("\n========== HARD FAULT ANALYSIS ==========\n\n");

    // Check if escalated from another fault
    if (regs.hfsr & HFSR_FORCED) {
        printf("Fault Type: ESCALATED (check CFSR for details)\n");
    } else if (regs.hfsr & HFSR_VECTTBL) {
        printf("Fault Type: Vector table read error\n");
    }

    // Decode CFSR
    printf("Cause: %s\n\n", get_fault_type(regs.cfsr));

    // Print fault address if valid
    uint32_t fault_addr;
    if (get_fault_address(regs.cfsr, regs.bfar, regs.mmfar, &fault_addr)) {
        printf("Fault Address: 0x%08X\n", fault_addr);
        if (fault_addr < 0x100) {
            printf("  -> Likely NULL pointer dereference\n");
        }
    }

    printf("\nFaulting Instruction (PC): 0x%08X\n", regs.pc);
    printf("Return Address (LR): 0x%08X\n", regs.lr);
    printf("Stack Pointer (SP): 0x%08X\n", regs.sp);

    printf("\nRaw Registers:\n");
    printf("  HFSR  = 0x%08X\n", regs.hfsr);
    printf("  CFSR  = 0x%08X\n", regs.cfsr);
    printf("  BFAR  = 0x%08X\n", regs.bfar);
    printf("  MMFAR = 0x%08X\n", regs.mmfar);

    printf("\n==========================================\n");
}

} // namespace Q23_HardFault

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


// ============================================================
// Demo helpers for Q1 and Q2 stream interface
// ============================================================
static std::vector<uint32_t> g_demo_data;
static std::vector<uint32_t> g_demo_output;
static size_t g_demo_pos = 0;

uint64_t demo_get_count() { return g_demo_data.size(); }
void demo_read(uint64_t i, uint32_t* v) { *v = g_demo_data[(size_t)i]; }
void demo_write(uint32_t v) { g_demo_output.push_back(v); }

// Stream helpers for Q2
bool demo_read_next(uint32_t* v) {
    if (g_demo_pos >= g_demo_data.size()) return false;
    *v = g_demo_data[g_demo_pos++];
    return true;
}
void demo_reset() { g_demo_pos = 0; }

int main() {
    printf("Reference Solutions File\n");
    printf("========================\n\n");
    printf("This file contains reference implementations.\n");
    printf("Review the code after attempting problems yourself.\n\n");

    // Q1 demo
    printf("Demo: Q1 - Find unique values in {5,3,1,3,5,2}\n");
    g_demo_data = {5, 3, 1, 3, 5, 2};
    g_demo_output.clear();
    q1_find_unique_values_solution(demo_get_count, demo_read, demo_write);
    printf("  Unique (sorted): ");
    for (uint32_t v : g_demo_output) printf("%u ", v);
    printf("\n");

    // Q2 demo
    printf("\nDemo: Q2 - Find missing from {0,1,3,4} (n=5, missing=2)\n");
    g_demo_data = {0, 1, 3, 4};
    g_demo_pos = 0;
    printf("  Single missing: %u\n",
           q2_find_missing_xor(demo_read_next, 5));

    printf("  Two missing from {0,2,4,5,7,8,9} (n=10, missing=1,3,6 — demo: missing=1,6):\n");
    g_demo_data = {0, 2, 3, 4, 5, 7, 8, 9};  // missing 1 and 6 from [0,9]
    g_demo_pos = 0;
    uint32_t m1, m2;
    q2_find_two_missing(demo_read_next, demo_reset, 10, &m1, &m2);
    printf("  Found: %u and %u\n", m1, m2);

    // Q5 demo
    printf("\nDemo: Q5 - Reverse bits of 0x12345678\n");
    printf("  Naive:  0x%08X\n", q5_reverse_naive(0x12345678));
    printf("  D&C:    0x%08X\n", q5_reverse_divide_conquer(0x12345678));

    // Q8 demo
    printf("\nDemo: Q8 - Find single in [1,2,3,2,1]\n");
    int arr[] = {1, 2, 3, 2, 1};
    printf("  Result: %d\n", q8_find_single(arr, 5));

    // Q6 demo
    printf("\nDemo: Q6 - Extract bits 8-15 from 0xABCD1234\n");
    printf("  Result: 0x%X\n", q6_extract_bits(0xABCD1234, 8, 15));

    // Q15 demo
    printf("\nDemo: Q15 - Byte swap 0xDEADBEEF\n");
    printf("  Result: 0x%08X\n", q15_swap_bytes_32(0xDEADBEEF));

    // Q3 demo
    printf("\nDemo: Q3 - Approximate distinct count\n");
    Q3_LinearCounter lc;
    for (int i = 0; i < 1000; i++) lc.add((uint16_t)(i % 200));
    printf("  Actual distinct: 200, Estimated: %u\n", lc.estimate());

    // Q23 demo
    printf("\nDemo: Q23 - Hard fault analysis\n");
    Q23_HardFault::FaultRegisters regs;
    regs.hfsr = Q23_HardFault::HFSR_FORCED;
    regs.cfsr = Q23_HardFault::BFSR_PRECISERR | Q23_HardFault::BFSR_BFARVALID;
    regs.bfar = 0x00000000;  // NULL pointer
    regs.mmfar = 0;
    regs.pc = 0x08001234;
    regs.lr = 0x08001100;
    regs.sp = 0x20007F00;
    Q23_HardFault::analyze_hard_fault(regs);

    return 0;
}
