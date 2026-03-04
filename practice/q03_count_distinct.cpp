/**
 * Q3: Count Distinct Values in Stream (Approximate)
 *
 * PROBLEM:
 * Count the approximate number of distinct 16-bit values in a data stream
 * using only 1KB (1024 bytes) of RAM. The stream may contain millions of values.
 *
 * This is a classic embedded systems problem where exact counting is impossible
 * due to memory constraints, but approximate counting is acceptable.
 *
 * CONSTRAINTS:
 * - RAM: 1024 bytes maximum
 * - Input: Stream of uint16_t values (up to millions)
 * - Output: Approximate count of distinct values
 * - Acceptable error: within 10% of actual count for large datasets
 *
 * APPROACHES TO CONSIDER:
 *
 * 1. Bitmap (exact for 16-bit values):
 *    - 2^16 = 65536 bits = 8192 bytes (too large!)
 *    - But wait... could you use sampling or partitioning?
 *
 * 2. Linear Counting:
 *    - Use a bitmap smaller than needed
 *    - Estimate based on number of empty slots
 *
 * 3. HyperLogLog:
 *    - Use hash functions and track maximum leading zeros
 *    - Very memory efficient for large cardinalities
 *
 * 4. Sampling:
 *    - Sample a subset of values deterministically
 *    - Extrapolate to estimate total distinct count
 *
 * COMPLEXITY TARGET:
 * - Time: O(n) where n is stream length
 * - Space: O(1) - fixed 1KB regardless of input size
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <set>
#include <random>

// ============ STREAM SIMULATION ============

static std::vector<uint16_t> g_stream_data;
static size_t g_stream_pos = 0;

void stream_init(const std::vector<uint16_t>& data) {
    g_stream_data = data;
    g_stream_pos = 0;
}

bool stream_has_next() {
    return g_stream_pos < g_stream_data.size();
}

uint16_t stream_next() {
    return g_stream_data[g_stream_pos++];
}

void stream_reset() {
    g_stream_pos = 0;
}

uint64_t stream_total_count() {
    return g_stream_data.size();
}

// ============ YOUR IMPLEMENTATION ============

/**
 * Count approximate number of distinct values in the stream.
 *
 * You have AT MOST 1024 bytes of memory to work with.
 * This means your solution struct/variables must fit in 1KB.
 *
 * Use stream_has_next() to check if more data exists.
 * Use stream_next() to get the next value.
 *
 * @return Estimated count of distinct values
 */
uint32_t count_distinct_approximate() {
    // TODO: Implement your solution here
    //
    // Example structure for 1KB:
    // - uint8_t bitmap[1024] gives you 8192 bits
    // - This can track 8192 distinct values exactly
    // - For 16-bit range (65536 values), you need a smarter approach
    //
    // Consider:
    // 1. Hash each value to a smaller range that fits in your bitmap
    // 2. Use statistical estimation based on collision rate
    // 3. Or implement HyperLogLog with multiple registers

    return 0;  // Placeholder
}

/**
 * Alternative: Implement HyperLogLog algorithm
 *
 * HyperLogLog uses the observation that the maximum number of leading
 * zeros in hash values relates to the cardinality of the set.
 *
 * With 1KB, you can have 1024 registers (buckets), each storing
 * a maximum leading zero count (0-16 for 16-bit values).
 */
uint32_t count_distinct_hyperloglog() {
    // TODO: Implement HyperLogLog here
    //
    // Steps:
    // 1. For each value, compute a hash
    // 2. Use some bits to select a register (bucket)
    // 3. Use remaining bits to count leading zeros
    // 4. Update register if new count > stored count
    // 5. At end, combine all registers to estimate cardinality

    return 0;  // Placeholder
}

// ============ TEST FRAMEWORK ============

struct TestCase {
    const char* name;
    std::vector<uint16_t> data;
    uint32_t actual_distinct;
    double tolerance;  // Acceptable error ratio (e.g., 0.1 = 10%)
};

