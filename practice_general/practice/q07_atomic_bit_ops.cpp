/**
 * Q7: Atomic Bit Operations for Memory-Mapped Registers
 *
 * PROBLEM:
 * In embedded systems, memory-mapped registers control hardware peripherals.
 * These registers are accessed through specific memory addresses, and
 * read-modify-write operations can cause race conditions with interrupts.
 *
 * Implement atomic bit set/clear operations that are safe to use in both
 * main code and interrupt handlers.
 *
 * REAL-WORLD CONTEXT:
 * Many microcontrollers provide special "bit-banding" regions or atomic
 * set/clear registers. For example, on ARM Cortex-M:
 * - GPIOx_BSRR: Bit set register
 * - GPIOx_BRR: Bit reset register
 *
 * On others, you must disable interrupts during RMW.
 *
 * CONSTRAINTS:
 * - Registers are volatile (can change at any time)
 * - ISRs may access the same registers
 * - Must prevent race conditions
 * - Cannot assume hardware atomic instructions
 *
 * INTERVIEW TIP:
 * Discuss:
 * - Why volatile is necessary for hardware registers
 * - Race condition between read-modify-write steps
 * - Critical sections and interrupt disable/enable
 * - Hardware solutions (bit-banding, set/clear registers)
 */

#include <cstdint>
#include <cstdio>
#include <atomic>
#include <thread>
#include <chrono>

// ============ SIMULATED HARDWARE ============

// Simulates a memory-mapped register
// In real hardware, this would be: volatile uint32_t* REG = (volatile uint32_t*)0x40020000;
static volatile uint32_t g_simulated_register = 0;

// Simulates interrupt enable/disable
static volatile bool g_interrupts_enabled = true;

// Simulated interrupt state for testing
static std::atomic<bool> g_isr_running{false};
[[maybe_unused]] static std::atomic<int> g_race_condition_count{0};

// Read the register (simulates volatile read)
uint32_t reg_read() {
    return g_simulated_register;
}

// Write the register (simulates volatile write)
void reg_write(uint32_t value) {
    g_simulated_register = value;
}

// Disable interrupts (returns previous state)
bool disable_interrupts() {
    bool prev = g_interrupts_enabled;
    g_interrupts_enabled = false;
    return prev;
}

// Enable interrupts
void enable_interrupts() {
    g_interrupts_enabled = true;
}

// Restore interrupt state
void restore_interrupts(bool prev_state) {
    g_interrupts_enabled = prev_state;
}

// ============ YOUR IMPLEMENTATION ============

/**
 * UNSAFE: Demonstrate the race condition with naive RMW.
 *
 * This function is intentionally broken to show the problem.
 * DO NOT use this pattern in real code!
 */
void unsafe_set_bit(int bit) {
    // This is the WRONG way - shows the race condition
    uint32_t temp = reg_read();   // Read
    temp |= (1U << bit);          // Modify
    // <-- ISR could run here and modify register!
    reg_write(temp);              // Write (overwrites ISR's change)
}

/**
 * SAFE: Set a bit in the register atomically.
 *
 * Implement using critical section (disable/enable interrupts).
 *
 * @param bit Bit position to set (0-31)
 */
void atomic_set_bit(int bit) {
    // TODO: Implement safely using disable_interrupts/restore_interrupts
    //
    // Pattern:
    // 1. Save interrupt state and disable
    // 2. Read-modify-write
    // 3. Restore interrupt state
    bool prevState = disable_interrupts();
    uint32_t temp = reg_read();
    temp |= (1U << bit);
    reg_write(temp);
    restore_interrupts(prevState);

}

/**
 * SAFE: Clear a bit in the register atomically.
 *
 * @param bit Bit position to clear (0-31)
 */
void atomic_clear_bit(int bit) {
    // TODO: Implement safely
    bool prevState = disable_interrupts();
    uint32_t temp = reg_read();
    temp &= ~(1U << bit);
    reg_write(temp);
    restore_interrupts(prevState);

}

/**
 * SAFE: Toggle a bit in the register atomically.
 *
 * @param bit Bit position to toggle (0-31)
 */
void atomic_toggle_bit(int bit) {
    // TODO: Implement safely
    bool prevState = disable_interrupts();
    uint32_t temp = reg_read();
    temp ^= (1U << bit);
    reg_write(temp);
    restore_interrupts(prevState);

}

/**
 * SAFE: Set multiple bits defined by a mask atomically.
 *
 * @param mask Bitmask of bits to set
 */
void atomic_set_mask(uint32_t mask) {
    // TODO: Implement safely
    bool prevState = disable_interrupts();
    uint32_t temp = reg_read();
    temp |= mask;
    reg_write(temp);
    restore_interrupts(prevState);

}

/**
 * SAFE: Clear multiple bits defined by a mask atomically.
 *
 * @param mask Bitmask of bits to clear
 */
void atomic_clear_mask(uint32_t mask) {
    // TODO: Implement safely
    bool prevState = disable_interrupts();
    uint32_t temp = reg_read();
    temp &= ~mask;
    reg_write(temp);
    restore_interrupts(prevState);

}

/**
 * SAFE: Write a value to a bit field atomically.
 *
 * @param start Starting bit position
 * @param end   Ending bit position (inclusive)
 * @param value Value to write to the field
 */
void atomic_write_field(int start, int end, uint32_t value) {
    // TODO: Implement safely
    // 1. Create mask for the field
    // 2. Clear the field
    // 3. Set the new value
    uint32_t width = end - start + 1;
    uint32_t mask = ((1U << width) - 1) << start;
    bool prevState = disable_interrupts();
    uint32_t temp = reg_read();
    temp &= ~mask;                    // Clear the field
    temp |= (value << start) & mask;  // Set new value
    reg_write(temp);
    restore_interrupts(prevState);

}

