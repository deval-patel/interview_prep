/**
 * Q21: Stack Overflow Detection in Embedded Systems
 *
 * PROBLEM:
 * How would you detect stack overflow in an embedded system without MMU?
 *
 * BACKGROUND:
 * Most microcontrollers don't have Memory Management Units (MMU) that
 * automatically detect invalid memory access. Stack overflow can silently
 * corrupt data, cause hard-to-debug crashes, or create security vulnerabilities.
 *
 * DETECTION METHODS:
 *
 * 1. Stack Canary (Stack Guard)
 *    - Place known pattern at bottom of stack
 *    - Periodically check if pattern is intact
 *    - Overhead: Minimal (one check per context switch or timer)
 *
 * 2. MPU (Memory Protection Unit)
 *    - Configure MPU region at stack boundary
 *    - Triggers fault on access
 *    - Hardware-based, no runtime overhead
 *
 * 3. Stack Painting
 *    - Fill entire stack with known pattern at startup
 *    - Check how much of pattern remains (watermark)
 *    - Useful for determining stack usage
 *
 * 4. Compiler Instrumentation
 *    - -fstack-protector (GCC/Clang)
 *    - Adds canary checks to functions
 *    - Detects overflow at function return
 *
 * 5. Static Analysis
 *    - Calculate maximum stack depth at compile time
 *    - Tools: GCC -fstack-usage, stack analyzer
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>

// ============ SIMULATED STACK MEMORY ============

const size_t STACK_SIZE = 1024;  // 1KB stack
const uint32_t CANARY_VALUE = 0xDEADBEEF;
const uint8_t PAINT_PATTERN = 0xCD;

// Simulated stack (grows downward)
static uint8_t g_stack[STACK_SIZE];
static uint8_t* g_stack_top;     // Top of stack (high address)
static uint8_t* g_stack_bottom;  // Bottom of stack (low address)
static uint8_t* g_stack_ptr;     // Current stack pointer

void stack_init() {
    g_stack_bottom = g_stack;
    g_stack_top = g_stack + STACK_SIZE;
    g_stack_ptr = g_stack_top;  // Stack starts at top, grows down
}

// Simulate push (stack grows downward)
bool stack_push(size_t bytes) {
    if (g_stack_ptr - bytes < g_stack_bottom) {
        return false;  // Would overflow
    }
    g_stack_ptr -= bytes;
    return true;
}

// Simulate pop
void stack_pop(size_t bytes) {
    g_stack_ptr += bytes;
    if (g_stack_ptr > g_stack_top) {
        g_stack_ptr = g_stack_top;
    }
}

size_t stack_used() {
    return g_stack_top - g_stack_ptr;
}

// ============ YOUR IMPLEMENTATION ============

/**
 * Initialize stack with canary value at the bottom.
 *
 * Places a known pattern at the lowest stack address.
 * If stack overflows, this pattern will be overwritten.
 */
void init_stack_canary() {
    // TODO: Implement
    // 1. Place CANARY_VALUE at the bottom of stack
    // 2. Store the canary location for checking

}

/**
 * Check if stack canary is intact.
 *
 * @return true if canary is intact (no overflow detected)
 */
bool check_stack_canary() {
    // TODO: Implement
    // 1. Read value at canary location
    // 2. Compare with expected CANARY_VALUE

    return true;  // Placeholder
}

/**
 * Paint the entire stack with a known pattern.
 *
 * Used to measure maximum stack usage (high-water mark).
 */
void paint_stack() {
    // TODO: Implement
    // Fill entire stack with PAINT_PATTERN

}

/**
 * Measure stack usage by finding how much paint remains.
 *
 * @return Maximum stack usage in bytes
 */
size_t measure_stack_usage() {
    // TODO: Implement
    // Scan from bottom of stack upward
    // Count bytes that are still PAINT_PATTERN
    // Return (STACK_SIZE - unpainted_bytes)

    return 0;  // Placeholder
}

/**
 * Get current free stack space.
 */
size_t get_free_stack() {
    // TODO: Implement
    return 0;
}

/**
 * Stack overflow handler (called when overflow detected).
 *
 * In a real system, this might:
 * - Log the error
 * - Save crash dump
 * - Reset the system
 * - Blink an LED error code
 */
void stack_overflow_handler() {
    printf("!!! STACK OVERFLOW DETECTED !!!\n");
    printf("Stack pointer: %p\n", (void*)g_stack_ptr);
    printf("Stack bottom: %p\n", (void*)g_stack_bottom);

    // In real embedded system:
    // - Disable interrupts
    // - Save diagnostic info to persistent storage
    // - Trigger watchdog or software reset
}

/**
 * Stack monitor task.
 *
 * Call this periodically (e.g., from timer ISR or idle task).
 */
void stack_monitor() {
    // TODO: Implement
    // 1. Check canary
    // 2. Optionally check watermark
    // 3. Call handler if overflow detected

}

/**
 * BONUS: Implement function-level stack checking.
 *
 * This simulates what -fstack-protector does:
 * - Place canary on stack at function entry
 * - Check canary at function exit
 */

// Stack protector canary (random at startup in real implementation)
static uint32_t __stack_chk_guard = 0;