bool run_test(const TestCase& tc, uint32_t (*count_func)()) {
    stream_init(tc.data);

    uint32_t estimate = count_func();
    double error = std::abs((double)estimate - tc.actual_distinct) / tc.actual_distinct;

    bool passed = error <= tc.tolerance;

    if (passed) {
        printf("PASS [%s]: Actual=%u, Estimate=%u, Error=%.1f%%\n",
               tc.name, tc.actual_distinct, estimate, error * 100);
    } else {
        printf("FAIL [%s]: Actual=%u, Estimate=%u, Error=%.1f%% (max %.0f%%)\n",
               tc.name, tc.actual_distinct, estimate, error * 100, tc.tolerance * 100);
    }

    return passed;
}

std::vector<uint16_t> generate_stream(uint32_t num_distinct, uint32_t total_count) {
    std::vector<uint16_t> data;
    std::random_device rd;
    std::mt19937 gen(42);  // Fixed seed for reproducibility

    // Generate distinct values
    std::set<uint16_t> distinct_values;
    std::uniform_int_distribution<uint16_t> dist(0, 65535);
    while (distinct_values.size() < num_distinct) {
        distinct_values.insert(dist(gen));
    }

    // Create vector of distinct values for sampling
    std::vector<uint16_t> values(distinct_values.begin(), distinct_values.end());

    // Generate stream with repetitions
    std::uniform_int_distribution<size_t> idx_dist(0, values.size() - 1);
    for (uint32_t i = 0; i < total_count; i++) {
        data.push_back(values[idx_dist(gen)]);
    }

    return data;
}

int main() {
    printf("Running Q3: Count Distinct Values (Approximate)\n");
    printf("================================================\n\n");

    std::vector<TestCase> tests;

    // Small exact tests (tolerance 0% - should be exact for small sets)
    tests.push_back({
        "10 distinct, 100 values",
        generate_stream(10, 100),
        10,
        0.0  // Exact for small counts
    });

    tests.push_back({
        "100 distinct, 1000 values",
        generate_stream(100, 1000),
        100,
        0.05  // 5% tolerance
    });

    tests.push_back({
        "1000 distinct, 10000 values",
        generate_stream(1000, 10000),
        1000,
        0.10  // 10% tolerance
    });

    tests.push_back({
        "5000 distinct, 50000 values",
        generate_stream(5000, 50000),
        5000,
        0.10
    });

    tests.push_back({
        "10000 distinct, 100000 values",
        generate_stream(10000, 100000),
        10000,
        0.15  // 15% tolerance for large counts
    });

    // Edge cases
    tests.push_back({
        "All same value",
        std::vector<uint16_t>(10000, 42),
        1,
        0.0  // Must be exact
    });

    tests.push_back({
        "All unique (8000 values)",
        generate_stream(8000, 8000),  // No repetition
        8000,
        0.10
    });

    // Test with sequential values (might challenge hash-based approaches)
    {
        std::vector<uint16_t> sequential;
        for (uint16_t i = 0; i < 5000; i++) {
            sequential.push_back(i);
            sequential.push_back(i);  // Each appears twice
        }
        tests.push_back({
            "Sequential 0-4999, each twice",
            sequential,
            5000,
            0.10
        });
    }

    int passed = 0;
    int total = tests.size();

    printf("Testing count_distinct_approximate():\n");
    printf("-------------------------------------\n");

    for (const auto& tc : tests) {
        if (run_test(tc, count_distinct_approximate)) {
            passed++;
        }
    }

    printf("\n================================================\n");
    printf("Results: %d/%d tests passed\n", passed, total);

    printf("\n--- Optional: Test HyperLogLog Implementation ---\n");
    printf("(Uncomment the tests below once implemented)\n\n");

    // Uncomment to test HyperLogLog implementation:
    /*
    passed = 0;
    for (const auto& tc : tests) {
        stream_reset();
        stream_init(tc.data);
        if (run_test(tc, count_distinct_hyperloglog)) {
            passed++;
        }
    }
    printf("HyperLogLog Results: %d/%d tests passed\n", passed, total);
    */

    if (passed == total) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Keep working on your solution.\n");
        return 1;
    }
}
