/**
 * Q16: Understanding `volatile` in Embedded C/C++
 *
 * PROBLEM:
 * Explain potential bugs in the following code and fix them:
 *
 *   int* status_reg = (int*)0x40000000;
 *   while (*status_reg == 0) { }  // Wait for ready
 *
 * ANSWER:
 * 1. Missing `volatile` - compiler may optimize away the loop
 * 2. Should be: volatile int* status_reg = (volatile int*)0x40000000;
 *
 * WHY `volatile` IS NEEDED:
 * - Tells compiler the value can change outside program's control
 * - Prevents caching value in register
 * - Prevents reordering reads/writes
 * - Forces actual memory access each time
 *
 * WHEN TO USE `volatile`:
 * - Memory-mapped hardware registers
 * - Variables modified by ISRs
 * - Variables shared with DMA
 * - Signal handlers
 *
 * WHEN NOT TO USE:
 * - Thread synchronization (use atomic/mutex instead)
 * - Normal variables (unnecessary overhead)
 *
 * INTERVIEW DISCUSSION:
 * - volatile vs atomic (C++11)
 * - Memory barriers
 * - Sequence points
 * - Compiler optimization levels
 */

#include <cstdint>
#include <cstdio>
#include <atomic>
#include <thread>
#include <chrono>

// ============ SIMULATED HARDWARE REGISTERS ============

// Simulates a status register that changes asynchronously
static volatile uint32_t g_hardware_status = 0;
static volatile uint32_t g_hardware_counter = 0;

// This simulates a hardware timer or external event
void simulate_hardware_event() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    g_hardware_status = 1;  // Set ready flag
}

void simulate_hardware_counter() {
    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        g_hardware_counter++;
    }
}

// ============ EXAMPLES AND EXERCISES ============

/**
 * EXAMPLE 1: Why volatile is needed for hardware registers
 *
 * Without volatile, the compiler may:
 * 1. Cache *status_reg in a register
 * 2. Optimize the loop to: if (*status_reg == 0) { while(1); }
 *
 * This is because the compiler sees no write to *status_reg
 * within the loop, so it assumes the value never changes.
 */

// BAD: May be optimized to infinite loop
void wait_for_ready_bad() {
    uint32_t* status_reg = (uint32_t*)&g_hardware_status;

    // Compiler might optimize this to:
    // uint32_t cached = *status_reg;
    // while (cached == 0) { }  // Infinite loop!

    while (*status_reg == 0) {
        // Spin wait
    }
}

// GOOD: volatile prevents optimization
void wait_for_ready_good() {
    volatile uint32_t* status_reg = (volatile uint32_t*)&g_hardware_status;

    while (*status_reg == 0) {
        // Each iteration reads from memory
    }
}

/**
 * EXAMPLE 2: Sequence of reads matters
 *
 * Sometimes you must read a register multiple times or in a specific order.
 * Without volatile, compiler may coalesce or reorder reads.
 */

// BAD: Reads may be optimized/reordered
uint32_t read_counter_twice_bad() {
    uint32_t* counter = (uint32_t*)&g_hardware_counter;

    uint32_t first = *counter;
    // ... do something ...
    uint32_t second = *counter;

    // Compiler might use same value for both!
    // Or might reorder the reads!

    return second - first;
}

// GOOD: Each read actually accesses memory
uint32_t read_counter_twice_good() {
    volatile uint32_t* counter = (volatile uint32_t*)&g_hardware_counter;

    uint32_t first = *counter;
    // ... do something ...
    uint32_t second = *counter;

    // Guaranteed to be two separate reads

    return second - first;
}

/**
 * EXAMPLE 3: ISR shared variable
 *
 * Variable modified by interrupt must be volatile.
 */

static volatile bool g_isr_flag = false;

// Simulated ISR
void ISR_handler() {
    g_isr_flag = true;  // Set flag (must be volatile!)
}

// Main code waiting for ISR
void wait_for_isr() {
    g_isr_flag = false;

    // Enable interrupt... (simulated)

    while (!g_isr_flag) {
        // Without volatile, this might become infinite loop
    }
}

/**
 * EXERCISE 1: Fix the bug in this code
 */
void exercise1_buggy() {
    // BUG: Find and fix the volatile-related issue

    int ready = 0;  // Should this be volatile?
    int* p_ready = &ready;

    // Simulating: another thread/ISR sets ready = 1

    while (*p_ready == 0) {
        // Wait...
    }
}

/**
 * EXERCISE 2: What's wrong with this ISR communication?
 */
static int g_shared_data = 0;      // Data from ISR
static int g_data_ready = 0;       // Flag from ISR

// ISR sets data and flag
void ISR_sets_data() {
    g_shared_data = 42;
    g_data_ready = 1;
}

