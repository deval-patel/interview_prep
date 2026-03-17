/*
 * Q11 — Bitwise Data Packing [Medium]
 *
 * Topic: Systems Programming — Bit-Level Manipulation
 *
 * Problem:
 *   Implement a BitPacker class that packs arbitrary-width fields into
 *   a byte buffer. This simulates how TPU instruction words or custom
 *   communication protocols pack multiple fields into a compact format.
 *
 *   Operations:
 *   1. pack(value, num_bits): Write num_bits of value to the bit stream
 *   2. unpack(num_bits): Read num_bits from the bit stream
 *   3. Fields can cross byte boundaries
 *
 *   Example: Pack a "TPU instruction word" with fields:
 *   - opcode: 4 bits
 *   - src_reg: 5 bits
 *   - dst_reg: 5 bits
 *   - immediate: 14 bits
 *   Total: 28 bits across 4 bytes
 *
 * Constraints:
 *   - Fields can be 1-32 bits wide
 *   - Must handle fields crossing byte boundaries correctly
 *   - Bit order: MSB first within each field
 *
 * Key Concepts:
 *   - Bit-level data packing and unpacking
 *   - Cross-byte field insertion/extraction
 *   - Shift and mask operations
 */

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>

using namespace std;

// ============ YOUR IMPLEMENTATION ============

struct BitPacker {
    vector<uint8_t> buffer;
    int bit_position;  // current bit position for writing/reading

    BitPacker() : bit_position(0) {
        buffer.resize(256, 0);  // pre-allocate
    }

    /*
     * pack: Write `num_bits` least-significant bits of `value` into the buffer
     * starting at the current bit_position.
     *
     * Example: pack(0b1101, 4) writes bits 1,1,0,1 starting at bit_position
     *
     * HINT: Process one bit at a time, or work byte-by-byte:
     *   byte_index = bit_position / 8
     *   bit_offset = bit_position % 8
     *   Set/clear the appropriate bit in buffer[byte_index]
     */
    void pack(uint32_t value, int num_bits) {
        // TODO: Implement bit packing
        // For each bit (from MSB to LSB of the field):
        //   1. Extract the bit from value
        //   2. Compute byte_index and bit_offset in buffer
        //   3. Set or clear that bit in buffer
        //   4. Advance bit_position
    }

    /*
     * reset_read: Reset read position to beginning of buffer.
     */
    void reset_read() {
        bit_position = 0;
    }

    /*
     * unpack: Read `num_bits` from the buffer starting at current bit_position.
     * Returns the value as a uint32_t.
     *
     * This is the inverse of pack.
     */
    uint32_t unpack(int num_bits) {
        // TODO: Implement bit unpacking
        // For each bit:
        //   1. Compute byte_index and bit_offset
        //   2. Extract the bit from buffer
        //   3. Shift into result
        //   4. Advance bit_position
        return 0;
    }

    /*
     * get_total_bytes: Return total bytes used by packed data.
     */
    int get_total_bytes() {
        return (bit_position + 7) / 8;
    }
};

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q11: Bitwise Data Packing ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Pack and unpack single field
    {
        total++;
        BitPacker bp;
        bp.pack(0b1101, 4);  // pack 13 in 4 bits
        bp.reset_read();
        uint32_t result = bp.unpack(4);
        bool ok = (result == 0b1101);
        printf("%s [Pack/unpack 4-bit value: got %u, expected %u]\n",
               ok ? "PASS" : "FAIL", result, 0b1101u);
        if (ok) passed++;
    }

    // Test 2: Multiple fields
    {
        total++;
        BitPacker bp;
        bp.pack(0b110, 3);   // 6
        bp.pack(0b10101, 5); // 21
        bp.reset_read();
        uint32_t f1 = bp.unpack(3);
        uint32_t f2 = bp.unpack(5);
        bool ok = (f1 == 6 && f2 == 21);
        printf("%s [Two fields: got (%u, %u), expected (6, 21)]\n",
               ok ? "PASS" : "FAIL", f1, f2);
        if (ok) passed++;
    }

    // Test 3: TPU instruction word
    {
        total++;
        BitPacker bp;
        uint32_t opcode = 0xA;       // 4 bits: 1010
        uint32_t src_reg = 0x15;     // 5 bits: 10101
        uint32_t dst_reg = 0x0B;     // 5 bits: 01011
        uint32_t immediate = 0x1234; // 14 bits: 01 0010 0011 0100

        bp.pack(opcode, 4);
        bp.pack(src_reg, 5);
        bp.pack(dst_reg, 5);
        bp.pack(immediate, 14);

        bp.reset_read();
        uint32_t r_op = bp.unpack(4);
        uint32_t r_src = bp.unpack(5);
        uint32_t r_dst = bp.unpack(5);
        uint32_t r_imm = bp.unpack(14);

        bool ok = (r_op == opcode && r_src == src_reg &&
                   r_dst == dst_reg && r_imm == immediate);
        printf("%s [TPU instruction: op=%X src=%X dst=%X imm=%X]\n",
               ok ? "PASS" : "FAIL", r_op, r_src, r_dst, r_imm);
        if (!ok) {
            printf("  Expected: op=%X src=%X dst=%X imm=%X\n",
                   opcode, src_reg, dst_reg, immediate);
        }
        if (ok) passed++;
    }

    // Test 4: Byte boundary crossing
    {
        total++;
        BitPacker bp;
        bp.pack(0xFF, 8);     // fills byte 0
        bp.pack(0b1010, 4);   // crosses into byte 1
        bp.pack(0b0101, 4);   // fills rest of byte 1
        bp.reset_read();
        uint32_t b1 = bp.unpack(8);
        uint32_t b2 = bp.unpack(4);
        uint32_t b3 = bp.unpack(4);
        bool ok = (b1 == 0xFF && b2 == 0b1010 && b3 == 0b0101);
        printf("%s [Byte boundary: got (%u, %u, %u)]\n",
               ok ? "PASS" : "FAIL", b1, b2, b3);
        if (ok) passed++;
    }

    // Test 5: Large field (32 bits)
    {
        total++;
        BitPacker bp;
        uint32_t big_val = 0xDEADBEEF;
        bp.pack(big_val, 32);
        bp.reset_read();
        uint32_t result = bp.unpack(32);
        bool ok = (result == big_val);
        printf("%s [32-bit field: got 0x%X, expected 0x%X]\n",
               ok ? "PASS" : "FAIL", result, big_val);
        if (ok) passed++;
    }

    // Test 6: Single bits
    {
        total++;
        BitPacker bp;
        bp.pack(1, 1);
        bp.pack(0, 1);
        bp.pack(1, 1);
        bp.pack(1, 1);
        bp.pack(0, 1);
        bp.reset_read();
        uint32_t b1 = bp.unpack(1);
        uint32_t b2 = bp.unpack(1);
        uint32_t b3 = bp.unpack(1);
        uint32_t b4 = bp.unpack(1);
        uint32_t b5 = bp.unpack(1);
        bool ok = (b1==1 && b2==0 && b3==1 && b4==1 && b5==0);
        printf("%s [Single bits: %u%u%u%u%u]\n", ok ? "PASS" : "FAIL",
               b1, b2, b3, b4, b5);
        if (ok) passed++;
    }

    // Test 7: Total bytes used
    {
        total++;
        BitPacker bp;
        bp.pack(0, 28);  // 28 bits = 4 bytes (ceil)
        bool ok = (bp.get_total_bytes() == 4);
        printf("%s [28 bits -> %d bytes, expected 4]\n",
               ok ? "PASS" : "FAIL", bp.get_total_bytes());
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
