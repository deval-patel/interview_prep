/**
 * Q2: Find Missing Number with O(1) Space
 *
 * PROBLEM:
 * Given n-1 unique integers from the range [0, n-1] stored in external memory
 * (accessible one at a time), find the single missing number.
 *
 * CONSTRAINTS:
 * - Space: O(1) - you cannot store all values in memory
 * - You can only read each external value once (single pass)
 * - Values are not sorted
 * - Exactly one value from [0, n-1] is missing
 *
 * FOLLOW-UP CHALLENGES:
 * 1. What if TWO numbers are missing? (O(1) space)
 * 2. What if the range is [1, n] instead of [0, n-1]?
 * 3. What if there could be overflow for large n?
 *
 * APPROACH HINTS:
 * - XOR has special properties: a ^ a = 0, a ^ 0 = a
 * - Sum formula: 0 + 1 + 2 + ... + (n-1) = n*(n-1)/2
 * - Consider: what happens if you XOR all values 0 to n-1 with all input values?
 *
 * COMPLEXITY TARGET:
 * - Time: O(n)
 * - Space: O(1)
 */

#include <cstdint>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <random>

// ============ EXTERNAL MEMORY SIMULATION ============

static std::vector<uint32_t> g_external_data;
static size_t g_read_count = 0;

void external_init(const std::vector<uint32_t>& data) {
    g_external_data = data;
    g_read_count = 0;
}

uint64_t external_get_count() {
    return g_external_data.size();
}

// Single-pass constraint: each index should only be read once
bool external_read_next(uint32_t* value) {
    if (g_read_count >= g_external_data.size()) {
        return false;
    }
    *value = g_external_data[g_read_count++];
    return true;
}

void external_reset() {
    g_read_count = 0;
}

// ============ YOUR IMPLEMENTATION ============

/**
 * Find the missing number from range [0, n-1].
 *
 * @param n The range is [0, n-1], and there are n-1 values stored.
 * @return The single missing value.
 *
 * Use external_get_count() to verify count (should be n-1).
 * Use external_read_next(&value) to read the next value.
 * You can only call external_read_next() n-1 times (single pass).
 */
uint32_t find_missing_number(uint32_t n) {
    // TODO: Implement your solution here
    //
    // Remember: You cannot use an array or vector to store values.
    // Only a constant number of variables allowed.
    uint32_t i;

    uint32_t missingNumber = 0;

    // Get count
    uint64_t count = external_get_count();

    if (count != n - 1) {
        return 0;
    }

    // XOR all numbers from 0 to n - 1
    for (i = 0; i < n; i++) {
        missingNumber ^= i;
    }

    uint32_t value;

    // XOR with all values from the external memory
    for (i = 0; i < count; i++) {
        // Duplicate numbers will cancel each other out due to XOR property.
        if (external_read_next(&value)) {
            missingNumber ^= value;
        }
    }

    return missingNumber;  // Placeholder
}

/**
 * FOLLOW-UP: Find TWO missing numbers from range [0, n-1].
 *
 * @param n The range is [0, n-1], and there are n-2 values stored.
 * @param missing1 Output: first missing number (smaller)
 * @param missing2 Output: second missing number (larger)
 *
 * Space: O(1) — a constant number of variables only.
 * Passes: TWO passes are allowed. Call external_reset() between them.
 *         (The XOR partition approach inherently needs two passes:
 *          you can't know the partition bit until after the first pass.)
 */
void find_two_missing_numbers(uint32_t n, uint32_t* missing1, uint32_t* missing2) {
    // TODO: Implement your solution here
    //
    // Hint: XOR alone gives you (missing1 ^ missing2).
    // To separate the two values, find a bit where they differ,
    // then use external_reset() and make a second pass to partition.
    uint32_t i;
    uint32_t missingNumbers = 0;

    // XOR all numbers from 0 to n - 1
    for (i = 0; i < n; i++) {
        missingNumbers ^= i;
    }

    uint32_t value;

    // XOR with all values from the external memory
    for (i = 0; i < n - 2; i++) {
        // Duplicate numbers will cancel each other out due to XOR property.
        if (external_read_next(&value)) {
            missingNumbers ^= value;
        }
    }

    // Now missingNumbers holds a ^ b

    // Isolate the lowest set bit

    uint32_t dividerBit = missingNumbers & (-missingNumbers);

    *missing1 = 0;  // Placeholder
    *missing2 = 0;  // Placeholder

    // Split them into two groups, one with the divider bit set, one without.
    for (i = 0; i < n; i++) {
        if (i & dividerBit)
        {
            *missing1 ^= i;
        }
        else
        {
            *missing2 ^= i;
        }
    }

    // Rewind the stream for the second pass.
    external_reset();

    for (i = 0; i < n - 2; i++) {
        // Duplicate numbers will cancel each other out due to XOR property.
        if (external_read_next(&value)) {
            if (value & dividerBit)
            {
                *missing1 ^= value;
            }
            else
            {
                *missing2 ^= value;
            }
        }
    }
}

