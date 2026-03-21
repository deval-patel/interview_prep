/**
 * Q6: Extract Bit Field from Register
 *
 * PROBLEM:
 * In embedded systems, hardware registers often pack multiple fields into
 * a single word. Implement functions to extract and modify bit fields.
 *
 * Example: A 32-bit status register might have:
 * - Bits [0-7]:   Error code (8 bits)
 * - Bits [8-15]:  Device ID (8 bits)
 * - Bits [16-23]: Status flags (8 bits)
 * - Bits [24-31]: Version (8 bits)
 *
 * CONSTRAINTS:
 * - Bit positions are 0-indexed (LSB is bit 0)
 * - start <= end (start is LSB of field, end is MSB)
 * - 0 <= start <= end <= 31 for 32-bit values
 *
 * INTERVIEW TIP:
 * Interviewers often ask about:
 * - Endianness implications
 * - Signed vs unsigned extraction
 * - Memory-mapped registers and volatile
 * - Atomic read-modify-write concerns
 *
 * COMPLEXITY:
 * - Time: O(1)
 * - Space: O(1)
 */

#include <cstdint>
#include <cstdio>
#include <cassert>

// ============ YOUR IMPLEMENTATION ============

/**
 * Extract bits [start, end] from a 32-bit value.
 *
 * @param value The source value
 * @param start Starting bit position (0 = LSB)
 * @param end   Ending bit position (inclusive)
 * @return The extracted bits, right-aligned (shifted to position 0)
 *
 * Example: extract_bits(0xABCD1234, 4, 11) extracts bits 4-11
 *          which is 0x23, returned as 0x00000023
 */
uint32_t extract_bits(uint32_t value, int start, int end) {
    // TODO: Implement bit extraction
    uint32_t mask = create_mask(start, end);
    return (value & mask) >> start;
}

/**
 * Set a bit field within a value without affecting other bits.
 *
 * @param original The original value
 * @param start    Starting bit position of field
 * @param end      Ending bit position of field (inclusive)
 * @param field    The value to insert (must fit in [end-start+1] bits)
 * @return New value with the bit field replaced
 *
 * Example: set_bits(0xFFFFFFFF, 8, 15, 0x42) -> 0xFFFF42FF
 */
uint32_t set_bits(uint32_t original, int start, int end, uint32_t field) {
    // TODO: Implement bit field setting
    uint32_t mask = create_mask(start, end);
    // Clear the target field in original, then set it with the new field value
    return (original & ~mask) | ((field << start) & mask);
}

/**
 * Toggle bits [start, end] in a value.
 *
 * @param value The source value
 * @param start Starting bit position
 * @param end   Ending bit position (inclusive)
 * @return Value with specified bits toggled
 */
uint32_t toggle_bits(uint32_t value, int start, int end) {
    // TODO: Implement bit toggling
    uint32_t mask = create_mask(start, end);
    return value ^ mask;
}

/**
 * Count set bits (population count / Hamming weight) in a bit field.
 *
 * @param value The source value
 * @param start Starting bit position
 * @param end   Ending bit position (inclusive)
 * @return Number of 1-bits in the specified field
 */
int count_bits_in_field(uint32_t value, int start, int end) {
    // TODO: Implement popcount for field
    uint32_t mask = create_mask(start, end);
    uint32_t field = (value & mask) >> start;
    int count = 0;
    while (field > 0) {
        count ++;
        field &= (field - 1); // Clear the least significant bit set
    }

    return count;
}

/**
 * BONUS: Create a mask for bits [start, end].
 * Useful helper function for the above operations.
 *
 * Example: create_mask(4, 7) -> 0x000000F0
 * 0000 0000 0000 0000 0000 0000 1111 0000
 */
uint32_t create_mask(int start, int end) {
    // TODO: Implement mask creation
    int width = end - start + 1;
    uint32_t mask = (1U << width) - 1;
    return mask << start;
}

// ============ TEST FRAMEWORK ============

bool test_extract(const char* name, uint32_t value, int start, int end, uint32_t expected) {
    uint32_t result = extract_bits(value, start, end);
    if (result == expected) {
        printf("PASS [%s]: extract(0x%08X, %d, %d) = 0x%X\n",
               name, value, start, end, result);
        return true;
    } else {
        printf("FAIL [%s]: extract(0x%08X, %d, %d) = 0x%X, expected 0x%X\n",
               name, value, start, end, result, expected);
        return false;
    }
}

bool test_set(const char* name, uint32_t orig, int start, int end,
              uint32_t field, uint32_t expected) {
    uint32_t result = set_bits(orig, start, end, field);
    if (result == expected) {
        printf("PASS [%s]: set(0x%08X, %d, %d, 0x%X) = 0x%08X\n",
               name, orig, start, end, field, result);
        return true;
    } else {
        printf("FAIL [%s]: set(0x%08X, %d, %d, 0x%X) = 0x%08X, expected 0x%08X\n",
               name, orig, start, end, field, result, expected);
        return false;
    }
}

bool test_toggle(const char* name, uint32_t value, int start, int end, uint32_t expected) {
    uint32_t result = toggle_bits(value, start, end);
    if (result == expected) {
        printf("PASS [%s]: toggle(0x%08X, %d, %d) = 0x%08X\n",
               name, value, start, end, result);
        return true;
    } else {
        printf("FAIL [%s]: toggle(0x%08X, %d, %d) = 0x%08X, expected 0x%08X\n",
               name, value, start, end, result, expected);
        return false;
    }
}

