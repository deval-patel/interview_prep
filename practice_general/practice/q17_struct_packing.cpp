/**
 * Q17: Struct Packing and Alignment
 *
 * PROBLEM:
 * Calculate the size of this struct on a 32-bit system.
 * How would you minimize it?
 *
 *   struct Data {
 *       char a;
 *       int b;
 *       char c;
 *       short d;
 *   };
 *
 * ANSWER:
 * Default (with padding): 12 bytes
 *   char a   [1 byte]
 *   padding  [3 bytes] (align b to 4-byte boundary)
 *   int b    [4 bytes]
 *   char c   [1 byte]
 *   padding  [1 byte]  (align d to 2-byte boundary)
 *   short d  [2 bytes]
 *   Total: 1 + 3 + 4 + 1 + 1 + 2 = 12 bytes
 *
 * Optimized (reordered): 8 bytes
 *   struct DataOptimized {
 *       int b;     [4 bytes]
 *       short d;   [2 bytes]
 *       char a;    [1 byte]
 *       char c;    [1 byte]
 *       Total: 4 + 2 + 1 + 1 = 8 bytes
 *   };
 *
 * ALIGNMENT RULES (typical):
 * - char: 1-byte aligned
 * - short: 2-byte aligned
 * - int/float: 4-byte aligned
 * - double/int64: 8-byte aligned (or 4 on some 32-bit systems)
 * - pointers: sizeof(void*) aligned
 * - struct: aligned to largest member
 *
 * INTERVIEW DISCUSSION:
 * - #pragma pack(1) to disable padding
 * - __attribute__((packed)) (GCC)
 * - Trade-offs: space vs. access speed vs. atomic access
 * - Unaligned access faults on some architectures (ARM without SCTLR.A)
 */

#include <cstdint>
#include <cstdio>
#include <cstddef>

// ============ STRUCT EXAMPLES ============

// Original struct with poor packing
struct Data_Original {
    char a;     // 1 byte
    int b;      // 4 bytes (requires padding before)
    char c;     // 1 byte
    short d;    // 2 bytes (may require padding)
};

// Optimized struct with members reordered
struct Data_Optimized {
    int b;      // 4 bytes (largest first)
    short d;    // 2 bytes
    char a;     // 1 byte
    char c;     // 1 byte (naturally aligned)
};

// Force packed (no padding) - use with caution!
#pragma pack(push, 1)
struct Data_Packed {
    char a;
    int b;
    char c;
    short d;
};
#pragma pack(pop)

// ============ MORE EXAMPLES ============

struct Example1 {
    char a;     // offset 0
    // 3 bytes padding
    int b;      // offset 4
    char c;     // offset 8
    // 3 bytes padding
    int d;      // offset 12
};
// Size: 16 bytes

struct Example1_Optimized {
    int b;      // offset 0
    int d;      // offset 4
    char a;     // offset 8
    char c;     // offset 9
    // 2 bytes padding to align struct to 4
};
// Size: 12 bytes

struct Example2 {
    char a;     // offset 0
    // 7 bytes padding
    double b;   // offset 8
    char c;     // offset 16
    // 7 bytes padding
};
// Size: 24 bytes

struct Example2_Optimized {
    double b;   // offset 0
    char a;     // offset 8
    char c;     // offset 9
    // 6 bytes padding
};
// Size: 16 bytes

// Nested struct alignment
struct Inner {
    char x;
    int y;
};

struct Outer {
    char a;
    Inner inner;  // Inner has alignment of 4
    char b;
};
// Think about the alignment of Outer

// Bit fields for even tighter packing
struct Flags {
    uint8_t enabled : 1;
    uint8_t mode : 3;
    uint8_t priority : 4;
    // All fit in 1 byte!
};

// ============ YOUR IMPLEMENTATION ============