// Main code reads data
int read_from_isr() {
    while (!g_data_ready) { }

    return g_shared_data;

    // Q: What could go wrong even if we add volatile?
    // A: Memory ordering! Compiler/CPU might reorder the writes.
    //    Need memory barrier between the two writes in ISR.
}

/**
 * EXERCISE 3: volatile vs atomic
 *
 * For thread synchronization, volatile is NOT enough!
 * Need atomic operations or memory barriers.
 */
void volatile_vs_atomic() {
    // volatile only ensures:
    // 1. Every access goes to memory
    // 2. Accesses aren't reordered by compiler

    // volatile does NOT ensure:
    // 1. Atomicity of read-modify-write
    // 2. Memory ordering across CPUs
    // 3. Prevention of word tearing

    // For multi-threaded code, use:
    // - std::atomic<T> (C++11)
    // - Mutexes
    // - Memory barriers
}

// ============ YOUR IMPLEMENTATION ============

/**
 * Implement a safe hardware register read with timeout.
 *
 * @param reg_addr  Address of the status register
 * @param expected  Value to wait for
 * @param timeout   Maximum iterations to wait
 * @return true if expected value seen, false on timeout
 */
bool wait_for_value(volatile uint32_t* reg_addr, uint32_t expected, uint32_t timeout) {
    // TODO: Implement safe polling with timeout

    return false;  // Placeholder
}

/**
 * Implement safe read of a multi-word value from hardware.
 *
 * Some hardware provides values across multiple registers that
 * must be read atomically. Common pattern: read until two consecutive
 * reads match.
 *
 * @param reg_addr  Address of first register word
 * @return 64-bit value (two 32-bit registers combined)
 */
uint64_t read_double_register(volatile uint32_t* reg_high, volatile uint32_t* reg_low) {
    // TODO: Implement safe multi-register read
    // Pattern: Read high, low, high again. If high changed, retry.

    return 0;  // Placeholder
}

// ============ TEST FRAMEWORK ============

bool test_volatile_wait() {
    printf("--- Volatile Wait Test ---\n");

    g_hardware_status = 0;

    // Start thread that will set status after delay
    std::thread hw_thread(simulate_hardware_event);

    // Wait with timeout
    volatile uint32_t* status = &g_hardware_status;
    bool success = wait_for_value(status, 1, 1000000);

    hw_thread.join();

    if (success) {
        printf("PASS: wait_for_value detected status change\n");
        return true;
    } else {
        printf("FAIL: wait_for_value timed out\n");
        return false;
    }
}

bool test_counter_diff() {
    printf("\n--- Counter Difference Test ---\n");

    g_hardware_counter = 0;

    // Start thread that increments counter
    std::thread hw_thread(simulate_hardware_counter);

    // Give it time to increment
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    uint32_t diff = read_counter_twice_good();

    hw_thread.join();

    // With volatile, we should see the counter changing
    if (diff > 0) {
        printf("PASS: Detected counter change (diff=%u)\n", diff);
        return true;
    } else {
        printf("Note: Counter diff was 0 (timing dependent)\n");
        return true;  // Not a failure, just timing
    }
}

void demonstrate_concepts() {
    printf("\n=== Volatile Concepts Demonstration ===\n\n");

    printf("1. volatile prevents loop optimization:\n");
    printf("   Without volatile: while(*reg == 0) might become infinite loop\n");
    printf("   With volatile: each iteration reads from memory\n\n");

    printf("2. volatile ensures read/write ordering (compiler level):\n");
    printf("   Compiler won't reorder volatile accesses\n");
    printf("   But CPU might still reorder (need memory barriers for that)\n\n");

    printf("3. volatile is NOT a substitute for:\n");
    printf("   - Atomic operations (use std::atomic for threads)\n");
    printf("   - Mutexes (for complex critical sections)\n");
    printf("   - Memory barriers (for CPU-level ordering)\n\n");

    printf("4. When to use volatile:\n");
    printf("   - Memory-mapped hardware registers\n");
    printf("   - Variables shared with ISRs (on single-core)\n");
    printf("   - Signal handlers\n");
    printf("   - Variables modified by DMA\n\n");

    printf("5. Common interview question:\n");
    printf("   Q: Can a variable be both const and volatile?\n");
    printf("   A: Yes! A read-only hardware status register:\n");
    printf("      const volatile uint32_t* status = (const volatile uint32_t*)0x40000000;\n");
    printf("      - const: Program can't write to it\n");
    printf("      - volatile: Value can change (hardware writes to it)\n\n");
}

int main() {
    printf("Running Q16: Volatile Demonstration\n");
    printf("====================================\n\n");

    bool all_passed = true;

    demonstrate_concepts();

    printf("=== Running Tests ===\n\n");

    if (!test_volatile_wait()) all_passed = false;
    if (!test_counter_diff()) all_passed = false;

    printf("\n====================================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed.\n");
        return 1;
    }
}
