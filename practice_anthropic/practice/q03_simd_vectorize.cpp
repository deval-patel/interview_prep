/*
 * Q03 — SIMD Vector Operations [Easy-Medium]
 *
 * Topic: Hardware Architecture — SIMD
 *
 * Problem:
 *   Implement vectorized operations using a simulated SIMD register abstraction.
 *   The SIMD "hardware" processes VECTOR_WIDTH elements in parallel.
 *
 *   Operations to implement:
 *   1. Vector Addition: C[i] = A[i] + B[i]
 *   2. Dot Product: sum(A[i] * B[i])
 *   3. ReLU: out[i] = max(0, in[i])
 *   4. Fused Multiply-Add: out[i] = A[i] * B[i] + C[i]
 *
 *   All operations must handle the case where N % VECTOR_WIDTH != 0 (tail elements).
 *
 * Constraints:
 *   - VECTOR_WIDTH = 8 (simulates 256-bit SIMD with float32)
 *   - Must use the provided SIMD primitives (simd_load, simd_store, etc.)
 *   - Scalar fallback required for tail elements
 *
 * Key Concepts:
 *   - Strip-mining (processing VECTOR_WIDTH elements per iteration)
 *   - Tail handling (remaining elements < VECTOR_WIDTH)
 *   - SIMD register abstraction
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

const int VECTOR_WIDTH = 8;

// Simulated SIMD register
struct SimdReg {
    float data[8];  // VECTOR_WIDTH elements
};

// ---- Simulated SIMD intrinsics ----
// These mimic hardware SIMD operations that process VECTOR_WIDTH elements at once

SimdReg simd_load(const float* ptr) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = ptr[i];
    return r;
}

void simd_store(float* ptr, SimdReg r) {
    for (int i = 0; i < VECTOR_WIDTH; i++) ptr[i] = r.data[i];
}

SimdReg simd_add(SimdReg a, SimdReg b) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = a.data[i] + b.data[i];
    return r;
}

SimdReg simd_mul(SimdReg a, SimdReg b) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = a.data[i] * b.data[i];
    return r;
}

SimdReg simd_max(SimdReg a, SimdReg b) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = max(a.data[i], b.data[i]);
    return r;
}

SimdReg simd_set1(float val) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = val;
    return r;
}

SimdReg simd_setzero() {
    return simd_set1(0.0f);
}

// Horizontal sum: sum all elements in a SIMD register
float simd_hsum(SimdReg r) {
    float sum = 0;
    for (int i = 0; i < VECTOR_WIDTH; i++) sum += r.data[i];
    return sum;
}

// FMA: a * b + c (fused multiply-add)
SimdReg simd_fmadd(SimdReg a, SimdReg b, SimdReg c) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = a.data[i] * b.data[i] + c.data[i];
    return r;
}

// ============ YOUR IMPLEMENTATION ============

/*
 * simd_vector_add: C[i] = A[i] + B[i] for i in [0, N)
 * Use strip-mining: process VECTOR_WIDTH elements per iteration
 * Handle tail with scalar ops
 */
void simd_vector_add(const float* A, const float* B, float* C, int N) {
    // TODO: Implement using simd_load, simd_add, simd_store
    // Process full SIMD widths, then handle remaining elements
}

/*
 * simd_dot_product: return sum(A[i] * B[i]) for i in [0, N)
 * Accumulate partial sums in a SIMD register, then horizontal sum
 */
float simd_dot_product(const float* A, const float* B, int N) {
    // TODO: Implement using simd_load, simd_mul, simd_add, simd_hsum
    return 0.0f;
}

/*
 * simd_relu: out[i] = max(0, in[i]) for i in [0, N)
 */
void simd_relu(const float* in, float* out, int N) {
    // TODO: Implement using simd_load, simd_max, simd_set1(0), simd_store
}

/*
 * simd_fma: out[i] = A[i] * B[i] + C[i] for i in [0, N)
 */
