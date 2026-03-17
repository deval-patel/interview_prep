/*
 * Q07 — FP8 Quantization and Dequantization [Hard]
 *
 * Topic: Kernel Optimization — Low-Precision Arithmetic
 *
 * Problem:
 *   Implement FP8 (8-bit floating point) quantization and dequantization.
 *   Two common FP8 formats:
 *     - E4M3: 1 sign + 4 exponent + 3 mantissa (higher precision, lower range)
 *     - E5M2: 1 sign + 5 exponent + 2 mantissa (lower precision, higher range)
 *
 *   Implement:
 *   1. Compute per-tensor scale factor to map float values into FP8 range
 *   2. Quantize float32 tensor to FP8 (with scaling)
 *   3. Dequantize FP8 tensor back to float32 (with scaling)
 *   4. Compare quantization error between E4M3 and E5M2
 *
 * Constraints:
 *   - E4M3: max representable value = 448.0, bias = 7
 *   - E5M2: max representable value = 57344.0, bias = 15
 *   - FP8 stored as uint8_t
 *   - Must handle overflow by clamping to max value
 *
 * Key Concepts:
 *   - FP8 formats and their tradeoffs
 *   - Dynamic range vs precision
 *   - Per-tensor scaling strategies
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cfloat>

using namespace std;

using fp8_t = uint8_t;

// FP8 format specifications
struct FP8Format {
    int exp_bits;     // number of exponent bits
    int man_bits;     // number of mantissa bits
    int exp_bias;     // exponent bias
    float max_val;    // maximum representable value
    const char* name;
};

const FP8Format E4M3 = {4, 3, 7, 448.0f, "E4M3"};
const FP8Format E5M2 = {5, 2, 15, 57344.0f, "E5M2"};

// ============ YOUR IMPLEMENTATION ============

/*
 * compute_scale: Compute per-tensor scale factor.
 * scale = max_abs_value / fp8_max_value
 * This maps the tensor's range into FP8's representable range.
 */
float compute_scale(const float* data, int N, const FP8Format& fmt) {
    // TODO: Find max absolute value in data
    // Return scale = max_abs / fmt.max_val
    // Handle edge case where max_abs is 0
    return 1.0f;
}

/*
 * quantize_to_fp8: Convert float to FP8.
 *
 * Steps:
 *   1. Scale: scaled_val = value / scale
 *   2. Clamp to [-max_val, max_val]
 *   3. Decompose into sign, exponent, mantissa
 *   4. Pack into 8 bits: [sign(1)][exp(exp_bits)][man(man_bits)]
 *
 * For the mantissa: extract the top man_bits of the float32 mantissa,
 * with rounding.
 */
fp8_t float_to_fp8(float value, float scale, const FP8Format& fmt) {
    // TODO: Implement float32 to FP8 conversion
    return 0;
}

/*
 * dequantize_from_fp8: Convert FP8 back to float.
 *
 * Steps:
 *   1. Unpack sign, exponent, mantissa from 8-bit value
 *   2. Reconstruct float: (-1)^sign * 2^(exp-bias) * (1 + mantissa/2^man_bits)
 *   3. Unscale: result * scale
 */
float fp8_to_float(fp8_t value, float scale, const FP8Format& fmt) {
    // TODO: Implement FP8 to float32 conversion
    return 0.0f;
}

/*
 * quantize_tensor: Quantize entire float tensor to FP8.
 */
void quantize_tensor(const float* input, fp8_t* output, int N,
                     float scale, const FP8Format& fmt) {
    // TODO: Quantize each element
}

/*
 * dequantize_tensor: Dequantize entire FP8 tensor back to float.
 */
void dequantize_tensor(const fp8_t* input, float* output, int N,
                       float scale, const FP8Format& fmt) {
    // TODO: Dequantize each element
}

/*
 * compute_quantization_error: Compute mean squared error between
 * original and quantized-then-dequantized tensor.
 */
