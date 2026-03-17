/*
 * Q13 — Softmax Kernel with Numerical Stability [Medium]
 *
 * Topic: ML Context — XLA-style Operations
 *
 * Problem:
 *   Implement multiple versions of softmax: softmax(x)_i = exp(x_i) / sum(exp(x_j))
 *
 *   1. Naive softmax: Direct computation (overflows for large values)
 *   2. Stable softmax: Subtract max before exp (standard trick)
 *   3. Online softmax: Single-pass algorithm (compute max and sum simultaneously)
 *   4. Tiled softmax: Process data in tiles with correction factors
 *
 * Constraints:
 *   - Must produce valid probability distributions (sum to 1, all >= 0)
 *   - Stable versions must handle inputs up to ±500
 *   - Online version must use a single pass over the data
 *
 * Key Concepts:
 *   - Numerical stability in floating point
 *   - Log-sum-exp trick
 *   - Online algorithms (single-pass computation)
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cfloat>
#include <algorithm>

using namespace std;

// ============ YOUR IMPLEMENTATION ============

/*
 * naive_softmax: Direct computation, no numerical stability.
 * softmax(x)_i = exp(x_i) / sum(exp(x_j))
 * WARNING: Will overflow for large input values!
 */
void naive_softmax(const float* input, float* output, int N) {
    // TODO: Implement direct softmax
    // 1. Compute sum = sum(exp(input[i]))
    // 2. output[i] = exp(input[i]) / sum
}

/*
 * stable_softmax: Numerically stable version.
 * softmax(x)_i = exp(x_i - max(x)) / sum(exp(x_j - max(x)))
 *
 * By subtracting max(x), the largest exp() argument is 0,
 * preventing overflow. This doesn't change the result because:
 *   exp(x_i - c) / sum(exp(x_j - c)) = exp(x_i)/sum(exp(x_j))
 */
void stable_softmax(const float* input, float* output, int N) {
    // TODO: Implement stable softmax
    // Pass 1: Find max
    // Pass 2: Compute sum of exp(x_i - max)
    // Pass 3: Normalize
}

/*
 * online_softmax: Single-pass algorithm.
 *
 * Maintains running max and running sum, correcting the sum
 * when a new max is found.
 *
 * Algorithm:
 *   max_val = -inf, sum_val = 0
 *   For each x_i:
 *     if x_i > max_val:
 *       sum_val = sum_val * exp(max_val - x_i) + exp(0)  // correct old sum
 *       max_val = x_i
 *     else:
 *       sum_val += exp(x_i - max_val)
 *   Then: output[i] = exp(input[i] - max_val) / sum_val
 *
 * This computes max and denominator in one pass (useful when data is streamed).
 */
void online_softmax(const float* input, float* output, int N) {
    // TODO: Implement online (single-pass) softmax
}

/*
 * tiled_softmax: Process in tiles, then combine.
 *
 * For each tile, compute local max and local sum.
 * Then combine across tiles:
 *   global_max = max of all tile maxes
 *   global_sum = sum of (tile_sum * exp(tile_max - global_max)) for all tiles
 *   output[i] = exp(input[i] - global_max) / global_sum
 *
 * This is how softmax is implemented on hardware with limited SRAM.
 */
void tiled_softmax(const float* input, float* output, int N, int tile_size) {
    // TODO: Implement tiled softmax
    // 1. For each tile: compute local max and local sum_exp
    // 2. Compute global max across all tiles
    // 3. Compute global sum by adjusting each tile's local sum
    // 4. Compute final outputs
}

// ============ TEST FRAMEWORK ============

bool is_valid_distribution(const float* output, int N, float tol = 1e-5) {
    float sum = 0;
    for (int i = 0; i < N; i++) {
        if (output[i] < 0) return false;
        sum += output[i];
    }
    return fabs(sum - 1.0f) < tol;
}

bool arrays_close(const float* a, const float* b, int N, float tol = 1e-5) {
    for (int i = 0; i < N; i++)
        if (fabs(a[i] - b[i]) > tol) return false;
    return true;
}

int main() {
    printf("=== Q13: Softmax Kernel ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Stable softmax basic correctness
    {
        total++;
        float input[] = {1.0f, 2.0f, 3.0f, 4.0f};
        float output[4];
        stable_softmax(input, output, 4);
        bool ok = is_valid_distribution(output, 4);
        // exp(4) dominates: output[3] should be largest
        ok = ok && output[3] > output[2] && output[2] > output[1] && output[1] > output[0];
        printf("%s [Stable softmax basic]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 2: Uniform input -> uniform output
    {
        total++;
        float input[] = {5.0f, 5.0f, 5.0f, 5.0f};
        float output[4];
        stable_softmax(input, output, 4);
        bool ok = true;
        for (int i = 0; i < 4; i++)
            if (fabs(output[i] - 0.25f) > 1e-5) ok = false;
        printf("%s [Uniform input -> uniform output]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 3: Stable softmax handles large values (naive would overflow)
    {
        total++;
        float input[] = {500.0f, 499.0f, 498.0f};
        float output_stable[3];
        stable_softmax(input, output_stable, 3);
        bool ok = is_valid_distribution(output_stable, 3);
        ok = ok && output_stable[0] > 0.5f;  // 500 should dominate
        printf("%s [Large values: stable handles ±500]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 4: Online softmax matches stable
    {
        total++;
        const int N = 32;
        vector<float> input(N);
        vector<float> out_stable(N), out_online(N);
        for (int i = 0; i < N; i++) input[i] = (float)(i * 3 - 40);

        stable_softmax(input.data(), out_stable.data(), N);
        online_softmax(input.data(), out_online.data(), N);

        bool ok = arrays_close(out_stable.data(), out_online.data(), N, 1e-5);
        ok = ok && is_valid_distribution(out_online.data(), N);
        printf("%s [Online matches stable]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 5: Tiled softmax matches stable
    {
        total++;
        const int N = 64;
        vector<float> input(N);
        vector<float> out_stable(N), out_tiled(N);
        for (int i = 0; i < N; i++) input[i] = (float)(rand() % 100 - 50);

        stable_softmax(input.data(), out_stable.data(), N);
        tiled_softmax(input.data(), out_tiled.data(), N, 16);

        bool ok = arrays_close(out_stable.data(), out_tiled.data(), N, 1e-4);
        ok = ok && is_valid_distribution(out_tiled.data(), N);
        printf("%s [Tiled matches stable (tile=16)]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 6: Single element
    {
        total++;
        float input[] = {42.0f};
        float output[1];
        stable_softmax(input, output, 1);
        bool ok = fabs(output[0] - 1.0f) < 1e-6;
        printf("%s [Single element -> 1.0]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 7: Negative values
    {
        total++;
        float input[] = {-10.0f, -20.0f, -5.0f, -15.0f};
        float output[4];
        stable_softmax(input, output, 4);
        bool ok = is_valid_distribution(output, 4);
        ok = ok && output[2] > output[0];  // -5 > -10 -> larger softmax
        printf("%s [Negative values]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