/**
 * Check if a bit is set (no atomicity needed for single read).
 */
bool is_bit_set(int bit) {
    return (reg_read() & (1U << bit)) != 0;
}

// ============ TEST FRAMEWORK ============

// Simulates an ISR that modifies the register
void simulated_isr() {
    g_isr_running = true;

    // ISR sets bit 0
    if (g_interrupts_enabled) {
        uint32_t temp = reg_read();
        temp |= 0x01;  // Set bit 0
        reg_write(temp);
    }

    g_isr_running = false;
}

bool test_basic_operations() {
    printf("--- Basic Operation Tests ---\n");
    int passed = 0;
    int total = 0;

    // Test set bit
    reg_write(0);
    atomic_set_bit(5);
    if (reg_read() == 0x20) {
        printf("PASS: set_bit(5) on 0x00 = 0x20\n");
        passed++;
    } else {
        printf("FAIL: set_bit(5) on 0x00 = 0x%08X (expected 0x20)\n", reg_read());
    }
    total++;

    // Test set bit doesn't affect others
    reg_write(0x0F);
    atomic_set_bit(5);
    if (reg_read() == 0x2F) {
        printf("PASS: set_bit(5) on 0x0F = 0x2F\n");
        passed++;
    } else {
        printf("FAIL: set_bit(5) on 0x0F = 0x%08X (expected 0x2F)\n", reg_read());
    }
    total++;

    // Test clear bit
    reg_write(0xFF);
    atomic_clear_bit(3);
    if (reg_read() == 0xF7) {
        printf("PASS: clear_bit(3) on 0xFF = 0xF7\n");
        passed++;
    } else {
        printf("FAIL: clear_bit(3) on 0xFF = 0x%08X (expected 0xF7)\n", reg_read());
    }
    total++;

    // Test toggle bit (0 -> 1)
    reg_write(0x00);
    atomic_toggle_bit(7);
    if (reg_read() == 0x80) {
        printf("PASS: toggle_bit(7) on 0x00 = 0x80\n");
        passed++;
    } else {
        printf("FAIL: toggle_bit(7) on 0x00 = 0x%08X (expected 0x80)\n", reg_read());
    }
    total++;

    // Test toggle bit (1 -> 0)
    reg_write(0xFF);
    atomic_toggle_bit(7);
    if (reg_read() == 0x7F) {
        printf("PASS: toggle_bit(7) on 0xFF = 0x7F\n");
        passed++;
    } else {
        printf("FAIL: toggle_bit(7) on 0xFF = 0x%08X (expected 0x7F)\n", reg_read());
    }
    total++;

    // Test set mask
    reg_write(0x00);
    atomic_set_mask(0x0F0F);
    if (reg_read() == 0x0F0F) {
        printf("PASS: set_mask(0x0F0F) on 0x00 = 0x0F0F\n");
        passed++;
    } else {
        printf("FAIL: set_mask(0x0F0F) on 0x00 = 0x%08X (expected 0x0F0F)\n", reg_read());
    }
    total++;

    // Test clear mask
    reg_write(0xFFFF);
    atomic_clear_mask(0x0F0F);
    if (reg_read() == 0xF0F0) {
        printf("PASS: clear_mask(0x0F0F) on 0xFFFF = 0xF0F0\n");
        passed++;
    } else {
        printf("FAIL: clear_mask(0x0F0F) on 0xFFFF = 0x%08X (expected 0xF0F0)\n", reg_read());
    }
    total++;

    // Test write field
    reg_write(0xFFFFFFFF);
    atomic_write_field(8, 15, 0x42);
    if (reg_read() == 0xFFFF42FF) {
        printf("PASS: write_field(8,15,0x42) on 0xFFFFFFFF = 0xFFFF42FF\n");
        passed++;
    } else {
        printf("FAIL: write_field(8,15,0x42) on 0xFFFFFFFF = 0x%08X (expected 0xFFFF42FF)\n",
               reg_read());
    }
    total++;

    printf("Basic tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_concurrency() {
    printf("--- Concurrency Tests (simulated ISR) ---\n");

    // This test simulates a race condition scenario
    // In a real embedded system, an ISR could fire between read and write

    reg_write(0x00);

    // Test that atomic operations preserve ISR changes
    // We simulate this by having another thread act as the ISR

    int race_detected = 0;
    const int iterations = 1000;

    for (int i = 0; i < iterations; i++) {
        reg_write(0x00);

        // Start ISR simulation in another thread
        std::thread isr_thread([]() {
            // Small delay to increase chance of race
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            simulated_isr();
        });

        // Main thread sets bit 1
        atomic_set_bit(1);

        isr_thread.join();

        // Both bit 0 (from ISR) and bit 1 (from main) should be set
        uint32_t final_val = reg_read();
        if ((final_val & 0x03) != 0x03) {
            race_detected++;
        }
    }

    if (race_detected == 0) {
        printf("PASS: No race conditions detected in %d iterations\n", iterations);
        return true;
    } else {
        printf("FAIL: Race conditions detected in %d/%d iterations\n",
               race_detected, iterations);
        printf("      (This tests that your critical section works correctly)\n");
        return false;
    }
}

int main() {
    printf("Running Q7: Atomic Bit Operations\n");
    printf("==================================\n\n");

    bool all_passed = true;

    if (!test_basic_operations()) all_passed = false;
    if (!test_concurrency()) all_passed = false;

    printf("==================================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Review your implementation.\n");
        return 1;
    }
}
