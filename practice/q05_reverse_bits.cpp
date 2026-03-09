/**
 * Q5: Reverse Bits in a 32-bit Integer
 *
 * PROBLEM:
 * Reverse all bits in a 32-bit unsigned integer.
 * Example: 0b00000010100101000001111010011100 (43261596)
 *       -> 0b00111001011110000010100101000000 (964176192)
 *
 * CONSTRAINTS:
 * - Input is uint32_t (unsigned 32-bit)
 * - Output must be exact bit reversal
 *
 * FOLLOW-UP (asked at Google):
 * - If this function is called many times, how would you optimize it?
 *   (Hint: Look-up table, cache-friendly access patterns)
 *
 * APPROACHES:
 *
 * 1. Naive: Loop through each bit (O(32) = O(1))
 *
 * 2. Divide and conquer: Swap adjacent bits, then pairs, then nibbles, etc.
 *    - Very efficient, no loops needed
 *
 * 3. Lookup table: Precompute reversal for bytes, combine 4 bytes
 *    - O(1) with O(256) space
 *    - Best for repeated calls
 *
 * COMPLEXITY:
 * - Time: O(1) - fixed 32 bits
 * - Space: O(1) for approaches 1-2, O(256) for lookup table
 */

#include <cstdint>
#include <cstdio>
#include <cstring>

// ============ YOUR IMPLEMENTATION ============

/**
 * Reverse bits using the naive bit-by-bit approach.
 */
uint32_t reverse_bits_naive(uint32_t n) {
    // TODO: Implement naive approach

    return 0;  // Placeholder
}

/**
 * Reverse bits using divide-and-conquer (parallel bit swap).
 *
 * The idea is to swap adjacent single bits, then adjacent pairs,
 * then adjacent nibbles (4 bits), then bytes, then 16-bit halves.
 *
 * Example masks for 8-bit (extend to 32-bit):
 * - Swap bits:    n = ((n & 0x55) << 1) | ((n & 0xAA) >> 1)
 * - Swap pairs:   n = ((n & 0x33) << 2) | ((n & 0xCC) >> 2)
 * - Swap nibbles: n = ((n & 0x0F) << 4) | ((n & 0xF0) >> 4)
 */
uint32_t reverse_bits_divide_conquer(uint32_t n) {
    // TODO: Implement divide-and-conquer approach

    return 0;  // Placeholder
}

/**
 * FOLLOW-UP: Reverse bits using a lookup table.
 *
 * Precompute the bit reversal for all possible byte values (0-255).
 * Then reverse a 32-bit integer by:
 * 1. Split into 4 bytes
 * 2. Look up the reversal of each byte
 * 3. Recombine in reverse order
 *
 * This is optimal for repeated calls.
 */

// Static lookup table - initialized once (used in reverse_bits_lookup)
[[maybe_unused]] static uint8_t g_reverse_table[256];
static bool g_table_initialized = false;

void init_reverse_table() {
    // TODO: Initialize the lookup table
    // g_reverse_table[i] should contain the bit-reversed value of i

    g_table_initialized = true;
}

uint32_t reverse_bits_lookup(uint32_t n) {
    if (!g_table_initialized) {
        init_reverse_table();
    }

    // TODO: Use the lookup table to reverse the 32-bit value

    return 0;  // Placeholder
}

// ============ TEST FRAMEWORK ============

typedef uint32_t (*ReverseBitsFunc)(uint32_t);

bool test_reverse(const char* name, ReverseBitsFunc func, uint32_t input, uint32_t expected) {
    uint32_t result = func(input);

    if (result == expected) {
        printf("PASS [%s]: 0x%08X -> 0x%08X\n", name, input, result);
        return true;
    } else {
        printf("FAIL [%s]: 0x%08X -> expected 0x%08X, got 0x%08X\n",
               name, input, expected, result);
        return false;
    }
}

void print_binary(uint32_t n) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (n >> i) & 1);
        if (i % 8 == 0 && i > 0) printf(" ");
    }
}

int main() {
    printf("Running Q5: Reverse Bits\n");
    printf("========================\n\n");

    // Test vectors
    struct TestCase {
        uint32_t input;
        uint32_t expected;
    };

    TestCase tests[] = {
        {0x00000000, 0x00000000},  // All zeros
        {0xFFFFFFFF, 0xFFFFFFFF},  // All ones
        {0x00000001, 0x80000000},  // Single bit at LSB
        {0x80000000, 0x00000001},  // Single bit at MSB
        {0x0000000F, 0xF0000000},  // Low nibble
        {0xF0000000, 0x0000000F},  // High nibble
        {0x12345678, 0x1E6A2C48},  // Random pattern 1
        {0xDEADBEEF, 0xF77DB57B},  // Random pattern 2
        {43261596,   964176192},   // Example from problem
        {0x55555555, 0xAAAAAAAA},  // Alternating bits
        {0xAAAAAAAA, 0x55555555},  // Alternating bits (inverse)
        {0x0F0F0F0F, 0xF0F0F0F0},  // Alternating nibbles
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0;
    int total = 0;

    // Show example of bit reversal
    printf("Example bit reversal visualization:\n");
    printf("Input:  "); print_binary(43261596); printf(" (%u)\n", 43261596);
    printf("Output: "); print_binary(964176192); printf(" (%u)\n\n", 964176192);

    // Test naive approach
    printf("--- Naive Approach ---\n");
    for (int i = 0; i < num_tests; i++) {
        char name[64];
        snprintf(name, sizeof(name), "Naive #%d", i + 1);
        if (test_reverse(name, reverse_bits_naive, tests[i].input, tests[i].expected)) {
            passed++;
        }
        total++;
    }

    // Test divide-and-conquer approach
    printf("\n--- Divide and Conquer Approach ---\n");
    for (int i = 0; i < num_tests; i++) {
        char name[64];
        snprintf(name, sizeof(name), "D&C #%d", i + 1);
        if (test_reverse(name, reverse_bits_divide_conquer, tests[i].input, tests[i].expected)) {
            passed++;
        }
        total++;
    }

    // Test lookup table approach
    printf("\n--- Lookup Table Approach ---\n");
    for (int i = 0; i < num_tests; i++) {
        char name[64];
        snprintf(name, sizeof(name), "Lookup #%d", i + 1);
        if (test_reverse(name, reverse_bits_lookup, tests[i].input, tests[i].expected)) {
            passed++;
        }
        total++;
    }

    // Verify double reversal returns original
    printf("\n--- Double Reversal Tests ---\n");
    uint32_t test_values[] = {0, 1, 42, 0xDEADBEEF, 0xCAFEBABE, UINT32_MAX};
    for (uint32_t val : test_values) {
        uint32_t reversed = reverse_bits_divide_conquer(val);
        uint32_t double_reversed = reverse_bits_divide_conquer(reversed);
        if (double_reversed == val) {
            printf("PASS: Double reversal of 0x%08X\n", val);
            passed++;
        } else {
            printf("FAIL: Double reversal of 0x%08X gave 0x%08X\n", val, double_reversed);
        }
        total++;
    }

    printf("\n========================\n");
    printf("Results: %d/%d tests passed\n", passed, total);

    if (passed == total) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Keep working on your solution.\n");
        return 1;
    }
}
