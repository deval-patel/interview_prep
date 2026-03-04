/**
 * Q10: External Merge Sort with Limited RAM
 *
 * PROBLEM:
 * Sort 1GB of 32-bit integers when you only have 10MB of RAM available.
 *
 * This is a classic embedded systems and big data problem that tests
 * understanding of:
 * - Memory management
 * - I/O efficiency
 * - Merge algorithms
 * - System constraints
 *
 * APPROACH: External Merge Sort
 * 1. Divide: Split input into chunks that fit in RAM
 * 2. Sort: Sort each chunk in memory using quicksort/heapsort
 * 3. Write: Write sorted chunks to temporary files
 * 4. Merge: K-way merge the sorted chunks
 *
 * EXAMPLE:
 * - 1GB = 2^30 bytes = 268,435,456 integers (4 bytes each)
 * - 10MB RAM = ~2,621,440 integers fit in memory
 * - Need ~103 sorted chunks
 * - Merge 103 sorted files using min-heap
 *
 * CONSTRAINTS:
 * - RAM: Simulated limit (configurable)
 * - External storage: Unlimited (simulated with files)
 * - Must handle very large datasets
 *
 * INTERVIEW DISCUSSION POINTS:
 * - How to minimize disk I/O? (Buffering, sequential access)
 * - What's the optimal chunk size? (Fill available RAM)
 * - Time complexity: O(n log n) overall
 * - Space: O(RAM_SIZE) in memory, O(n) on disk
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <queue>
#include <algorithm>
#include <random>
#include <fstream>
#include <filesystem>

// ============ CONFIGURATION ============

// Simulated RAM limit (in number of integers)
// In real scenario: 10MB / 4 bytes = 2,621,440 integers
// For testing: we use smaller values
const size_t SIMULATED_RAM_INTEGERS = 100;  // Small for testing

// Simulated external storage directory
const char* TEMP_DIR = "/tmp/external_sort_test";

// ============ YOUR IMPLEMENTATION ============

/**
 * External sort implementation.
 *
 * @param input_data  Pointer to input array (treat as "external memory")
 * @param count       Number of integers to sort
 * @param ram_limit   Maximum integers that fit in RAM
 * @param output_data Pointer to output array (treat as "external memory")
 *
 * STEPS TO IMPLEMENT:
 * 1. Create sorted chunks:
 *    - Read ram_limit integers at a time
 *    - Sort in memory
 *    - Write to temporary file
 *
 * 2. K-way merge:
 *    - Open all chunk files
 *    - Use min-heap to merge
 *    - Write merged output
 *
 * For simplicity, you can use std::sort for in-memory sorting
 * and focus on the external merge logic.
 */
void external_sort(const uint32_t* input_data, size_t count,
                   size_t ram_limit, uint32_t* output_data) {
    // TODO: Implement external merge sort
    //
    // Hint: Use std::priority_queue for the k-way merge
    // Each element in the heap should track:
    // - Current value
    // - Which chunk it came from
    // - Position in that chunk

}

/**
 * Helper: K-way merge of sorted chunks.
 *
 * @param chunks     Vector of sorted chunks (each chunk is a sorted vector)
 * @param output     Output vector to store merged result
 *
 * This is the core merge step. Implement using a min-heap.
 */
void k_way_merge(const std::vector<std::vector<uint32_t>>& chunks,
                 std::vector<uint32_t>& output) {
    // TODO: Implement k-way merge using min-heap
    //
    // Min-heap element: (value, chunk_index, position_in_chunk)

}

/**
 * BONUS: Implement with actual file I/O.
 *
 * This is closer to real-world external sort.
 * Creates temporary files for chunks and merges them.
 */
void external_sort_with_files(const char* input_file, const char* output_file,
                               size_t ram_limit) {
    // TODO (optional): Implement with real file I/O

}

// ============ TEST FRAMEWORK ============

// Simple in-memory verification
bool verify_sorted(const uint32_t* arr, size_t count) {
    for (size_t i = 1; i < count; i++) {
        if (arr[i] < arr[i-1]) {
            return false;
        }
    }
    return true;
}

