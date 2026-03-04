/**
 * Q15: Endianness Conversion
 *
 * PROBLEM:
 * Implement functions to convert between big-endian and little-endian
 * for 16-bit, 32-bit, and 64-bit values.
 *
 * BACKGROUND:
 * - Little-endian: Least significant byte stored at lowest address (x86, ARM)
 * - Big-endian: Most significant byte stored at lowest address (network byte order)
 *
 * EXAMPLE (32-bit value 0x12345678):
 * - Little-endian memory: [78] [56] [34] [12] (addresses 0, 1, 2, 3)
 * - Big-endian memory:    [12] [34] [56] [78] (addresses 0, 1, 2, 3)
 *
 * REAL-WORLD USES:
 * - Network protocols (TCP/IP uses big-endian)
 * - File formats (PNG uses big-endian, BMP uses little-endian)
 * - Cross-platform data exchange
 * - Hardware register access on different architectures
 *
 * INTERVIEW TIP:
 * Also discuss:
 * - How to detect system endianness at runtime
 * - Compiler intrinsics (__builtin_bswap32)
 * - Standard library functions (htonl, ntohl)
 * - Unaligned access considerations
 */

#include <cstdint>
#include <cstdio>
#include <cstring>

// ============ YOUR IMPLEMENTATION ============

/**
 * Detect if the current system is little-endian.
 */
bool is_little_endian() {
    // TODO: Implement runtime detection
    // Hint: Store a multi-byte value and check first byte

    return true;  // Placeholder
}

/**
 * Swap bytes in a 16-bit value.
 *
 * Example: 0x1234 -> 0x3412
 */
uint16_t swap_bytes_16(uint16_t value) {
    // TODO: Implement byte swap

    return 0;  // Placeholder
}

/**
 * Swap bytes in a 32-bit value.
 *
 * Example: 0x12345678 -> 0x78563412
 */
uint32_t swap_bytes_32(uint32_t value) {
    // TODO: Implement byte swap

    return 0;  // Placeholder
}

/**
 * Swap bytes in a 64-bit value.
 *
 * Example: 0x0123456789ABCDEF -> 0xEFCDAB8967452301
 */
uint64_t swap_bytes_64(uint64_t value) {
    // TODO: Implement byte swap

    return 0;  // Placeholder
}

/**
 * Convert from host byte order to big-endian (network byte order).
 */
uint16_t host_to_be16(uint16_t value) {
    // TODO: Implement
    // If host is big-endian, no conversion needed
    // If host is little-endian, swap bytes

    return 0;  // Placeholder
}

uint32_t host_to_be32(uint32_t value) {
    // TODO: Implement
    return 0;  // Placeholder
}

uint64_t host_to_be64(uint64_t value) {
    // TODO: Implement
    return 0;  // Placeholder
}

/**
 * Convert from big-endian (network byte order) to host byte order.
 */
uint16_t be16_to_host(uint16_t value) {
    // Same as host_to_be16 (swapping is symmetric)
    return host_to_be16(value);
}

uint32_t be32_to_host(uint32_t value) {
    return host_to_be32(value);
}

uint64_t be64_to_host(uint64_t value) {
    return host_to_be64(value);
}

/**
 * Convert from host byte order to little-endian.
 */
uint16_t host_to_le16(uint16_t value) {
    // TODO: Implement
    return 0;  // Placeholder
}

uint32_t host_to_le32(uint32_t value) {
    // TODO: Implement
    return 0;  // Placeholder
}

uint64_t host_to_le64(uint64_t value) {
    // TODO: Implement
    return 0;  // Placeholder
}

/**
 * Read a big-endian 32-bit value from a byte array.
 *
 * This handles unaligned access safely.
 */
uint32_t read_be32(const uint8_t* data) {
    // TODO: Implement
    // Read bytes individually to avoid unaligned access issues

    return 0;  // Placeholder
}

/**
 * Write a 32-bit value as big-endian to a byte array.
 */
void write_be32(uint8_t* data, uint32_t value) {
    // TODO: Implement

}

/**
 * Read a little-endian 32-bit value from a byte array.
 */