// ============ TEST FRAMEWORK ============

bool test_single_missing(const char* name, uint32_t n, uint32_t missing) {
    // Create input without the missing number
    std::vector<uint32_t> input;
    for (uint32_t i = 0; i < n; i++) {
        if (i != missing) {
            input.push_back(i);
        }
    }

    // Shuffle to ensure order doesn't matter
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(input.begin(), input.end(), gen);

    external_init(input);
    uint32_t result = find_missing_number(n);

    if (result == missing) {
        printf("PASS [%s]: Found missing number %u\n", name, result);
        return true;
    } else {
        printf("FAIL [%s]: Expected %u, got %u\n", name, missing, result);
        return false;
    }
}

bool test_two_missing(const char* name, uint32_t n, uint32_t miss1, uint32_t miss2) {
    // Ensure miss1 < miss2
    if (miss1 > miss2) std::swap(miss1, miss2);

    // Create input without the two missing numbers
    std::vector<uint32_t> input;
    for (uint32_t i = 0; i < n; i++) {
        if (i != miss1 && i != miss2) {
            input.push_back(i);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(input.begin(), input.end(), gen);

    external_init(input);

    uint32_t result1, result2;
    find_two_missing_numbers(n, &result1, &result2);

    // Ensure result1 < result2 for comparison
    if (result1 > result2) std::swap(result1, result2);

    if (result1 == miss1 && result2 == miss2) {
        printf("PASS [%s]: Found missing numbers %u and %u\n", name, result1, result2);
        return true;
    } else {
        printf("FAIL [%s]: Expected (%u, %u), got (%u, %u)\n",
               name, miss1, miss2, result1, result2);
        return false;
    }
}

int main() {
    int passed = 0;
    int total = 0;

    printf("Running Q2: Find Missing Number\n");
    printf("================================\n\n");

    printf("--- Single Missing Number Tests ---\n");

    // Test cases for single missing
    if (test_single_missing("Missing 0 from [0,9]", 10, 0)) passed++;
    total++;

    if (test_single_missing("Missing 9 from [0,9]", 10, 9)) passed++;
    total++;

    if (test_single_missing("Missing 5 from [0,9]", 10, 5)) passed++;
    total++;

    if (test_single_missing("Missing 0 from [0,1]", 2, 0)) passed++;
    total++;

    if (test_single_missing("Missing 1 from [0,1]", 2, 1)) passed++;
    total++;

    if (test_single_missing("Single element range", 1, 0)) passed++;
    total++;

    if (test_single_missing("Large range, missing middle", 10000, 5000)) passed++;
    total++;

    if (test_single_missing("Large range, missing first", 10000, 0)) passed++;
    total++;

    if (test_single_missing("Large range, missing last", 10000, 9999)) passed++;
    total++;

    printf("\n--- Two Missing Numbers Tests (Follow-up) ---\n");

    // Test cases for two missing
    if (test_two_missing("Missing 0,1 from [0,9]", 10, 0, 1)) passed++;
    total++;

    if (test_two_missing("Missing 8,9 from [0,9]", 10, 8, 9)) passed++;
    total++;

    if (test_two_missing("Missing 3,7 from [0,9]", 10, 3, 7)) passed++;
    total++;

    if (test_two_missing("Missing 0,9 from [0,9]", 10, 0, 9)) passed++;
    total++;

    if (test_two_missing("Large range, adjacent", 1000, 500, 501)) passed++;
    total++;

    if (test_two_missing("Large range, far apart", 1000, 100, 900)) passed++;
    total++;

    printf("\n================================\n");
    printf("Results: %d/%d tests passed\n", passed, total);

    if (passed == total) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Keep working on your solution.\n");
        return 1;
    }
}
