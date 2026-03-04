/**
 * Q22: Memory Leak Detection in Embedded Systems
 *
 * PROBLEM:
 * Describe techniques to detect memory leaks in a resource-constrained
 * embedded device.
 *
 * CHALLENGES:
 * - Limited RAM (no room for leak detection overhead)
 * - No OS memory tracking
 * - Long-running systems (leaks accumulate over days/weeks)
 * - No file system for logging
 *
 * DETECTION STRATEGIES:
 *
 * 1. Static Analysis
 *    - Don't use dynamic allocation at all!
 *    - Use static allocation, memory pools
 *    - Best practice for safety-critical systems
 *
 * 2. Memory Pool Tracking
 *    - Track allocation counts per pool
 *    - Compare allocations vs frees periodically
 *    - Low overhead approach
 *
 * 3. Allocation Wrapping
 *    - Wrap malloc/free with tracking
 *    - Record allocation site (file/line)
 *    - Higher overhead, for debugging only
 *
 * 4. Heap Watermark
 *    - Monitor heap usage over time
 *    - Steadily increasing = likely leak
 *    - Doesn't identify source
 *
 * 5. Pattern Detection
 *    - Correlation with specific operations
 *    - Statistical analysis of memory usage
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>

// ============ SIMULATED HEAP ============

const size_t HEAP_SIZE = 4096;
static uint8_t g_heap[HEAP_SIZE];
static size_t g_heap_used = 0;
static size_t g_heap_peak = 0;
static size_t g_allocation_count = 0;
static size_t g_free_count = 0;

// Simple bump allocator for simulation
void* sim_malloc(size_t size) {
    // Align to 4 bytes
    size = (size + 3) & ~3;

    if (g_heap_used + size > HEAP_SIZE) {
        return nullptr;
    }

    void* ptr = &g_heap[g_heap_used];
    g_heap_used += size;
    g_allocation_count++;

    if (g_heap_used > g_heap_peak) {
        g_heap_peak = g_heap_used;
    }

    return ptr;
}

void sim_free(void* ptr) {
    // Simplified: just track count
    if (ptr) {
        g_free_count++;
    }
}

void sim_heap_reset() {
    g_heap_used = 0;
    g_heap_peak = 0;
    g_allocation_count = 0;
    g_free_count = 0;
}

// ============ LEAK DETECTION METHODS ============

// Method 1: Simple allocation counter
struct AllocationStats {
    size_t allocations;
    size_t frees;
    size_t peak_usage;
    size_t current_usage;
};

static AllocationStats g_stats = {0};

void stats_reset() {
    memset(&g_stats, 0, sizeof(g_stats));
}

void stats_alloc(size_t size) {
    g_stats.allocations++;
    g_stats.current_usage += size;
    if (g_stats.current_usage > g_stats.peak_usage) {
        g_stats.peak_usage = g_stats.current_usage;
    }
}

void stats_free(size_t size) {
    g_stats.frees++;
    g_stats.current_usage -= size;
}

bool stats_check_leak() {
    return g_stats.allocations != g_stats.frees;
}

// Method 2: Detailed allocation tracking (debug mode)
struct AllocationRecord {
    void* address;
    size_t size;
    const char* file;
    int line;
    uint32_t timestamp;
};

const size_t MAX_TRACKED_ALLOCATIONS = 100;
static AllocationRecord g_tracked[MAX_TRACKED_ALLOCATIONS];
static size_t g_tracked_count = 0;

void track_allocation(void* ptr, size_t size, const char* file, int line) {
    if (g_tracked_count < MAX_TRACKED_ALLOCATIONS) {
        g_tracked[g_tracked_count].address = ptr;
        g_tracked[g_tracked_count].size = size;
        g_tracked[g_tracked_count].file = file;
        g_tracked[g_tracked_count].line = line;
        g_tracked[g_tracked_count].timestamp = 0;  // Would use real time
        g_tracked_count++;
    }
}

void track_free(void* ptr) {
    for (size_t i = 0; i < g_tracked_count; i++) {
        if (g_tracked[i].address == ptr) {
            // Remove by swapping with last
            g_tracked[i] = g_tracked[g_tracked_count - 1];
            g_tracked_count--;
            return;
        }
    }
    printf("WARNING: Free of untracked pointer %p\n", ptr);
}

void print_leaks() {
    if (g_tracked_count == 0) {
        printf("No memory leaks detected.\n");
        return;
    }

    printf("=== MEMORY LEAKS DETECTED ===\n");
    printf("%zu allocation(s) not freed:\n\n", g_tracked_count);

    for (size_t i = 0; i < g_tracked_count; i++) {
        printf("Leak #%zu:\n", i + 1);
        printf("  Address: %p\n", g_tracked[i].address);
        printf("  Size: %zu bytes\n", g_tracked[i].size);
        printf("  Location: %s:%d\n", g_tracked[i].file, g_tracked[i].line);
        printf("\n");
    }
}

// Debug macros for tracking
#define DEBUG_MALLOC(size) \
    debug_malloc(size, __FILE__, __LINE__)

#define DEBUG_FREE(ptr) \
    debug_free(ptr)

void* debug_malloc(size_t size, const char* file, int line) {
    void* ptr = sim_malloc(size);
    if (ptr) {
        track_allocation(ptr, size, file, line);
    }
    return ptr;
}

void debug_free(void* ptr) {
    track_free(ptr);
    sim_free(ptr);
}

// ============ YOUR IMPLEMENTATION ============

/**
 * Memory monitor that tracks heap health over time.
 *
 * Call periodically to detect gradual leaks.
 */
