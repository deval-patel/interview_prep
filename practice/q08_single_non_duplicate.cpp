/**
 * Q8: Find Single Non-Duplicate Element
 *
 * PROBLEM:
 * Given an array where every element appears exactly twice except for one
 * element which appears exactly once, find that single element.
 *
 * CONSTRAINTS:
 * - Time: O(n)
 * - Space: O(1) - cannot use hash map or sorting
 * - Array is not sorted
 * - Array size is always odd (2*k + 1 elements for some k >= 0)
 *
 * APPROACH:
 * Use XOR properties:
 * - a ^ a = 0 (XOR of same numbers is 0)
 * - a ^ 0 = a (XOR with 0 is identity)
 * - XOR is commutative and associative
 *
 * Therefore: x1 ^ x1 ^ x2 ^ x2 ^ ... ^ xn = xn (the single element)
 *
 * FOLLOW-UP (Q9):
 * What if TWO elements appear once instead of one?
 *
 * COMPLEXITY:
 * - Time: O(n)
 * - Space: O(1)
 */

#include <cstdint>
#include <cstdio>
#include <vector>

// ============ YOUR IMPLEMENTATION ============

/**
 * Find the single element that appears only once.
 * All other elements appear exactly twice.
 *
 * @param arr  The input array
 * @param size Number of elements in the array
 * @return The element that appears only once
 */
int find_single(const int* arr, size_t size) {
    // TODO: Implement using XOR

    return 0;  // Placeholder
}

/**
 * Alternative: Find single element in SORTED array.
 *
 * In a sorted array, duplicates are adjacent. The single element
 * breaks the pattern of pairs starting at even indices.
 *
 * Can you do this in O(log n) time?
 *
 * @param arr  The sorted input array
 * @param size Number of elements
 * @return The element that appears only once
 */
int find_single_sorted(const int* arr, size_t size) {
    // TODO: Implement using binary search
    //
    // In a sorted array with pairs:
    // [1,1,2,2,3,4,4,5,5]
    //  ^   ^   ^ ^
    // Pairs start at even indices until we hit the single element.
    // After the single, pairs start at odd indices.
    //
    // Binary search for the transition point.

    return 0;  // Placeholder
}

// ============ TEST FRAMEWORK ============

bool test_unsorted(const char* name, const std::vector<int>& arr, int expected) {
    int result = find_single(arr.data(), arr.size());
    if (result == expected) {
        printf("PASS [%s]: Found single element %d\n", name, result);
        return true;
    } else {
        printf("FAIL [%s]: Expected %d, got %d\n", name, expected, result);
        return false;
    }
}

bool test_sorted(const char* name, const std::vector<int>& arr, int expected) {
    int result = find_single_sorted(arr.data(), arr.size());
    if (result == expected) {
        printf("PASS [%s]: Found single element %d (binary search)\n", name, result);
        return true;
    } else {
        printf("FAIL [%s]: Expected %d, got %d (binary search)\n", name, expected, result);
        return false;
    }
}

int main() {
    printf("Running Q8: Find Single Non-Duplicate\n");
    printf("=====================================\n\n");

    int passed = 0;
    int total = 0;

    // --- XOR Method (Unsorted) ---
    printf("--- XOR Method (Unsorted Arrays) ---\n");

    if (test_unsorted("Basic: [2,1,2]", {2, 1, 2}, 1)) passed++;
    total++;

    if (test_unsorted("Single element", {42}, 42)) passed++;
    total++;

    if (test_unsorted("At beginning: [1,2,2,3,3]", {1, 2, 2, 3, 3}, 1)) passed++;
    total++;

    if (test_unsorted("At end: [1,1,2,2,3]", {1, 1, 2, 2, 3}, 3)) passed++;
    total++;

    if (test_unsorted("In middle: [1,1,2,3,3]", {1, 1, 2, 3, 3}, 2)) passed++;
    total++;

    if (test_unsorted("Scrambled: [3,1,2,1,3,4,2]",
                       {3, 1, 2, 1, 3, 4, 2}, 4)) passed++;
    total++;

    if (test_unsorted("With negatives: [-1,1,-1,2,2]",
                       {-1, 1, -1, 2, 2}, 1)) passed++;
    total++;

    if (test_unsorted("Zero is single: [1,1,0,2,2]",
                       {1, 1, 0, 2, 2}, 0)) passed++;
    total++;

    if (test_unsorted("Large values: [INT_MAX,1,1]",
                       {INT32_MAX, 1, 1}, INT32_MAX)) passed++;
    total++;

    if (test_unsorted("Negative single: [1,1,-5,2,2]",
                       {1, 1, -5, 2, 2}, -5)) passed++;
    total++;

    // --- Binary Search Method (Sorted) ---
    printf("\n--- Binary Search Method (Sorted Arrays) ---\n");

    if (test_sorted("At beginning: [1,2,2,3,3]", {1, 2, 2, 3, 3}, 1)) passed++;
    total++;

    if (test_sorted("At end: [1,1,2,2,3]", {1, 1, 2, 2, 3}, 3)) passed++;
    total++;

    if (test_sorted("In middle: [1,1,2,3,3]", {1, 1, 2, 3, 3}, 2)) passed++;
    total++;

    if (test_sorted("Single element", {7}, 7)) passed++;
    total++;

    if (test_sorted("Longer array: [0,0,1,1,2,2,3,4,4,5,5]",
                     {0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 5}, 3)) passed++;
    total++;

    if (test_sorted("First element: [1,2,2,3,3,4,4,5,5]",
                     {1, 2, 2, 3, 3, 4, 4, 5, 5}, 1)) passed++;
    total++;

    if (test_sorted("Last element: [1,1,2,2,3,3,4,4,5]",
                     {1, 1, 2, 2, 3, 3, 4, 4, 5}, 5)) passed++;
    total++;

    printf("\n=====================================\n");
    printf("Results: %d/%d tests passed\n", passed, total);

    if (passed == total) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Keep working on your solution.\n");
        return 1;
    }
}