/**
 * Calculate expected size of a struct given member sizes and alignments.
 *
 * This function simulates what the compiler does.
 *
 * @param member_sizes      Array of member sizes in bytes
 * @param member_alignments Array of member alignment requirements
 * @param num_members       Number of members
 * @return Expected struct size with padding
 */
size_t calculate_struct_size(const size_t* member_sizes,
                             const size_t* member_alignments,
                             size_t num_members) {
    // TODO: Implement
    // 1. Track current offset
    // 2. For each member:
    //    a. Add padding to align to member's alignment
    //    b. Add member size
    // 3. Add final padding to align struct to largest member alignment

    return 0;  // Placeholder
}

/**
 * Get offset of each member in a struct.
 *
 * @param member_sizes      Array of member sizes
 * @param member_alignments Array of member alignments
 * @param num_members       Number of members
 * @param offsets           Output: offset of each member
 */
void calculate_offsets(const size_t* member_sizes,
                       const size_t* member_alignments,
                       size_t num_members,
                       size_t* offsets) {
    // TODO: Implement

}

/**
 * Suggest optimal member ordering to minimize struct size.
 *
 * General rule: Order members from largest to smallest alignment.
 *
 * @param member_sizes      Array of member sizes
 * @param member_alignments Array of member alignments
 * @param num_members       Number of members
 * @param optimal_order     Output: indices in optimal order
 */
void suggest_optimal_order(const size_t* member_sizes,
                           const size_t* member_alignments,
                           size_t num_members,
                           size_t* optimal_order) {
    // TODO: Implement
    // Sort indices by alignment (descending), then by size (descending)

}

// ============ TEST FRAMEWORK ============

void print_struct_info(const char* name, size_t actual_size, size_t expected_size) {
    printf("%s:\n", name);
    printf("  Actual size: %zu bytes\n", actual_size);
    printf("  Expected:    %zu bytes\n", expected_size);
    printf("  %s\n\n", actual_size == expected_size ? "PASS" : "FAIL");
}