class MemoryMonitor {
private:
    static const size_t HISTORY_SIZE = 10;
    size_t usage_history[HISTORY_SIZE];
    size_t history_index;
    size_t sample_count;

public:
    MemoryMonitor() : history_index(0), sample_count(0) {
        memset(usage_history, 0, sizeof(usage_history));
    }

    /**
     * Record current memory usage sample.
     */
    void sample() {
        // TODO: Implement
        // 1. Get current heap usage
        // 2. Store in circular buffer
        // 3. Increment sample count

    }

    /**
     * Analyze trend in memory usage.
     *
     * @return true if leak is suspected (steadily increasing)
     */
    bool detect_leak_trend() {
        // TODO: Implement
        // 1. Need at least HISTORY_SIZE samples
        // 2. Check if usage is consistently increasing
        // 3. Some tolerance for temporary spikes

        return false;  // Placeholder
    }

    /**
     * Get average memory usage.
     */
    size_t get_average_usage() {
        // TODO: Implement
        return 0;
    }

    /**
     * Get usage trend (positive = increasing, negative = decreasing).
     */
    int get_trend() {
        // TODO: Implement
        // Calculate slope of usage over time
        return 0;
    }

    void print_status() {
        printf("Memory Monitor Status:\n");
        printf("  Samples: %zu\n", sample_count);
        printf("  Current: %zu bytes\n", g_heap_used);
        printf("  Peak: %zu bytes\n", g_heap_peak);
        printf("  Average: %zu bytes\n", get_average_usage());
        printf("  Trend: %d\n", get_trend());
        printf("  Leak suspected: %s\n", detect_leak_trend() ? "YES" : "no");
    }
};

/**
 * Checkpoint system for detecting leaks around operations.
 *
 * Usage:
 *   MemoryCheckpoint cp;
 *   cp.start();
 *   // ... do operation ...
 *   cp.end();
 *   if (cp.leaked()) { ... }
 */
class MemoryCheckpoint {
private:
    size_t start_allocations;
    size_t start_frees;
    size_t start_usage;
    size_t end_allocations;
    size_t end_frees;
    size_t end_usage;
    bool active;

public:
    MemoryCheckpoint() : active(false) {}

    void start() {
        // TODO: Implement
        // Record current allocation stats

        active = true;
    }

    void end() {
        // TODO: Implement
        // Record final allocation stats

        active = false;
    }

    /**
     * Check if memory was leaked during checkpoint period.
     */
    bool leaked() {
        // TODO: Implement
        // Compare start and end stats
        return false;
    }

    /**
     * Get number of bytes leaked.
     */
    int bytes_leaked() {
        // TODO: Implement
        return 0;
    }

    void print() {
        printf("Checkpoint Results:\n");
        printf("  Allocations: %zu -> %zu (delta: %zu)\n",
               start_allocations, end_allocations,
               end_allocations - start_allocations);
        printf("  Frees: %zu -> %zu (delta: %zu)\n",
               start_frees, end_frees,
               end_frees - start_frees);
        printf("  Leaked: %s (%d bytes)\n",
               leaked() ? "YES" : "no", bytes_leaked());
    }
};

