"""
Q11 — Bitwise Data Packing [Medium]

Topic: Systems Programming — Bit-Level Manipulation

Problem:
  Implement a BitPacker class that packs arbitrary-width fields into
  a byte buffer. This simulates how TPU instruction words or custom
  communication protocols pack multiple fields into a compact format.

  Operations:
  1. pack(value, num_bits): Write num_bits of value to the bit stream
  2. unpack(num_bits): Read num_bits from the bit stream
  3. Fields can cross byte boundaries

Constraints:
  - Fields can be 1-32 bits wide
  - Must handle fields crossing byte boundaries correctly
  - Bit order: MSB first within each field

Key Concepts:
  - Bit-level data packing and unpacking
  - Cross-byte field insertion/extraction
  - Shift and mask operations
"""


class BitPacker:
    def __init__(self):
        self.buffer = bytearray(256)
        self.bit_position = 0

    def pack(self, value, num_bits):
        """
        Write num_bits least-significant bits of value into the buffer.
        HINT: Process one bit at a time, MSB first.
        """
        # TODO: Implement bit packing
        pass

    def reset_read(self):
        self.bit_position = 0

    def unpack(self, num_bits):
        """Read num_bits from the buffer, return as integer."""
        # TODO: Implement bit unpacking
        return 0

    def get_total_bytes(self):
        return (self.bit_position + 7) // 8


if __name__ == "__main__":
    print("=== Q11: Bitwise Data Packing ===\n")
    passed = 0
    total = 0

    # Test 1: Pack and unpack single field
    total += 1
    bp = BitPacker()
    bp.pack(0b1101, 4)
    bp.reset_read()
    result = bp.unpack(4)
    ok = result == 0b1101
    print(f"{'PASS' if ok else 'FAIL'} [Pack/unpack 4-bit value: got {result}, expected {0b1101}]")
    if ok: passed += 1

    # Test 2: Multiple fields
    total += 1
    bp = BitPacker()
    bp.pack(0b110, 3)
    bp.pack(0b10101, 5)
    bp.reset_read()
    f1 = bp.unpack(3)
    f2 = bp.unpack(5)
    ok = f1 == 6 and f2 == 21
    print(f"{'PASS' if ok else 'FAIL'} [Two fields: got ({f1}, {f2}), expected (6, 21)]")
    if ok: passed += 1

    # Test 3: TPU instruction word
    total += 1
    bp = BitPacker()
    opcode, src_reg, dst_reg, immediate = 0xA, 0x15, 0x0B, 0x1234
    bp.pack(opcode, 4)
    bp.pack(src_reg, 5)
    bp.pack(dst_reg, 5)
    bp.pack(immediate, 14)
    bp.reset_read()
    r_op, r_src, r_dst, r_imm = bp.unpack(4), bp.unpack(5), bp.unpack(5), bp.unpack(14)
    ok = r_op == opcode and r_src == src_reg and r_dst == dst_reg and r_imm == immediate
    print(f"{'PASS' if ok else 'FAIL'} [TPU instruction: op={r_op:#X} src={r_src:#X} dst={r_dst:#X} imm={r_imm:#X}]")
    if ok: passed += 1

    # Test 4: Byte boundary crossing
    total += 1
    bp = BitPacker()
    bp.pack(0xFF, 8)
    bp.pack(0b1010, 4)
    bp.pack(0b0101, 4)
    bp.reset_read()
    b1, b2, b3 = bp.unpack(8), bp.unpack(4), bp.unpack(4)
    ok = b1 == 0xFF and b2 == 0b1010 and b3 == 0b0101
    print(f"{'PASS' if ok else 'FAIL'} [Byte boundary: got ({b1}, {b2}, {b3})]")
    if ok: passed += 1

    # Test 5: Large field (32 bits)
    total += 1
    bp = BitPacker()
    big_val = 0xDEADBEEF
    bp.pack(big_val, 32)
    bp.reset_read()
    result = bp.unpack(32)
    ok = result == big_val
    print(f"{'PASS' if ok else 'FAIL'} [32-bit field: got {result:#X}, expected {big_val:#X}]")
    if ok: passed += 1

    # Test 6: Single bits
    total += 1
    bp = BitPacker()
    for b in [1, 0, 1, 1, 0]:
        bp.pack(b, 1)
    bp.reset_read()
    bits = [bp.unpack(1) for _ in range(5)]
    ok = bits == [1, 0, 1, 1, 0]
    print(f"{'PASS' if ok else 'FAIL'} [Single bits: {''.join(map(str, bits))}]")
    if ok: passed += 1

    # Test 7: Total bytes used
    total += 1
    bp = BitPacker()
    bp.pack(0, 28)
    ok = bp.get_total_bytes() == 4
    print(f"{'PASS' if ok else 'FAIL'} [28 bits -> {bp.get_total_bytes()} bytes, expected 4]")
    if ok: passed += 1

    print(f"\nPassed {passed}/{total} tests")
    exit(0 if passed == total else 1)
