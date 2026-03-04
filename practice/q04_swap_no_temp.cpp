/**
 * Q4: Swap Two Integers Without Temporary Variable
 *
 * PROBLEM:
 * Implement a function to swap two integers without using a temporary variable.
 *
 * CONSTRAINTS:
 * - Cannot declare any additional variables in swap function
 * - Must handle all edge cases including:
 *   - Same memory location (a == &b)
 *   - Negative numbers
 *   - INT_MIN and INT_MAX values
 *   - Zero values
 *
 * APPROACHES:
 * 1. XOR swap: a ^= b; b ^= a; a ^= b;
 *    - Caveat: Fails if a and b point to same location!
 *
 * 2. Arithmetic swap: a = a + b; b = a - b; a = a - b;
 *    - Caveat: Can overflow for large values
 *
 * INTERVIEW TIP:
 * In practice, using a temp variable is preferred (clearer, no edge cases).
 * This question tests your understanding of XOR properties and edge cases.
 *
 * COMPLEXITY:
 * - Time: O(1)
 * - Space: O(1)
 */

#include <cstdint>
#include <cstdio>
#include <climits>
#include <cassert>

// ============ YOUR IMPLEMENTATION ============

/**
 * Swap two integers using XOR without a temporary variable.
 *
 * IMPORTANT: Must handle the case where a and b point to the same location!
 */
void swap_xor(int* a, int* b) {
    // TODO: Implement XOR swap with edge case handling

}

/**
 * Swap two integers using arithmetic without a temporary variable.
 *
 * Note: This approach can overflow. Document this limitation.
 */
void swap_arithmetic(int* a, int* b) {
    // TODO: Implement arithmetic swap

}

/**
 * BONUS: Implement swap for uint64_t values.
 * Same constraints apply.
 */
void swap_xor_64(uint64_t* a, uint64_t* b) {
    // TODO: Implement

}

// ============ TEST FRAMEWORK ============

typedef void (*SwapFunc)(int*, int*);

bool test_swap(const char* name, SwapFunc swap, int a, int b) {
    int original_a = a;
    int original_b = b;

    swap(&a, &b);

    if (a == original_b && b == original_a) {
        printf("PASS [%s]: (%d, %d) -> (%d, %d)\n", name, original_a, original_b, a, b);
        return true;
    } else {
        printf("FAIL [%s]: (%d, %d) -> expected (%d, %d), got (%d, %d)\n",
               name, original_a, original_b, original_b, original_a, a, b);
        return false;
    }
}

bool test_self_swap(const char* name, SwapFunc swap, int val) {
    int x = val;

    swap(&x, &x);  // Same pointer for both!

    if (x == val) {
        printf("PASS [%s]: self-swap(%d) = %d (unchanged)\n", name, val, x);
        return true;
    } else {
        printf("FAIL [%s]: self-swap(%d) = %d (should be unchanged!)\n", name, val, x);
        return false;
    }
}

int main() {
    printf("Running Q4: Swap Without Temporary Variable\n");
    printf("============================================\n\n");

    int passed = 0;
    int total = 0;

    // ---- XOR Swap Tests ----
    printf("--- XOR Swap Tests ---\n");

    if (test_swap("XOR: Basic positive", swap_xor, 5, 10)) passed++;
    total++;

    if (test_swap("XOR: Both negative", swap_xor, -5, -10)) passed++;
    total++;

    if (test_swap("XOR: Mixed signs", swap_xor, -5, 10)) passed++;
    total++;

    if (test_swap("XOR: With zero (a=0)", swap_xor, 0, 42)) passed++;
    total++;

    if (test_swap("XOR: With zero (b=0)", swap_xor, 42, 0)) passed++;
    total++;

    if (test_swap("XOR: Both zero", swap_xor, 0, 0)) passed++;
    total++;

    if (test_swap("XOR: Same value", swap_xor, 7, 7)) passed++;
    total++;

    if (test_swap("XOR: INT_MAX", swap_xor, INT_MAX, 1)) passed++;
    total++;

    if (test_swap("XOR: INT_MIN", swap_xor, INT_MIN, 1)) passed++;
    total++;

    if (test_swap("XOR: INT_MAX and INT_MIN", swap_xor, INT_MAX, INT_MIN)) passed++;
    total++;

    // Critical edge case: same memory location
    if (test_self_swap("XOR: Self-swap (CRITICAL)", swap_xor, 42)) passed++;
    total++;

    if (test_self_swap("XOR: Self-swap zero", swap_xor, 0)) passed++;
    total++;

    // ---- Arithmetic Swap Tests ----
    printf("\n--- Arithmetic Swap Tests ---\n");

    if (test_swap("Arith: Basic positive", swap_arithmetic, 5, 10)) passed++;
    total++;

    if (test_swap("Arith: Both negative", swap_arithmetic, -5, -10)) passed++;
    total++;

    if (test_swap("Arith: Mixed signs", swap_arithmetic, -5, 10)) passed++;
    total++;

    if (test_swap("Arith: With zero", swap_arithmetic, 0, 42)) passed++;
    total++;

    if (test_swap("Arith: Both zero", swap_arithmetic, 0, 0)) passed++;
    total++;

    if (test_self_swap("Arith: Self-swap", swap_arithmetic, 42)) passed++;
    total++;

    // Note: Arithmetic swap may fail these due to overflow - that's expected
    printf("\n[Note: Arithmetic swap may fail overflow tests - document limitation]\n");

    // Test the 64-bit version
    printf("\n--- 64-bit XOR Swap Tests ---\n");
    {
        uint64_t a = 0xDEADBEEFCAFEBABEULL;
        uint64_t b = 0x1234567890ABCDEFULL;
        uint64_t orig_a = a, orig_b = b;

        swap_xor_64(&a, &b);

        if (a == orig_b && b == orig_a) {
            printf("PASS [64-bit: Large values]\n");
            passed++;
        } else {
            printf("FAIL [64-bit: Large values]\n");
        }
        total++;
    }

    {
        uint64_t x = 999;
        swap_xor_64(&x, &x);
        if (x == 999) {
            printf("PASS [64-bit: Self-swap]\n");
            passed++;
        } else {
            printf("FAIL [64-bit: Self-swap]\n");
        }
        total++;
    }

    printf("\n============================================\n");
    printf("Results: %d/%d tests passed\n", passed, total);

    if (passed == total) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Review edge cases!\n");
        return 1;
    }
}
