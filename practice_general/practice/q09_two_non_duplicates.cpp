/**
 * Q9: Find Two Non-Duplicate Elements
 *
 * PROBLEM:
 * Given an array where every element appears exactly twice except for TWO
 * elements which each appear exactly once, find both unique elements.
 *
 * CONSTRAINTS:
 * - Time: O(n)
 * - Space: O(1) - cannot use hash map or sorting
 * - Must find BOTH unique elements
 *
 * APPROACH:
 * Step 1: XOR all elements
 *         Result = a ^ b (where a and b are the two unique elements)
 *
 * Step 2: Find a bit position where a and b differ
 *         (any set bit in a^b works - use rightmost set bit)
 *
 * Step 3: Partition elements into two groups based on that bit
 *         - XOR all elements with bit set: gives one unique
 *         - XOR all elements with bit clear: gives the other unique
 *
 * WHY THIS WORKS:
 * - If a^b has bit k set, then a and b differ at bit k
 * - One of (a, b) has bit k = 1, the other has bit k = 0
 * - Duplicate pairs always go into the same group (same bit values)
 * - So each group has exactly one unique element
 *
 * COMPLEXITY:
 * - Time: O(n)
 * - Space: O(1)
 */

#include <cstdint>
#include <cstdio>
#include <vector>
#include <algorithm>

// ============ YOUR IMPLEMENTATION ============

/**
 * Find the rightmost set bit in a number.
 *
 * @param n The input number (must be non-zero)
 * @return A mask with only the rightmost set bit
 *
 * Example: rightmost_set_bit(0b1010100) = 0b0000100
 *
 * Hint: n & (-n) or n & ~(n-1)
 */
uint32_t rightmost_set_bit(uint32_t n) {
    // TODO: Implement
    return n & ~(n - 1);
}

/**
 * Find the two elements that appear only once.
 *
 * @param arr    The input array
 * @param size   Number of elements
 * @param result Array of size 2 to store the results (smaller value first)
 */
void find_two_singles(const int* arr, size_t size, int result[2]) {
    // TODO: Implement using the approach described above
    //
    // Step 1: XOR all elements to get a^b
    uint32_t xorALL = 0;
    for (size_t i = 0; i < size; i++) {
        xorALL ^= arr[i];
    }
    //
    // Step 2: Find a bit where a and b differ (rightmost set bit of a^b)
    //
    uint32_t dividerBit = rightmost_set_bit(xorALL);

    // Step 3: Partition and XOR to find a and b separately
    for (size_t i = 0; i < size; i++) {
        if (arr[i] & dividerBit) {
            result[0] ^= arr[i];  // Group with bit set
        } else {
            result[1] ^= arr[i];  // Group with bit clear
        }
    }
}

/**
 * BONUS: Find THREE non-duplicate elements.
 *
 * This is significantly harder and may require a different approach.
 * Consider using the fact that 3 unique XORed together won't be zero,
 * and you need to find two bits that separate them into groups.
 *
 * This is beyond typical interview scope but good for practice.
 */
void find_three_singles(const int* arr, size_t size, int result[3]) {
    // TODO (optional): Implement finding three unique elements

    result[0] = result[1] = result[2] = 0;
}

// ============ TEST FRAMEWORK ============

bool test_two_singles(const char* name, const std::vector<int>& arr,
                      int expected1, int expected2) {
    // Ensure expected1 < expected2
    if (expected1 > expected2) std::swap(expected1, expected2);

    int result[2];
    find_two_singles(arr.data(), arr.size(), result);

    // Sort results for comparison
    if (result[0] > result[1]) std::swap(result[0], result[1]);

    if (result[0] == expected1 && result[1] == expected2) {
        printf("PASS [%s]: Found singles %d and %d\n", name, result[0], result[1]);
        return true;
    } else {
        printf("FAIL [%s]: Expected (%d, %d), got (%d, %d)\n",
               name, expected1, expected2, result[0], result[1]);
        return false;
    }
}

bool test_rightmost(const char* name, uint32_t n, uint32_t expected) {
    uint32_t result = rightmost_set_bit(n);
    if (result == expected) {
        printf("PASS [%s]: rightmost_set_bit(0x%X) = 0x%X\n", name, n, result);
        return true;
    } else {
        printf("FAIL [%s]: rightmost_set_bit(0x%X) = 0x%X, expected 0x%X\n",
               name, n, result, expected);
        return false;
    }
}

int main() {
    printf("Running Q9: Find Two Non-Duplicates\n");
    printf("====================================\n\n");

    int passed = 0;
    int total = 0;

    // --- Rightmost Set Bit Tests ---
    printf("--- Rightmost Set Bit Helper ---\n");

    if (test_rightmost("Power of 2 (0x10)", 0x10, 0x10)) passed++;
    total++;

    if (test_rightmost("All bits (0xFF)", 0xFF, 0x01)) passed++;
    total++;

    if (test_rightmost("Mixed (0b1010100)", 0b1010100, 0b100)) passed++;
    total++;

    if (test_rightmost("Single bit (0x80000000)", 0x80000000, 0x80000000)) passed++;
    total++;

    if (test_rightmost("Alternating (0xAAAAAAAA)", 0xAAAAAAAA, 0x2)) passed++;
    total++;

    // --- Two Singles Tests ---
    printf("\n--- Find Two Singles ---\n");

    if (test_two_singles("Basic: [1,2,3,1,2,4]",
                          {1, 2, 3, 1, 2, 4}, 3, 4)) passed++;
    total++;

    if (test_two_singles("Adjacent values: [1,1,2,3]",
                          {1, 1, 2, 3}, 2, 3)) passed++;
    total++;

    if (test_two_singles("Far apart: [1,5,1,6]",
                          {1, 5, 1, 6}, 5, 6)) passed++;
    total++;

    if (test_two_singles("With zero: [0,1,2,2,1,3]",
                          {0, 1, 2, 2, 1, 3}, 0, 3)) passed++;
    total++;

    if (test_two_singles("Negative: [-1,2,-1,3,2,-5]",
                          {-1, 2, -1, 3, 2, -5}, -5, 3)) passed++;
    total++;

    if (test_two_singles("Both negative: [-1,-2,1,1,-2,-3]",
                          {-1, -2, 1, 1, -2, -3}, -3, -1)) passed++;
    total++;

    if (test_two_singles("Large values: [INT_MAX,1,1,INT_MIN]",
                          {INT32_MAX, 1, 1, INT32_MIN}, INT32_MIN, INT32_MAX)) passed++;
    total++;

    if (test_two_singles("Same low bit: [2,4,6,6,4,8]",
                          {2, 4, 6, 6, 4, 8}, 2, 8)) passed++;
    total++;

    if (test_two_singles("Longer array: [1,2,3,4,5,1,2,3,4,6]",
                          {1, 2, 3, 4, 5, 1, 2, 3, 4, 6}, 5, 6)) passed++;
    total++;

    // Scrambled order shouldn't matter
    if (test_two_singles("Scrambled: [4,1,3,2,1,2,4,5]",
                          {4, 1, 3, 2, 1, 2, 4, 5}, 3, 5)) passed++;
    total++;

    printf("\n====================================\n");
    printf("Results: %d/%d tests passed\n", passed, total);

    if (passed == total) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Keep working on your solution.\n");
        return 1;
    }
}