uint32_t read_le32(const uint8_t* data) {
    // TODO: Implement
    return 0;  // Placeholder
}

/**
 * Write a 32-bit value as little-endian to a byte array.
 */
void write_le32(uint8_t* data, uint32_t value) {
    // TODO: Implement

}

// ============ TEST FRAMEWORK ============

bool test_byte_swap() {
    printf("--- Byte Swap Tests ---\n");
    int passed = 0;
    int total = 0;

    // 16-bit swaps
    if (swap_bytes_16(0x1234) == 0x3412) {
        printf("PASS: swap_bytes_16(0x1234) = 0x3412\n");
        passed++;
    } else {
        printf("FAIL: swap_bytes_16(0x1234) = 0x%04X\n", swap_bytes_16(0x1234));
    }
    total++;

    if (swap_bytes_16(0x0000) == 0x0000) {
        printf("PASS: swap_bytes_16(0x0000) = 0x0000\n");
        passed++;
    } else {
        printf("FAIL: swap_bytes_16(0x0000)\n");
    }
    total++;

    if (swap_bytes_16(0xFFFF) == 0xFFFF) {
        printf("PASS: swap_bytes_16(0xFFFF) = 0xFFFF\n");
        passed++;
    } else {
        printf("FAIL: swap_bytes_16(0xFFFF)\n");
    }
    total++;

    // 32-bit swaps
    if (swap_bytes_32(0x12345678) == 0x78563412) {
        printf("PASS: swap_bytes_32(0x12345678) = 0x78563412\n");
        passed++;
    } else {
        printf("FAIL: swap_bytes_32(0x12345678) = 0x%08X\n", swap_bytes_32(0x12345678));
    }
    total++;

    if (swap_bytes_32(0x00000000) == 0x00000000) {
        printf("PASS: swap_bytes_32(0x00000000)\n");
        passed++;
    } else {
        printf("FAIL: swap_bytes_32(0x00000000)\n");
    }
    total++;

    if (swap_bytes_32(0xDEADBEEF) == 0xEFBEADDE) {
        printf("PASS: swap_bytes_32(0xDEADBEEF) = 0xEFBEADDE\n");
        passed++;
    } else {
        printf("FAIL: swap_bytes_32(0xDEADBEEF) = 0x%08X\n", swap_bytes_32(0xDEADBEEF));
    }
    total++;

    // 64-bit swaps
    if (swap_bytes_64(0x0123456789ABCDEFULL) == 0xEFCDAB8967452301ULL) {
        printf("PASS: swap_bytes_64(0x0123456789ABCDEF)\n");
        passed++;
    } else {
        printf("FAIL: swap_bytes_64(0x0123456789ABCDEF) = 0x%016llX\n",
               (unsigned long long)swap_bytes_64(0x0123456789ABCDEFULL));
    }
    total++;

    // Double swap should return original
    uint32_t original = 0xCAFEBABE;
    if (swap_bytes_32(swap_bytes_32(original)) == original) {
        printf("PASS: Double swap returns original\n");
        passed++;
    } else {
        printf("FAIL: Double swap\n");
    }
    total++;

    printf("Byte swap tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_endian_detection() {
    printf("--- Endian Detection ---\n");

    bool is_le = is_little_endian();

    // Verify using a known method
    uint32_t test_val = 0x01020304;
    uint8_t* bytes = (uint8_t*)&test_val;
    bool actual_le = (bytes[0] == 0x04);

    if (is_le == actual_le) {
        printf("PASS: Correctly detected %s-endian system\n",
               is_le ? "little" : "big");
        return true;
    } else {
        printf("FAIL: Endian detection incorrect\n");
        return false;
    }
}

bool test_byte_array_access() {
    printf("\n--- Byte Array Access ---\n");
    int passed = 0;
    int total = 0;

    // Test read_be32
    uint8_t be_data[] = {0x12, 0x34, 0x56, 0x78};
    if (read_be32(be_data) == 0x12345678) {
        printf("PASS: read_be32([12,34,56,78]) = 0x12345678\n");
        passed++;
    } else {
        printf("FAIL: read_be32([12,34,56,78]) = 0x%08X\n", read_be32(be_data));
    }
    total++;

    // Test read_le32
    uint8_t le_data[] = {0x78, 0x56, 0x34, 0x12};
    if (read_le32(le_data) == 0x12345678) {
        printf("PASS: read_le32([78,56,34,12]) = 0x12345678\n");
        passed++;
    } else {
        printf("FAIL: read_le32([78,56,34,12]) = 0x%08X\n", read_le32(le_data));
    }
    total++;

    // Test write_be32
    uint8_t out_be[4] = {0};
    write_be32(out_be, 0x12345678);
    if (out_be[0] == 0x12 && out_be[1] == 0x34 &&
        out_be[2] == 0x56 && out_be[3] == 0x78) {
        printf("PASS: write_be32(0x12345678) = [12,34,56,78]\n");
        passed++;
    } else {
        printf("FAIL: write_be32(0x12345678) = [%02X,%02X,%02X,%02X]\n",
               out_be[0], out_be[1], out_be[2], out_be[3]);
    }
    total++;

    // Test write_le32
    uint8_t out_le[4] = {0};
    write_le32(out_le, 0x12345678);
    if (out_le[0] == 0x78 && out_le[1] == 0x56 &&
        out_le[2] == 0x34 && out_le[3] == 0x12) {
        printf("PASS: write_le32(0x12345678) = [78,56,34,12]\n");
        passed++;
    } else {
        printf("FAIL: write_le32(0x12345678) = [%02X,%02X,%02X,%02X]\n",
               out_le[0], out_le[1], out_le[2], out_le[3]);
    }
    total++;

    // Test round-trip
    uint8_t buffer[4];
    uint32_t original = 0xDEADBEEF;
    write_be32(buffer, original);
    uint32_t read_back = read_be32(buffer);
    if (read_back == original) {
        printf("PASS: BE round-trip preserves value\n");
        passed++;
    } else {
        printf("FAIL: BE round-trip\n");
    }
    total++;

    write_le32(buffer, original);
    read_back = read_le32(buffer);
    if (read_back == original) {
        printf("PASS: LE round-trip preserves value\n");
        passed++;
    } else {
        printf("FAIL: LE round-trip\n");
    }
    total++;

    printf("Byte array tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_host_conversions() {
    printf("--- Host Conversion Tests ---\n");
    int passed = 0;
    int total = 0;

    // host_to_be and back should be identity
    uint32_t original = 0x12345678;
    uint32_t converted = be32_to_host(host_to_be32(original));

    if (converted == original) {
        printf("PASS: host -> BE -> host round-trip\n");
        passed++;
    } else {
        printf("FAIL: host -> BE -> host = 0x%08X\n", converted);
    }
    total++;

    // Same for LE
    converted = host_to_le32(host_to_le32(original));  // Double convert
    // Note: This is NOT identity! host_to_le on LE system is identity,
    // but on BE system it swaps twice = identity

    // Test that BE conversion produces correct byte order
    uint32_t be_val = host_to_be32(0x12345678);
    uint8_t* bytes = (uint8_t*)&be_val;

    // In big-endian representation, MSB should be first
    // But we're reading in host order, so check the value directly

    // On little-endian host: host_to_be32(0x12345678) = 0x78563412
    // On big-endian host: host_to_be32(0x12345678) = 0x12345678

    printf("PASS: Host conversion functions implemented\n");
    passed++;
    total++;

    printf("Host conversion tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

int main() {
    printf("Running Q15: Endianness Conversion\n");
    printf("===================================\n\n");

    printf("System info:\n");
    printf("  sizeof(short) = %zu\n", sizeof(short));
    printf("  sizeof(int) = %zu\n", sizeof(int));
    printf("  sizeof(long) = %zu\n", sizeof(long));
    printf("  sizeof(long long) = %zu\n\n", sizeof(long long));

    bool all_passed = true;

    if (!test_endian_detection()) all_passed = false;
    if (!test_byte_swap()) all_passed = false;
    if (!test_byte_array_access()) all_passed = false;
    if (!test_host_conversions()) all_passed = false;

    printf("===================================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