bool test_mask(const char* name, int start, int end, uint32_t expected) {
    uint32_t result = create_mask(start, end);
    if (result == expected) {
        printf("PASS [%s]: mask(%d, %d) = 0x%08X\n", name, start, end, result);
        return true;
    } else {
        printf("FAIL [%s]: mask(%d, %d) = 0x%08X, expected 0x%08X\n",
               name, start, end, result, expected);
        return false;
    }
}

bool test_count(const char* name, uint32_t value, int start, int end, int expected) {
    int result = count_bits_in_field(value, start, end);
    if (result == expected) {
        printf("PASS [%s]: count(0x%08X, %d, %d) = %d\n",
               name, value, start, end, result);
        return true;
    } else {
        printf("FAIL [%s]: count(0x%08X, %d, %d) = %d, expected %d\n",
               name, value, start, end, result, expected);
        return false;
    }
}

int main() {
    printf("Running Q6: Extract Bit Field\n");
    printf("=============================\n\n");

    int passed = 0;
    int total = 0;

    // --- Mask Tests ---
    printf("--- Mask Creation Tests ---\n");

    if (test_mask("Single bit 0", 0, 0, 0x00000001)) passed++;
    total++;

    if (test_mask("Single bit 31", 31, 31, 0x80000000)) passed++;
    total++;

    if (test_mask("Low nibble", 0, 3, 0x0000000F)) passed++;
    total++;

    if (test_mask("Second nibble", 4, 7, 0x000000F0)) passed++;
    total++;

    if (test_mask("Low byte", 0, 7, 0x000000FF)) passed++;
    total++;

    if (test_mask("High byte", 24, 31, 0xFF000000)) passed++;
    total++;

    if (test_mask("Middle two bytes", 8, 23, 0x00FFFF00)) passed++;
    total++;

    if (test_mask("All bits", 0, 31, 0xFFFFFFFF)) passed++;
    total++;

    // --- Extract Tests ---
    printf("\n--- Bit Field Extraction Tests ---\n");

    if (test_extract("Low nibble of 0xABCD1234", 0xABCD1234, 0, 3, 0x4)) passed++;
    total++;

    if (test_extract("Byte 1 of 0xABCD1234", 0xABCD1234, 8, 15, 0x12)) passed++;
    total++;

    if (test_extract("Byte 2 of 0xABCD1234", 0xABCD1234, 16, 23, 0xCD)) passed++;
    total++;

    if (test_extract("High byte of 0xABCD1234", 0xABCD1234, 24, 31, 0xAB)) passed++;
    total++;

    if (test_extract("Bits 4-11 of 0xABCD1234", 0xABCD1234, 4, 11, 0x23)) passed++;
    total++;

    if (test_extract("Single bit (set)", 0x80000000, 31, 31, 1)) passed++;
    total++;

    if (test_extract("Single bit (clear)", 0x7FFFFFFF, 31, 31, 0)) passed++;
    total++;

    if (test_extract("All bits", 0xDEADBEEF, 0, 31, 0xDEADBEEF)) passed++;
    total++;

    // --- Set Tests ---
    printf("\n--- Bit Field Setting Tests ---\n");

    if (test_set("Set byte 1 in 0xFFFFFFFF", 0xFFFFFFFF, 8, 15, 0x42, 0xFFFF42FF)) passed++;
    total++;

    if (test_set("Set low nibble in 0x00000000", 0x00000000, 0, 3, 0xA, 0x0000000A)) passed++;
    total++;

    if (test_set("Set high byte in 0x00000000", 0x00000000, 24, 31, 0xDE, 0xDE000000)) passed++;
    total++;

    if (test_set("Clear byte 2 in 0xFFFFFFFF", 0xFFFFFFFF, 16, 23, 0x00, 0xFF00FFFF)) passed++;
    total++;

    if (test_set("Set single bit 0", 0x00000000, 0, 0, 1, 0x00000001)) passed++;
    total++;

    if (test_set("Clear single bit 31", 0xFFFFFFFF, 31, 31, 0, 0x7FFFFFFF)) passed++;
    total++;

    // --- Toggle Tests ---
    printf("\n--- Bit Toggle Tests ---\n");

    if (test_toggle("Toggle low nibble of 0x0F", 0x0000000F, 0, 3, 0x00000000)) passed++;
    total++;

    if (test_toggle("Toggle low nibble of 0x00", 0x00000000, 0, 3, 0x0000000F)) passed++;
    total++;

    if (test_toggle("Toggle byte 1 of 0xFF00FF00", 0xFF00FF00, 8, 15, 0xFF00FFFF)) passed++;
    total++;

    if (test_toggle("Toggle all of 0xAAAAAAAA", 0xAAAAAAAA, 0, 31, 0x55555555)) passed++;
    total++;

    // --- Count Tests ---
    printf("\n--- Bit Count Tests ---\n");

    if (test_count("Count in 0xFF (bits 0-7)", 0x000000FF, 0, 7, 8)) passed++;
    total++;

    if (test_count("Count in 0x0F (bits 0-3)", 0x0000000F, 0, 3, 4)) passed++;
    total++;

    if (test_count("Count in 0xAAAAAAAA (bits 0-7)", 0xAAAAAAAA, 0, 7, 4)) passed++;
    total++;

    if (test_count("Count in 0 (any range)", 0x00000000, 0, 31, 0)) passed++;
    total++;

    if (test_count("Count in 0xFFFFFFFF (all)", 0xFFFFFFFF, 0, 31, 32)) passed++;
    total++;

    printf("\n=============================\n");
    printf("Results: %d/%d tests passed\n", passed, total);

    if (passed == total) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed. Keep working on your solution.\n");
        return 1;
    }
}
