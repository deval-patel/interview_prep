"""
Q03 — SIMD Vector Operations [Easy-Medium]

Topic: Hardware Architecture — SIMD

Problem:
  Implement vectorized operations using a simulated SIMD register abstraction.
  The SIMD "hardware" processes VECTOR_WIDTH elements in parallel.

  Operations to implement:
  1. Vector Addition: C[i] = A[i] + B[i]
  2. Dot Product: sum(A[i] * B[i])
  3. ReLU: out[i] = max(0, in[i])
  4. Fused Multiply-Add: out[i] = A[i] * B[i] + C[i]

  All operations must handle the case where N % VECTOR_WIDTH != 0 (tail elements).

Constraints:
  - VECTOR_WIDTH = 8 (simulates 256-bit SIMD with float32)
  - Must use the provided SIMD primitives
  - Scalar fallback required for tail elements

Key Concepts:
  - Strip-mining (processing VECTOR_WIDTH elements per iteration)
  - Tail handling (remaining elements < VECTOR_WIDTH)
  - SIMD register abstraction
"""

VECTOR_WIDTH = 8


class SimdReg:
    """Simulated SIMD register."""
    def __init__(self):
        self.data = [0.0] * VECTOR_WIDTH


# ---- Simulated SIMD intrinsics ----

def simd_load(ptr, offset):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = ptr[offset + i]
    return r

def simd_store(ptr, offset, r):
    for i in range(VECTOR_WIDTH):
        ptr[offset + i] = r.data[i]

def simd_add(a, b):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = a.data[i] + b.data[i]
    return r

def simd_mul(a, b):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = a.data[i] * b.data[i]
    return r

def simd_max(a, b):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = max(a.data[i], b.data[i])
    return r

def simd_set1(val):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = val
    return r

def simd_setzero():
    return simd_set1(0.0)

def simd_hsum(r):
    return sum(r.data)

def simd_fmadd(a, b, c):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = a.data[i] * b.data[i] + c.data[i]
    return r


# ============ YOUR IMPLEMENTATION ============

def simd_vector_add(A, B, C, N):
    """C[i] = A[i] + B[i] for i in [0, N). Use strip-mining with tail handling."""
    # TODO: Implement using simd_load, simd_add, simd_store
    pass

def simd_dot_product(A, B, N):
    """Return sum(A[i] * B[i]) for i in [0, N)."""
    # TODO: Implement using simd_load, simd_mul, simd_add, simd_hsum
    return 0.0

def simd_relu(inp, out, N):
    """out[i] = max(0, inp[i]) for i in [0, N)."""
    # TODO: Implement using simd_load, simd_max, simd_set1(0), simd_store
    pass

def simd_fma(A, B, C, out, N):
    """out[i] = A[i] * B[i] + C[i] for i in [0, N)."""
    # TODO: Implement using simd_load, simd_fmadd, simd_store
    pass


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q03: SIMD Vector Operations ===\n")
    passed = 0
    total = 0

    N = 37  # 37 = 4*8 + 5 (5 tail elements)
    A = [float(i + 1) for i in range(N)]
    B = [float(N - i) for i in range(N)]

    # Test 1: Vector Add
    total += 1
    expected = [A[i] + B[i] for i in range(N)]
    C_arr = [0.0] * N
    simd_vector_add(A, B, C_arr, N)
    ok = all(abs(C_arr[i] - expected[i]) < 1e-5 for i in range(N))
    print(f"{'PASS' if ok else 'FAIL'} [Vector Add, N={N}]")
    if ok: passed += 1

    # Test 2: Dot Product
    total += 1
    exp_dot = sum(A[i] * B[i] for i in range(N))
    result = simd_dot_product(A, B, N)
    ok = abs(result - exp_dot) < 1e-2
    print(f"{'PASS' if ok else 'FAIL'} [Dot Product = {result:.1f}, expected {exp_dot:.1f}]")
    if ok: passed += 1

    # Test 3: ReLU
    total += 1
    inp = [float(i - N // 2) for i in range(N)]
    exp_relu = [max(0.0, x) for x in inp]
    output = [0.0] * N
    simd_relu(inp, output, N)
    ok = all(abs(output[i] - exp_relu[i]) < 1e-5 for i in range(N))
    print(f"{'PASS' if ok else 'FAIL'} [ReLU, N={N}]")
    if ok: passed += 1

    # Test 4: FMA
    total += 1
    fma_c = [i * 0.5 for i in range(N)]
    fma_exp = [A[i] * B[i] + fma_c[i] for i in range(N)]
    fma_out = [0.0] * N
    simd_fma(A, B, fma_c, fma_out, N)
    ok = all(abs(fma_out[i] - fma_exp[i]) < 1e-3 for i in range(N))
    print(f"{'PASS' if ok else 'FAIL'} [FMA, N={N}]")
    if ok: passed += 1

    # Test 5: Edge case N = 1
    total += 1
    a = [3.0]
    b = [4.0]
    c = [0.0]
    simd_vector_add(a, b, c, 1)
    ok = abs(c[0] - 7.0) < 1e-5
    print(f"{'PASS' if ok else 'FAIL'} [Vector Add, N=1]")
    if ok: passed += 1

    # Test 6: Exact SIMD width
    total += 1
    N2 = VECTOR_WIDTH
    a2 = [float(i) for i in range(N2)]
    b2 = [float(i * 2) for i in range(N2)]
    c2 = [0.0] * N2
    e2 = [a2[i] + b2[i] for i in range(N2)]
    simd_vector_add(a2, b2, c2, N2)
    ok = all(abs(c2[i] - e2[i]) < 1e-5 for i in range(N2))
    print(f"{'PASS' if ok else 'FAIL'} [Vector Add, N=VECTOR_WIDTH={VECTOR_WIDTH}]")
    if ok: passed += 1

    print(f"\nPassed {passed}/{total} tests")
    exit(0 if passed == total else 1)