void simd_fma(const float* A, const float* B, const float* C, float* out, int N) {
    // TODO: Implement using simd_load, simd_fmadd, simd_store
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q03: SIMD Vector Operations ===\n\n");
    int passed = 0, total = 0;

    // Test with N not divisible by VECTOR_WIDTH
    const int N = 37;  // 37 = 4*8 + 5 (5 tail elements)
    vector<float> A(N), B(N), C_arr(N), expected(N);

    for (int i = 0; i < N; i++) {
        A[i] = (float)(i + 1);
        B[i] = (float)(N - i);
    }

    // Test 1: Vector Add
    {
        total++;
        for (int i = 0; i < N; i++) expected[i] = A[i] + B[i];
        fill(C_arr.begin(), C_arr.end(), 0.0f);
        simd_vector_add(A.data(), B.data(), C_arr.data(), N);
        bool ok = true;
        for (int i = 0; i < N; i++)
            if (fabs(C_arr[i] - expected[i]) > 1e-5) { ok = false; break; }
        printf("%s [Vector Add, N=%d]\n", ok ? "PASS" : "FAIL", N);
        if (ok) passed++;
    }

    // Test 2: Dot Product
    {
        total++;
        float exp_dot = 0;
        for (int i = 0; i < N; i++) exp_dot += A[i] * B[i];
        float result = simd_dot_product(A.data(), B.data(), N);
        bool ok = fabs(result - exp_dot) < 1e-2;
        printf("%s [Dot Product = %.1f, expected %.1f]\n", ok ? "PASS" : "FAIL", result, exp_dot);
        if (ok) passed++;
    }

    // Test 3: ReLU
    {
        total++;
        vector<float> input(N), output(N, 0), exp_relu(N);
        for (int i = 0; i < N; i++) {
            input[i] = (float)(i - N/2);  // mix of negative and positive
            exp_relu[i] = max(0.0f, input[i]);
        }
        simd_relu(input.data(), output.data(), N);
        bool ok = true;
        for (int i = 0; i < N; i++)
            if (fabs(output[i] - exp_relu[i]) > 1e-5) { ok = false; break; }
        printf("%s [ReLU, N=%d]\n", ok ? "PASS" : "FAIL", N);
        if (ok) passed++;
    }

    // Test 4: FMA
    {
        total++;
        vector<float> fma_c(N), fma_out(N, 0), fma_exp(N);
        for (int i = 0; i < N; i++) {
            fma_c[i] = (float)(i * 0.5);
            fma_exp[i] = A[i] * B[i] + fma_c[i];
        }
        simd_fma(A.data(), B.data(), fma_c.data(), fma_out.data(), N);
        bool ok = true;
        for (int i = 0; i < N; i++)
            if (fabs(fma_out[i] - fma_exp[i]) > 1e-3) { ok = false; break; }
        printf("%s [FMA, N=%d]\n", ok ? "PASS" : "FAIL", N);
        if (ok) passed++;
    }

    // Test 5: Edge case N = 1
    {
        total++;
        float a = 3.0f, b = 4.0f, c = 0.0f;
        simd_vector_add(&a, &b, &c, 1);
        bool ok = fabs(c - 7.0f) < 1e-5;
        printf("%s [Vector Add, N=1]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 6: Exact SIMD width
    {
        total++;
        const int N2 = VECTOR_WIDTH;
        vector<float> a2(N2), b2(N2), c2(N2, 0), e2(N2);
        for (int i = 0; i < N2; i++) {
            a2[i] = (float)i; b2[i] = (float)(i*2); e2[i] = a2[i] + b2[i];
        }
        simd_vector_add(a2.data(), b2.data(), c2.data(), N2);
        bool ok = true;
        for (int i = 0; i < N2; i++)
            if (fabs(c2[i] - e2[i]) > 1e-5) { ok = false; break; }
        printf("%s [Vector Add, N=VECTOR_WIDTH=%d]\n", ok ? "PASS" : "FAIL", VECTOR_WIDTH);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