bool verify_same_elements(const uint32_t* original, const uint32_t* sorted,
                          size_t count) {
    std::vector<uint32_t> orig_copy(original, original + count);
    std::vector<uint32_t> sort_copy(sorted, sorted + count);
    std::sort(orig_copy.begin(), orig_copy.end());
    std::sort(sort_copy.begin(), sort_copy.end());
    return orig_copy == sort_copy;
}

bool run_test(const char* name, size_t count, size_t ram_limit) {
    // Generate random data
    std::vector<uint32_t> input(count);
    std::vector<uint32_t> output(count);

    std::random_device rd;
    std::mt19937 gen(42);  // Fixed seed for reproducibility
    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    for (size_t i = 0; i < count; i++) {
        input[i] = dist(gen);
    }

    // Run external sort
    external_sort(input.data(), count, ram_limit, output.data());

    // Verify results
    bool is_sorted = verify_sorted(output.data(), count);
    bool same_elements = verify_same_elements(input.data(), output.data(), count);

    if (is_sorted && same_elements) {
        printf("PASS [%s]: Sorted %zu elements with RAM limit %zu\n",
               name, count, ram_limit);
        return true;
    } else {
        printf("FAIL [%s]: ", name);
        if (!is_sorted) printf("Not sorted. ");
        if (!same_elements) printf("Elements changed. ");
        printf("\n");
        return false;
    }
}

bool test_k_way_merge(const char* name,
                      const std::vector<std::vector<uint32_t>>& chunks,
                      const std::vector<uint32_t>& expected) {
    std::vector<uint32_t> output;
    k_way_merge(chunks, output);

    if (output == expected) {
        printf("PASS [%s]\n", name);
        return true;
    } else {
        printf("FAIL [%s]: Output mismatch\n", name);
        printf("  Expected: ");
        for (auto v : expected) printf("%u ", v);
        printf("\n  Got:      ");
        for (auto v : output) printf("%u ", v);
        printf("\n");
        return false;
    }
}

int main() {
    printf("Running Q10: External Merge Sort\n");
    printf("=================================\n\n");

    int passed = 0;
    int total = 0;

    // --- K-way Merge Tests ---
    printf("--- K-way Merge Tests ---\n");

    if (test_k_way_merge("Two chunks",
                          {{1, 3, 5}, {2, 4, 6}},
                          {1, 2, 3, 4, 5, 6})) passed++;
    total++;

    if (test_k_way_merge("Three chunks",
                          {{1, 4, 7}, {2, 5, 8}, {3, 6, 9}},
                          {1, 2, 3, 4, 5, 6, 7, 8, 9})) passed++;
    total++;

    if (test_k_way_merge("Unequal sizes",
                          {{1, 2}, {3}, {4, 5, 6, 7}},
                          {1, 2, 3, 4, 5, 6, 7})) passed++;
    total++;

    if (test_k_way_merge("Single chunk",
                          {{1, 2, 3, 4, 5}},
                          {1, 2, 3, 4, 5})) passed++;
    total++;

    if (test_k_way_merge("Empty chunks",
                          {{}, {1, 2}, {}},
                          {1, 2})) passed++;
    total++;

    if (test_k_way_merge("With duplicates",
                          {{1, 3, 5}, {1, 3, 5}},
                          {1, 1, 3, 3, 5, 5})) passed++;
    total++;

    // --- Full External Sort Tests ---
    printf("\n--- External Sort Tests ---\n");

    // Small tests (RAM can hold all)
    if (run_test("Tiny (RAM fits all)", 10, 100)) passed++;
    total++;

    // Tests requiring actual external sort
    if (run_test("Small external", 100, 20)) passed++;
    total++;

    if (run_test("Medium external", 500, 50)) passed++;
    total++;

    if (run_test("Larger external", 1000, 100)) passed++;
    total++;

    if (run_test("Many chunks", 1000, 25)) passed++;  // 40 chunks
    total++;

    // Edge cases
    if (run_test("Single element", 1, 100)) passed++;
    total++;

    if (run_test("Two elements, one chunk", 2, 100)) passed++;
    total++;

    if (run_test("RAM = 1 (extreme)", 10, 1)) passed++;  // Each chunk is 1 element
    total++;

    printf("\n=================================\n");
    printf("Results: %d/%d tests passed\n", passed, total);

    if (passed == total) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Keep working on your solution.\n");
        return 1;
    }
}