bool test_struct_sizes() {
    printf("--- Struct Size Tests ---\n\n");
    int passed = 0;
    int total = 0;

    // Test Data_Original
    size_t orig_size = sizeof(Data_Original);
    printf("Data_Original:\n");
    printf("  char a   at offset %zu\n", offsetof(Data_Original, a));
    printf("  int b    at offset %zu\n", offsetof(Data_Original, b));
    printf("  char c   at offset %zu\n", offsetof(Data_Original, c));
    printf("  short d  at offset %zu\n", offsetof(Data_Original, d));
    printf("  Total size: %zu bytes\n", orig_size);
    if (orig_size == 12) { printf("  PASS\n"); passed++; }
    else { printf("  FAIL (expected 12)\n"); }
    total++;
    printf("\n");

    // Test Data_Optimized
    size_t opt_size = sizeof(Data_Optimized);
    printf("Data_Optimized:\n");
    printf("  int b    at offset %zu\n", offsetof(Data_Optimized, b));
    printf("  short d  at offset %zu\n", offsetof(Data_Optimized, d));
    printf("  char a   at offset %zu\n", offsetof(Data_Optimized, a));
    printf("  char c   at offset %zu\n", offsetof(Data_Optimized, c));
    printf("  Total size: %zu bytes\n", opt_size);
    if (opt_size == 8) { printf("  PASS\n"); passed++; }
    else { printf("  FAIL (expected 8)\n"); }
    total++;
    printf("\n");

    // Test Data_Packed
    size_t packed_size = sizeof(Data_Packed);
    printf("Data_Packed (with #pragma pack(1)):\n");
    printf("  Total size: %zu bytes\n", packed_size);
    if (packed_size == 8) { printf("  PASS\n"); passed++; }
    else { printf("  FAIL (expected 8)\n"); }
    total++;
    printf("\n");

    // Test bit fields
    printf("Flags (bit fields):\n");
    printf("  sizeof(Flags) = %zu bytes\n", sizeof(Flags));
    if (sizeof(Flags) == 1) { printf("  PASS\n"); passed++; }
    else { printf("  FAIL (expected 1)\n"); }
    total++;
    printf("\n");

    printf("Struct size tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_calculate_size() {
    printf("--- Calculate Size Function Tests ---\n\n");
    int passed = 0;
    int total = 0;

    // Test: char, int, char, short (like Data_Original)
    {
        size_t sizes[] = {1, 4, 1, 2};
        size_t aligns[] = {1, 4, 1, 2};
        size_t result = calculate_struct_size(sizes, aligns, 4);
        if (result == 12) {
            printf("PASS: char, int, char, short = 12\n");
            passed++;
        } else {
            printf("FAIL: char, int, char, short = %zu (expected 12)\n", result);
        }
        total++;
    }

    // Test: int, short, char, char (optimized)
    {
        size_t sizes[] = {4, 2, 1, 1};
        size_t aligns[] = {4, 2, 1, 1};
        size_t result = calculate_struct_size(sizes, aligns, 4);
        if (result == 8) {
            printf("PASS: int, short, char, char = 8\n");
            passed++;
        } else {
            printf("FAIL: int, short, char, char = %zu (expected 8)\n", result);
        }
        total++;
    }

    // Test: char, double, char
    {
        size_t sizes[] = {1, 8, 1};
        size_t aligns[] = {1, 8, 1};
        size_t result = calculate_struct_size(sizes, aligns, 3);
        if (result == 24) {
            printf("PASS: char, double, char = 24\n");
            passed++;
        } else {
            printf("FAIL: char, double, char = %zu (expected 24)\n", result);
        }
        total++;
    }

    // Test: double, char, char (optimized)
    {
        size_t sizes[] = {8, 1, 1};
        size_t aligns[] = {8, 1, 1};
        size_t result = calculate_struct_size(sizes, aligns, 3);
        if (result == 16) {
            printf("PASS: double, char, char = 16\n");
            passed++;
        } else {
            printf("FAIL: double, char, char = %zu (expected 16)\n", result);
        }
        total++;
    }

    printf("\nCalculate size tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

void print_recommendations() {
    printf("=== Struct Packing Recommendations ===\n\n");

    printf("1. Order members by alignment (largest first):\n");
    printf("   - double/int64_t (8 bytes)\n");
    printf("   - int/float/pointer (4 bytes)\n");
    printf("   - short (2 bytes)\n");
    printf("   - char (1 byte)\n\n");

    printf("2. Group same-sized members together\n\n");

    printf("3. Use bit fields for boolean flags:\n");
    printf("   struct { uint8_t flag1:1; uint8_t flag2:1; };\n\n");

    printf("4. Use #pragma pack(1) sparingly:\n");
    printf("   - Causes unaligned access (slow or illegal)\n");
    printf("   - May cause atomic access issues\n");
    printf("   - Use for wire protocols/file formats only\n\n");

    printf("5. Use static_assert to verify sizes:\n");
    printf("   static_assert(sizeof(MyStruct) == 16, \"Check struct size\");\n\n");

    printf("6. Use offsetof() to check member positions:\n");
    printf("   #include <cstddef>\n");
    printf("   size_t off = offsetof(MyStruct, member);\n\n");
}

int main() {
    printf("Running Q17: Struct Packing\n");
    printf("===========================\n\n");

    printf("System info:\n");
    printf("  sizeof(char) = %zu\n", sizeof(char));
    printf("  sizeof(short) = %zu\n", sizeof(short));
    printf("  sizeof(int) = %zu\n", sizeof(int));
    printf("  sizeof(long) = %zu\n", sizeof(long));
    printf("  sizeof(double) = %zu\n", sizeof(double));
    printf("  sizeof(void*) = %zu\n\n", sizeof(void*));

    bool all_passed = true;

    if (!test_struct_sizes()) all_passed = false;
    if (!test_calculate_size()) all_passed = false;

    print_recommendations();

    printf("===========================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