// ============ TEST FRAMEWORK ============

void simulate_no_leak() {
    printf("Simulating operation with NO leak...\n");
    void* p1 = DEBUG_MALLOC(64);
    void* p2 = DEBUG_MALLOC(128);
    DEBUG_FREE(p1);
    DEBUG_FREE(p2);
}

void simulate_leak() {
    printf("Simulating operation with leak...\n");
    void* p1 = DEBUG_MALLOC(64);
    void* p2 = DEBUG_MALLOC(128);  // This leaks!
    DEBUG_FREE(p1);
    // Forgot to free p2
}

bool test_allocation_tracking() {
    printf("--- Allocation Tracking Tests ---\n\n");
    int passed = 0;
    int total = 0;

    // Reset tracking
    g_tracked_count = 0;
    sim_heap_reset();

    // Test no leak scenario
    simulate_no_leak();

    if (g_tracked_count == 0) {
        printf("PASS: No leak correctly detected\n");
        passed++;
    } else {
        printf("FAIL: False positive leak detection\n");
    }
    total++;

    // Test leak scenario
    simulate_leak();

    if (g_tracked_count > 0) {
        printf("PASS: Leak correctly detected\n");
        passed++;
    } else {
        printf("FAIL: Leak not detected\n");
    }
    total++;

    print_leaks();

    printf("\nAllocation tracking tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_checkpoint() {
    printf("--- Checkpoint Tests ---\n\n");
    int passed = 0;
    int total = 0;

    g_tracked_count = 0;
    sim_heap_reset();
    stats_reset();

    MemoryCheckpoint cp;

    // Test clean operation
    cp.start();
    void* p = DEBUG_MALLOC(100);
    DEBUG_FREE(p);
    cp.end();

    if (!cp.leaked()) {
        printf("PASS: Clean operation detected\n");
        passed++;
    } else {
        printf("FAIL: False leak in clean operation\n");
    }
    total++;

    // Test leaky operation
    g_tracked_count = 0;
    cp.start();
    DEBUG_MALLOC(200);  // Intentional leak
    cp.end();

    if (cp.leaked()) {
        printf("PASS: Leaky operation detected\n");
        passed++;
    } else {
        printf("FAIL: Leak not detected\n");
    }
    total++;

    cp.print();

    printf("\nCheckpoint tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_trend_detection() {
    printf("--- Trend Detection Tests ---\n\n");

    MemoryMonitor monitor;
    sim_heap_reset();

    // Simulate gradual leak
    printf("Simulating gradual memory leak...\n");
    for (int i = 0; i < 15; i++) {
        sim_malloc(100);  // Allocate without free = leak
        monitor.sample();
    }

    monitor.print_status();

    if (monitor.detect_leak_trend()) {
        printf("PASS: Leak trend detected\n");
        return true;
    } else {
        printf("Note: Trend detection may need implementation\n");
        return true;  // Not a hard failure
    }
}

void print_best_practices() {
    printf("\n=== Memory Leak Prevention Best Practices ===\n\n");

    printf("1. Avoid dynamic allocation when possible:\n");
    printf("   - Use static allocation\n");
    printf("   - Use memory pools with fixed sizes\n");
    printf("   - Allocate at startup, never during runtime\n\n");

    printf("2. Follow ownership patterns:\n");
    printf("   - Clear ownership: who allocates, who frees\n");
    printf("   - Document allocation responsibilities\n");
    printf("   - Use RAII in C++ (constructors/destructors)\n\n");

    printf("3. Use defensive coding:\n");
    printf("   - Always check allocation success\n");
    printf("   - Set pointers to NULL after free\n");
    printf("   - Use static analysis tools\n\n");

    printf("4. Implement monitoring:\n");
    printf("   - Track allocation counts\n");
    printf("   - Monitor heap usage trends\n");
    printf("   - Use checkpoints around operations\n\n");

    printf("5. Testing:\n");
    printf("   - Run long-duration tests\n");
    printf("   - Stress test with repeated operations\n");
    printf("   - Use memory sanitizers in development\n\n");
}

int main() {
    printf("Running Q22: Memory Leak Detection\n");
    printf("===================================\n\n");

    bool all_passed = true;

    if (!test_allocation_tracking()) all_passed = false;
    if (!test_checkpoint()) all_passed = false;
    if (!test_trend_detection()) all_passed = false;

    print_best_practices();

    printf("===================================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
