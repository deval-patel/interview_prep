"""
Q06 -- BFloat16 Arithmetic [Medium]

Topic: Kernel Optimization -- Low-Precision Arithmetic

Problem:
  Implement BFloat16 (Brain Floating Point) arithmetic operations.
  BFloat16 uses the same exponent range as float32 but with reduced mantissa:
    - 1 sign bit, 8 exponent bits, 7 mantissa bits (vs 23 for float32)
    - Simply truncates/rounds the lower 16 bits of float32

  Implement:
  1. float_to_bf16: Convert float32 to bfloat16 with round-to-nearest-even
  2. bf16_to_float: Convert bfloat16 back to float32
  3. bf16_add, bf16_mul: Arithmetic (convert to float, compute, convert back)
  4. bf16_dot_product: Dot product with float32 accumulation

Constraints:
  - BFloat16 stored as int (uint16 value)
  - Round-to-nearest-even on conversion
  - Dot product must accumulate in float32 for numerical stability

Key Concepts:
  - IEEE 754 floating point layout
  - BFloat16 format (truncated float32)
  - Mixed-precision: compute in low precision, accumulate in high precision
"""

import struct
import math


# bf16 values are represented as plain Python ints (uint16 range: 0..65535)

# ============ YOUR IMPLEMENTATION ============

def float_to_bf16(f):
    """
    float_to_bf16: Convert float32 to bfloat16.

    BFloat16 is the upper 16 bits of float32, with rounding:
      float32: [sign(1)][exponent(8)][mantissa(23)]
      bfloat16: [sign(1)][exponent(8)][mantissa(7)]

    Round-to-nearest-even:
      - Look at bit 16 (round bit) and bits 0-15 (sticky bits) of float32
      - If round bit is 0: truncate (round down)
      - If round bit is 1 and sticky bits != 0: round up
      - If round bit is 1 and sticky bits == 0: round to even (round up if bit 17 is 1)

    Handle special cases: NaN stays NaN, Inf stays Inf

    HINT: Use struct.pack/struct.unpack to get the uint32 bit pattern of f

    Returns: int representing the bf16 value (uint16)
    """
    # TODO: Implement float32 to bfloat16 conversion with round-to-nearest-even
    return 0


def bf16_to_float(b):
    """
    bf16_to_float: Convert bfloat16 back to float32.
    Simply place the 16-bit value in the upper 16 bits of a 32-bit float.

    b: int representing the bf16 value (uint16)
    Returns: float
    """
    # TODO: Implement bfloat16 to float32 conversion
    return 0.0


def bf16_add(a, b):
    """
    bf16_add: Add two bfloat16 values.
    Convert to float32, add, convert back.

    a, b: int (uint16 bf16 values)
    Returns: int (uint16 bf16 value)
    """
    # TODO: Implement
    return 0


def bf16_mul(a, b):
    """
    bf16_mul: Multiply two bfloat16 values.
    Convert to float32, multiply, convert back.

    a, b: int (uint16 bf16 values)
    Returns: int (uint16 bf16 value)
    """
    # TODO: Implement
    return 0


def bf16_dot_product(a, b, N):
    """
    bf16_dot_product: Compute dot product of two bf16 vectors.
    CRITICAL: Accumulate partial sums in float32, not bf16!
    This is the standard "mixed-precision" pattern used in TPUs.

    a, b: lists of int (uint16 bf16 values)
    N: number of elements
    Returns: float (float32 result)
    """
    # TODO: Implement with float32 accumulation
    return 0.0


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q06: BFloat16 Arithmetic ===\n")
    passed = 0
    total = 0

    # Test 1: Simple conversion roundtrip
    total += 1
    vals = [1.0, -1.0, 0.0, 3.14159, 1e10, 1e-10]
    ok = True
    for v in vals:
        b = float_to_bf16(v)
        back = bf16_to_float(b)
        if v != 0.0:
            rel_err = abs(back - v) / abs(v)
            if rel_err > 0.01:  # bf16 has ~0.8% precision
                ok = False
                break
        else:
            if back != 0.0:
                ok = False
                break
    print(f"{'PASS' if ok else 'FAIL'} [Conversion roundtrip]")
    if ok:
        passed += 1

    # Test 2: BF16 is exactly upper 16 bits for powers of 2
    total += 1
    v = 4.0
    b = float_to_bf16(v)
    back = bf16_to_float(b)
    ok = (back == 4.0)
    print(f"{'PASS' if ok else 'FAIL'} [Exact conversion for 4.0]")
    if ok:
        passed += 1

    # Test 3: Addition
    total += 1
    a = float_to_bf16(1.5)
    b = float_to_bf16(2.5)
    c = bf16_add(a, b)
    result = bf16_to_float(c)
    ok = abs(result - 4.0) < 0.1
    print(f"{'PASS' if ok else 'FAIL'} [1.5 + 2.5 = {result:.2f}, expected 4.0]")
    if ok:
        passed += 1

    # Test 4: Multiplication
    total += 1
    a = float_to_bf16(3.0)
    b = float_to_bf16(7.0)
    c = bf16_mul(a, b)
    result = bf16_to_float(c)
    ok = abs(result - 21.0) < 0.5
    print(f"{'PASS' if ok else 'FAIL'} [3.0 * 7.0 = {result:.2f}, expected 21.0]")
    if ok:
        passed += 1

    # Test 5: Dot product with float32 accumulation
    total += 1
    N = 1024
    a_vec = []
    b_vec = []
    expected = 0.0
    for i in range(N):
        fa = float(i + 1) / 100.0
        fb = float(N - i) / 100.0
        a_vec.append(float_to_bf16(fa))
        b_vec.append(float_to_bf16(fb))
        expected += fa * fb
    result = bf16_dot_product(a_vec, b_vec, N)
    rel_err = abs(result - expected) / abs(expected)
    ok = rel_err < 0.02  # allow 2% error from bf16 quantization
    print(f"{'PASS' if ok else 'FAIL'} [Dot product N={N}, result={result:.1f}, expected={expected:.1f}, rel_err={rel_err:.4f}]")
    if ok:
        passed += 1

    # Test 6: Special values
    total += 1
    zero = float_to_bf16(0.0)
    neg = float_to_bf16(-0.0)
    z = bf16_to_float(zero)
    n = bf16_to_float(neg)
    ok = (z == 0.0) and (n == -0.0 or n == 0.0)
    # Also test infinity
    inf_val = float_to_bf16(float('inf'))
    inf_back = bf16_to_float(inf_val)
    ok = ok and math.isinf(inf_back)
    print(f"{'PASS' if ok else 'FAIL'} [Special values (0, -0, inf)]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
