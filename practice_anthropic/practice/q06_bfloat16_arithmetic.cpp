/*
 * Q06 — BFloat16 Arithmetic [Medium]
 *
 * Topic: Kernel Optimization — Low-Precision Arithmetic
 *
 * Problem:
 *   Implement BFloat16 (Brain Floating Point) arithmetic operations.
 *   BFloat16 uses the same exponent range as float32 but with reduced mantissa:
 *     - 1 sign bit, 8 exponent bits, 7 mantissa bits (vs 23 for float32)
 *     - Simply truncates/rounds the lower 16 bits of float32
 *
 *   Implement:
 *   1. float_to_bf16: Convert float32 to bfloat16 with round-to-nearest-even
 *   2. bf16_to_float: Convert bfloat16 back to float32
 *   3. bf16_add, bf16_mul: Arithmetic (convert to float, compute, convert back)
 *   4. bf16_dot_product: Dot product with float32 accumulation
 *
 * Constraints:
 *   - BFloat16 stored as uint16_t
 *   - Round-to-nearest-even on conversion
 *   - Dot product must accumulate in float32 for numerical stability
 *
 * Key Concepts:
 *   - IEEE 754 floating point layout
 *   - BFloat16 format (truncated float32)
 *   - Mixed-precision: compute in low precision, accumulate in high precision
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <vector>

using namespace std;

using bf16_t = uint16_t;

// ============ YOUR IMPLEMENTATION ============

/*
 * float_to_bf16: Convert float32 to bfloat16.
 *
 * BFloat16 is the upper 16 bits of float32, with rounding:
 *   float32: [sign(1)][exponent(8)][mantissa(23)]
 *   bfloat16: [sign(1)][exponent(8)][mantissa(7)]
 *
 * Round-to-nearest-even:
 *   - Look at bit 16 (round bit) and bits 0-15 (sticky bits) of float32
 *   - If round bit is 0: truncate (round down)
 *   - If round bit is 1 and sticky bits != 0: round up
 *   - If round bit is 1 and sticky bits == 0: round to even (round up if bit 17 is 1)
 *
 * Handle special cases: NaN stays NaN, Inf stays Inf
 */
bf16_t float_to_bf16(float f) {
    // TODO: Implement float32 to bfloat16 conversion with round-to-nearest-even
    // HINT: Use memcpy to get the uint32_t bit pattern of f
    return 0;
}

/*
 * bf16_to_float: Convert bfloat16 back to float32.
 * Simply place the 16-bit value in the upper 16 bits of a 32-bit float.
 */
float bf16_to_float(bf16_t b) {
    // TODO: Implement bfloat16 to float32 conversion
    return 0.0f;
}

/*
 * bf16_add: Add two bfloat16 values.
 * Convert to float32, add, convert back.
 */
bf16_t bf16_add(bf16_t a, bf16_t b) {
    // TODO: Implement
    return 0;
}

/*
 * bf16_mul: Multiply two bfloat16 values.
 * Convert to float32, multiply, convert back.
 */
bf16_t bf16_mul(bf16_t a, bf16_t b) {
    // TODO: Implement
    return 0;
}

/*
 * bf16_dot_product: Compute dot product of two bf16 vectors.
 * CRITICAL: Accumulate partial sums in float32, not bf16!
 * This is the standard "mixed-precision" pattern used in TPUs.
 *
 * Returns float32 result.
 */
float bf16_dot_product(const bf16_t* a, const bf16_t* b, int N) {
    // TODO: Implement with float32 accumulation
    return 0.0f;
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q06: BFloat16 Arithmetic ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Simple conversion roundtrip
    {
        total++;
        float vals[] = {1.0f, -1.0f, 0.0f, 3.14159f, 1e10f, 1e-10f};
        bool ok = true;
        for (float v : vals) {
            bf16_t b = float_to_bf16(v);
            float back = bf16_to_float(b);
            float rel_err = (v != 0.0f) ? fabs(back - v) / fabs(v) : fabs(back);
            if (rel_err > 0.01f && v != 0.0f) { ok = false; break; }  // bf16 has ~0.8% precision
            if (v == 0.0f && back != 0.0f) { ok = false; break; }
        }
        printf("%s [Conversion roundtrip]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 2: BF16 is exactly upper 16 bits for powers of 2
    {
        total++;
        float v = 4.0f;
        bf16_t b = float_to_bf16(v);
        float back = bf16_to_float(b);
        bool ok = (back == 4.0f);
        printf("%s [Exact conversion for 4.0]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 3: Addition
    {
        total++;
        bf16_t a = float_to_bf16(1.5f);
        bf16_t b = float_to_bf16(2.5f);
        bf16_t c = bf16_add(a, b);
        float result = bf16_to_float(c);
        bool ok = fabs(result - 4.0f) < 0.1f;
        printf("%s [1.5 + 2.5 = %.2f, expected 4.0]\n", ok ? "PASS" : "FAIL", result);
        if (ok) passed++;
    }

    // Test 4: Multiplication
    {
        total++;
        bf16_t a = float_to_bf16(3.0f);
        bf16_t b = float_to_bf16(7.0f);
        bf16_t c = bf16_mul(a, b);
        float result = bf16_to_float(c);
        bool ok = fabs(result - 21.0f) < 0.5f;
        printf("%s [3.0 * 7.0 = %.2f, expected 21.0]\n", ok ? "PASS" : "FAIL", result);
        if (ok) passed++;
    }

    // Test 5: Dot product with float32 accumulation
    {
        total++;
        const int N = 1024;
        vector<bf16_t> a(N), b(N);
        float expected = 0.0f;
        for (int i = 0; i < N; i++) {
            float fa = (float)(i + 1) / 100.0f;
            float fb = (float)(N - i) / 100.0f;
            a[i] = float_to_bf16(fa);
            b[i] = float_to_bf16(fb);
            expected += fa * fb;
        }
        float result = bf16_dot_product(a.data(), b.data(), N);
        float rel_err = fabs(result - expected) / fabs(expected);
        bool ok = rel_err < 0.02f;  // allow 2% error from bf16 quantization
        printf("%s [Dot product N=%d, result=%.1f, expected=%.1f, rel_err=%.4f]\n",
               ok ? "PASS" : "FAIL", N, result, expected, rel_err);
        if (ok) passed++;
    }

    // Test 6: Special values
    {
        total++;
        bf16_t zero = float_to_bf16(0.0f);
        bf16_t neg = float_to_bf16(-0.0f);
        float z = bf16_to_float(zero);
        float n = bf16_to_float(neg);
        bool ok = (z == 0.0f) && (n == -0.0f || n == 0.0f);
        // Also test infinity
        bf16_t inf = float_to_bf16(1.0f / 0.0f);
        float inf_back = bf16_to_float(inf);
        ok = ok && isinf(inf_back);
        printf("%s [Special values (0, -0, inf)]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