float compute_quantization_error(const float* original, const float* reconstructed, int N) {
    // TODO: Return MSE
    return 0.0f;
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q07: FP8 Quantization ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Scale computation
    {
        total++;
        float data[] = {1.0f, -3.0f, 2.5f, -0.5f};
        float scale_e4m3 = compute_scale(data, 4, E4M3);
        float expected = 3.0f / 448.0f;
        bool ok = fabs(scale_e4m3 - expected) < 1e-6;
        printf("%s [Scale computation E4M3: %.6f, expected %.6f]\n",
               ok ? "PASS" : "FAIL", scale_e4m3, expected);
        if (ok) passed++;
    }

    // Test 2: Roundtrip simple values (E4M3)
    {
        total++;
        float values[] = {0.0f, 1.0f, -1.0f, 2.0f, 0.5f};
        float scale = compute_scale(values, 5, E4M3);
        bool ok = true;
        for (int i = 0; i < 5; i++) {
            fp8_t q = float_to_fp8(values[i], scale, E4M3);
            float dq = fp8_to_float(q, scale, E4M3);
            float err = fabs(dq - values[i]);
            if (err > 0.2f) { ok = false; }
        }
        printf("%s [E4M3 roundtrip simple values]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 3: Roundtrip simple values (E5M2)
    {
        total++;
        float values[] = {0.0f, 1.0f, -1.0f, 2.0f, 0.5f};
        float scale = compute_scale(values, 5, E5M2);
        bool ok = true;
        for (int i = 0; i < 5; i++) {
            fp8_t q = float_to_fp8(values[i], scale, E5M2);
            float dq = fp8_to_float(q, scale, E5M2);
            float err = fabs(dq - values[i]);
            if (err > 0.5f) { ok = false; }
        }
        printf("%s [E5M2 roundtrip simple values]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 4: Tensor quantization
    {
        total++;
        const int N = 256;
        vector<float> input(N), output(N);
        vector<fp8_t> quantized(N);

        srand(42);
        for (int i = 0; i < N; i++)
            input[i] = ((float)(rand() % 2000) - 1000) / 100.0f;

        float scale = compute_scale(input.data(), N, E4M3);
        quantize_tensor(input.data(), quantized.data(), N, scale, E4M3);
        dequantize_tensor(quantized.data(), output.data(), N, scale, E4M3);

        float mse = compute_quantization_error(input.data(), output.data(), N);
        bool ok = mse < 1.0f;  // reasonable error for FP8
        printf("%s [E4M3 tensor quant MSE = %.4f]\n", ok ? "PASS" : "FAIL", mse);
        if (ok) passed++;
    }

    // Test 5: E4M3 should have lower error than E5M2 for same data (more mantissa bits)
    {
        total++;
        const int N = 256;
        vector<float> input(N), out_e4(N), out_e5(N);
        vector<fp8_t> q_e4(N), q_e5(N);

        for (int i = 0; i < N; i++)
            input[i] = ((float)(rand() % 200) - 100) / 100.0f;

        float scale_e4 = compute_scale(input.data(), N, E4M3);
        float scale_e5 = compute_scale(input.data(), N, E5M2);

        quantize_tensor(input.data(), q_e4.data(), N, scale_e4, E4M3);
        quantize_tensor(input.data(), q_e5.data(), N, scale_e5, E5M2);
        dequantize_tensor(q_e4.data(), out_e4.data(), N, scale_e4, E4M3);
        dequantize_tensor(q_e5.data(), out_e5.data(), N, scale_e5, E5M2);

        float mse_e4 = compute_quantization_error(input.data(), out_e4.data(), N);
        float mse_e5 = compute_quantization_error(input.data(), out_e5.data(), N);

        bool ok = mse_e4 < mse_e5;
        printf("%s [E4M3 error (%.6f) < E5M2 error (%.6f)]\n",
               ok ? "PASS" : "FAIL", mse_e4, mse_e5);
        if (ok) passed++;
    }

    // Test 6: Zero preservation
    {
        total++;
        fp8_t q = float_to_fp8(0.0f, 1.0f, E4M3);
        float dq = fp8_to_float(q, 1.0f, E4M3);
        bool ok = (dq == 0.0f);
        printf("%s [Zero preservation]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