void init_stack_protector() {
    // In real system, use hardware random number generator
    __stack_chk_guard = 0x12345678 ^ (uint32_t)(uintptr_t)&__stack_chk_guard;
}

// Called by compiler when stack smashing detected
void __stack_chk_fail() {
    printf("*** Stack smashing detected ***\n");
    stack_overflow_handler();
}

// Macro for function protection (used at start of function)
#define STACK_PROTECT_ENTER() \
    volatile uint32_t __stack_canary = __stack_chk_guard

// Macro for function protection (used at end of function)
#define STACK_PROTECT_EXIT() \
    do { \
        if (__stack_canary != __stack_chk_guard) { \
            __stack_chk_fail(); \
        } \
    } while(0)

// Example of protected function
void example_protected_function() {
    STACK_PROTECT_ENTER();

    // Function body...
    char buffer[64];
    memset(buffer, 0, sizeof(buffer));

    STACK_PROTECT_EXIT();
}

// ============ TEST FRAMEWORK ============

bool test_canary_detection() {
    printf("--- Canary Detection Tests ---\n");
    int passed = 0;
    int total = 0;

    stack_init();
    init_stack_canary();

    // Canary should be intact initially
    if (check_stack_canary()) {
        printf("PASS: Canary intact after init\n");
        passed++;
    } else {
        printf("FAIL: Canary corrupted after init\n");
    }
    total++;

    // Normal stack usage - canary should remain intact
    stack_push(100);
    if (check_stack_canary()) {
        printf("PASS: Canary intact after normal push\n");
        passed++;
    } else {
        printf("FAIL: Canary corrupted after normal push\n");
    }
    total++;

    // Simulate overflow by corrupting canary location
    uint32_t* canary_ptr = (uint32_t*)g_stack_bottom;
    *canary_ptr = 0x00000000;  // Corrupt canary

    if (!check_stack_canary()) {
        printf("PASS: Canary corruption detected\n");
        passed++;
    } else {
        printf("FAIL: Canary corruption not detected\n");
    }
    total++;

    printf("Canary tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_stack_painting() {
    printf("--- Stack Painting Tests ---\n");
    int passed = 0;
    int total = 0;

    stack_init();
    paint_stack();

    // Initially, all stack should be painted (0 usage measured)
    size_t initial_usage = measure_stack_usage();
    printf("Initial measured usage: %zu bytes\n", initial_usage);

    // Use some stack
    stack_push(256);
    memset(g_stack_ptr, 0xAA, 256);  // Actually write to stack

    size_t after_push = measure_stack_usage();
    printf("After 256-byte push: %zu bytes\n", after_push);

    if (after_push >= 256) {
        printf("PASS: Stack usage detected\n");
        passed++;
    } else {
        printf("FAIL: Stack usage not properly detected\n");
    }
    total++;

    // Use more stack
    stack_push(256);
    memset(g_stack_ptr, 0xBB, 256);

    size_t after_more = measure_stack_usage();
    printf("After another 256-byte push: %zu bytes\n", after_more);

    if (after_more >= 512) {
        printf("PASS: Increased usage detected\n");
        passed++;
    } else {
        printf("FAIL: Increased usage not detected\n");
    }
    total++;

    printf("Stack painting tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_stack_protector() {
    printf("--- Stack Protector Tests ---\n");
    int passed = 0;
    int total = 0;

    init_stack_protector();

    // Normal function call - should not trigger
    printf("Calling protected function normally...\n");
    example_protected_function();
    printf("PASS: Normal call completed\n");
    passed++;
    total++;

    // Note: Actually triggering stack smashing would require
    // writing past buffer bounds, which is undefined behavior.
    // In real testing, you'd use a controlled test case.

    printf("Stack protector tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

void print_recommendations() {
    printf("=== Stack Overflow Prevention Recommendations ===\n\n");

    printf("1. Design Phase:\n");
    printf("   - Calculate maximum stack depth for each task\n");
    printf("   - Use static analysis tools (-fstack-usage)\n");
    printf("   - Avoid deep recursion and large stack allocations\n\n");

    printf("2. Compile Time:\n");
    printf("   - Enable -fstack-protector-strong\n");
    printf("   - Use -Wstack-usage=N warning\n");
    printf("   - Enable -fstack-limit-symbol (if supported)\n\n");

    printf("3. Runtime:\n");
    printf("   - Initialize stack canaries at startup\n");
    printf("   - Check canaries in idle task or timer ISR\n");
    printf("   - Use MPU if available\n\n");

    printf("4. Testing:\n");
    printf("   - Paint stacks and measure high-water marks\n");
    printf("   - Add 20-30%% margin to measured usage\n");
    printf("   - Test worst-case scenarios\n\n");

    printf("5. Common Stack Hogs:\n");
    printf("   - Large local arrays (use static or heap)\n");
    printf("   - printf/sprintf (use snprintf, or simpler alternatives)\n");
    printf("   - Recursive functions\n");
    printf("   - Nested function calls with many parameters\n\n");
}

int main() {
    printf("Running Q21: Stack Overflow Detection\n");
    printf("=====================================\n\n");

    bool all_passed = true;

    if (!test_canary_detection()) all_passed = false;
    if (!test_stack_painting()) all_passed = false;
    if (!test_stack_protector()) all_passed = false;

    print_recommendations();

    printf("=====================================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
