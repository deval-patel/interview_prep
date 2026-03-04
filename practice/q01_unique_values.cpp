/**
 * Q1: Unique Values with Extreme Memory Constraint
 *
 * PROBLEM:
 * Given 1 million 32-bit integers stored in external memory, find and print
 * all unique values in sorted order. You have only 512 bytes of RAM available
 * but unlimited disk/external storage.
 *
 * This simulates the constraint from your Google interview but with even
 * tighter memory limits, requiring a multi-pass approach.
 *
 * CONSTRAINTS:
 * - RAM limit: 512 bytes (simulated - you can only use ~128 uint32_t variables)
 * - External memory: unlimited (use provided read/write functions)
 * - Input: Up to 1 million 32-bit unsigned integers
 * - Output: All unique values in ascending order
 *
 * APPROACH HINTS:
 * - Consider processing the value range in chunks
 * - For each chunk, use a bitmap if it fits, or count occurrences
 * - Multiple passes over external data are acceptable
 *
 * COMPLEXITY TARGET:
 * - Time: O(n * k) where k is number of passes
 * - Space: O(1) RAM (excluding external storage)
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cassert>

// ============ EXTERNAL MEMORY SIMULATION ============
// In a real embedded system, these would interface with external flash/EEPROM

static std::vector<uint32_t> g_external_data;
static std::vector<uint32_t> g_output_buffer;

void external_init(const uint32_t* data, size_t count) {
    g_external_data.assign(data, data + count);
    g_output_buffer.clear();
}

uint64_t external_get_count() {
    return g_external_data.size();
}

void external_read(uint64_t index, uint32_t* value) {
    if (index < g_external_data.size()) {
        *value = g_external_data[index];
    }
}

void external_write_output(uint32_t value) {
    g_output_buffer.push_back(value);
}

const std::vector<uint32_t>& get_output() {
    return g_output_buffer;
}

// ============ YOUR IMPLEMENTATION ============

/**
 * Find and output all unique values from external memory.
 *
 * You may only use O(1) RAM - approximately 512 bytes.
 * This means you can have at most ~64 local uint64_t variables,
 * or ~128 uint32_t variables, or one small array.
 *
 * Use external_get_count() to get the number of elements.
 * Use external_read(index, &value) to read element at index.
 * Use external_write_output(value) to output a unique value.
 *
 * Output must be in ascending sorted order.
 */
void find_unique_values() {
    // TODO: Implement your solution here
    //
    // Hint: Process the 32-bit value range in chunks.
    // For example, with 512 bytes you could have a 512*8 = 4096 bit bitmap,
    // covering 4096 consecutive values per pass.
    // Total passes needed: 2^32 / 4096 = ~1 million passes (too many!)
    //
    // Better approach: Use a smaller range per pass but smarter chunking.
    // Consider: what if you first find the min/max values?
    // Or: process in ranges of 65536 values (16-bit chunks)
}

// ============ TEST FRAMEWORK ============

struct TestCase {
    const char* name;
    std::vector<uint32_t> input;
    std::vector<uint32_t> expected;
};

bool run_test(const TestCase& tc) {
    external_init(tc.input.data(), tc.input.size());
    find_unique_values();

    const auto& output = get_output();

    if (output.size() != tc.expected.size()) {
        printf("FAIL [%s]: Expected %zu unique values, got %zu\n",
               tc.name, tc.expected.size(), output.size());
        return false;
    }

    for (size_t i = 0; i < output.size(); i++) {
        if (output[i] != tc.expected[i]) {
            printf("FAIL [%s]: At index %zu, expected %u, got %u\n",
                   tc.name, i, tc.expected[i], output[i]);
            return false;
        }
    }

    printf("PASS [%s]\n", tc.name);
    return true;
}

int main() {
    std::vector<TestCase> tests = {
        {
            "Basic duplicates",
            {1, 2, 3, 2, 1, 4, 3, 5},
            {1, 2, 3, 4, 5}
        },
        {
            "All same",
            {42, 42, 42, 42, 42},
            {42}
        },
        {
            "Already unique sorted",
            {1, 2, 3, 4, 5},
            {1, 2, 3, 4, 5}
        },
        {
            "Reverse order with dups",
            {5, 5, 4, 4, 3, 3, 2, 2, 1, 1},
            {1, 2, 3, 4, 5}
        },
        {
            "Single element",
            {999},
            {999}
        },
        {
            "Large values",
            {0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFE},
            {0x00000000, 0xFFFFFFFE, 0xFFFFFFFF}
        },
        {
            "Sparse values",
            {0, 1000000, 500000, 0, 1000000, 250000},
            {0, 250000, 500000, 1000000}
        },
        {
            "Empty input",
            {},
            {}
        }
    };

    // Generate a larger test case
    {
        TestCase large;
        large.name = "Large dataset (10000 elements)";
        for (int i = 0; i < 10000; i++) {
            large.input.push_back(i % 1000);  // Values 0-999 repeated
        }
        for (int i = 0; i < 1000; i++) {
            large.expected.push_back(i);
        }
        tests.push_back(large);
    }

    int passed = 0;
    int total = tests.size();

    printf("Running Q1: Unique Values with Memory Constraint\n");
    printf("================================================\n\n");

    for (const auto& tc : tests) {
        if (run_test(tc)) {
            passed++;
        }
    }

    printf("\n================================================\n");
    printf("Results: %d/%d tests passed\n", passed, total);

    if (passed == total) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Keep working on your solution.\n");
        return 1;
    }
}
