/*
 * Q24 — Gradient Accumulation & Mixed Precision Training Sim [Medium]
 *
 * Topic: ML Context — Training Optimization
 *
 * Problem:
 *   Simulate gradient accumulation with mixed precision training.
 *   Mixed precision uses fp16/bf16 for forward/backward passes to save memory
 *   and bandwidth, while maintaining fp32 master weights for numerical stability.
 *
 *   Implement:
 *   1. Forward pass in bf16 (simulated as reduced precision floats)
 *   2. Loss scaling to prevent underflow in fp16 gradients
 *   3. Gradient accumulation across microsteps in fp32
 *   4. Optimizer step (simple SGD) in fp32
 *   5. Copy updated weights back to bf16 for next forward pass
 *
 *   Functions:
 *   - float to_bf16(float x) — round to bf16 precision
 *   - mixed_precision_step(weights_fp32, gradients_fp16, loss_scale, lr)
 *   - gradient_accumulation(weights, data_batches, loss_scale, lr, accum_steps)
 *   - find_loss_scale(weights, data, initial_scale)
 *
 *   Use simple MSE loss and linear model (y = w * x) for testing.
 *
 * Constraints:
 *   - bf16 has 1 sign bit, 8 exponent bits, 7 mantissa bits (vs fp32's 23)
 *   - Loss scaling multiplies loss by a large factor before backward pass
 *   - Gradients are unscaled (divided by loss_scale) before accumulation
 *   - Overflow detection: if any gradient is inf or nan, skip the step
 *
 * Key Concepts:
 *   - Mixed precision training (Micikevicius et al.)
 *   - Loss scaling for numerical stability
 *   - Gradient accumulation to simulate larger batch sizes
 *   - Dynamic loss scaling
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

using namespace std;

// ============ YOUR IMPLEMENTATION ============

/*
 * to_bf16: Simulate bfloat16 by truncating the mantissa of an fp32 value.
 *
 * bf16 keeps the upper 16 bits of fp32 (1 sign + 8 exponent + 7 mantissa).
 * Implementation: reinterpret float as uint32, zero out bottom 16 bits,
 * reinterpret back as float.
 */
float to_bf16(float x) {
    // TODO: Truncate mantissa to simulate bf16 precision
    return x;
}

/*
 * has_overflow: Check if any value in the vector is inf or nan.
 */
bool has_overflow(const vector<float>& values) {
    // TODO: Return true if any value is inf or nan
    return false;
}

/*
 * compute_mse_gradient: For a simple linear model y_pred = w * x,
 * compute gradient of MSE loss = mean((y_pred - y_target)^2) w.r.t. w.
 *
 * grad_w = (2/N) * sum((w*x_i - y_i) * x_i)
 *
 * Simulate forward pass in bf16:
 *   - Cast w to bf16 for forward pass
 *   - Compute predictions in bf16
 *   - Compute gradient (which would be in fp16 in real training)
 *   - Multiply gradient by loss_scale before returning
 *
 * Returns: scaled gradient (caller must divide by loss_scale)
 */
float compute_mse_gradient_bf16(float w, const vector<float>& x,
                                 const vector<float>& y, float loss_scale) {
    // TODO: Compute MSE gradient with bf16 simulation and loss scaling
    return 0.0f;
}

/*
 * mixed_precision_step: One optimizer step with mixed precision.
 *
 * 1. Unscale gradient: grad_fp32 = grad_scaled / loss_scale
 * 2. Check for overflow in unscaled gradient
 * 3. If no overflow: w_fp32 -= lr * grad_fp32
 * 4. Return whether the step was applied (false if overflow)
 */
bool mixed_precision_step(float& w_fp32, float grad_scaled,
                          float loss_scale, float lr) {
    // TODO: Implement one SGD step with overflow checking
    return false;
}

/*
 * gradient_accumulation: Accumulate gradients over multiple microbatches
 * before taking an optimizer step.
 *
 * For each group of accum_steps microbatches:
 *   1. Compute gradient for each microbatch (with loss scaling, in bf16)
 *   2. Unscale and accumulate in fp32: accumulated += grad_scaled / loss_scale
 *   3. After accum_steps microbatches, apply: w -= lr * (accumulated / accum_steps)
 *   4. Reset accumulator
 *
 * Returns final weight after processing all data_batches.
 * data_x and data_y are vectors of microbatches (vector of vectors).
 */
float gradient_accumulation(float w_init,
                            const vector<vector<float>>& data_x,
                            const vector<vector<float>>& data_y,
                            float loss_scale, float lr, int accum_steps) {
    // TODO: Implement gradient accumulation with mixed precision
    return w_init;
}

/*
 * find_loss_scale: Dynamic loss scaling.
 *
 * Start with initial_scale. For each microbatch:
 *   - Compute gradient with current scale
 *   - If overflow detected: halve the scale, skip step
 *   - If no overflow for 'growth_interval' consecutive steps: double the scale
 *
 * Returns the final loss scale after processing all data.
 * growth_interval: number of stable steps before doubling scale (default 10).
 */
float find_loss_scale(float w, const vector<float>& x,
                      const vector<float>& y,
                      float initial_scale, int growth_interval = 10) {
    // TODO: Implement dynamic loss scaling
    return initial_scale;
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q24: Gradient Accumulation & Mixed Precision Training ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Weights update in correct direction
    // y = 3*x, initial w = 1.0, gradient should push w toward 3.0
    {
        total++;
        float w = 1.0f;
        vector<float> x = {1.0f, 2.0f, 3.0f, 4.0f};
        vector<float> y = {3.0f, 6.0f, 9.0f, 12.0f};
        float loss_scale = 1.0f;
        float lr = 0.01f;

        float grad = compute_mse_gradient_bf16(w, x, y, loss_scale);
        mixed_precision_step(w, grad, loss_scale, lr);

        // w should have increased (moved toward 3.0)
        bool ok = (w > 1.0f);
        printf("%s [Weights update toward target: w=%.4f > 1.0]\n",
               ok ? "PASS" : "FAIL", w);
        if (ok) passed++;
    }

    // Test 2: Gradient accumulation equivalent to full batch (within precision)
    {
        total++;
        // Full batch
        float w_full = 1.0f;
        vector<float> x_all = {1, 2, 3, 4, 5, 6, 7, 8};
        vector<float> y_all = {3, 6, 9, 12, 15, 18, 21, 24};
        float grad_full = compute_mse_gradient_bf16(w_full, x_all, y_all, 1.0f);
        mixed_precision_step(w_full, grad_full, 1.0f, 0.01f);

        // Accumulated over 2 microbatches
        float w_acc = 1.0f;
        vector<vector<float>> data_x = {{1, 2, 3, 4}, {5, 6, 7, 8}};
        vector<vector<float>> data_y = {{3, 6, 9, 12}, {15, 18, 21, 24}};
        w_acc = gradient_accumulation(w_acc, data_x, data_y, 1.0f, 0.01f, 2);

        bool ok = fabs(w_full - w_acc) < 0.05f;  // Allow bf16 precision difference
        printf("%s [Accum matches full batch: full=%.4f, accum=%.4f]\n",
               ok ? "PASS" : "FAIL", w_full, w_acc);
        if (ok) passed++;
    }

    // Test 3: Loss scale reduces on overflow detection
    {
        total++;
        float w = 1.0f;
        vector<float> x = {1e10f, 2e10f};  // Large values to cause overflow
        vector<float> y = {3e10f, 6e10f};
        float initial_scale = 1e20f;  // Very large scale to force overflow
        float final_scale = find_loss_scale(w, x, y, initial_scale, 10);
        bool ok = (final_scale < initial_scale);
        printf("%s [Loss scale reduces on overflow: %.2e -> %.2e]\n",
               ok ? "PASS" : "FAIL", initial_scale, final_scale);
        if (ok) passed++;
    }

    // Test 4: Loss scale increases after stable steps
    {
        total++;
        float w = 2.5f;
        // Small well-behaved data, won't overflow
        vector<float> x, y;
        for (int i = 0; i < 50; i++) {
            x.push_back((float)(i + 1) * 0.1f);
            y.push_back((float)(i + 1) * 0.3f);
        }
        float initial_scale = 1.0f;
        float final_scale = find_loss_scale(w, x, y, initial_scale, 5);
        bool ok = (final_scale > initial_scale);
        printf("%s [Loss scale increases when stable: %.2f -> %.2f]\n",
               ok ? "PASS" : "FAIL", initial_scale, final_scale);
        if (ok) passed++;
    }

    // Test 5: Mixed precision matches fp32 training approximately
    {
        total++;
        vector<float> x = {1, 2, 3, 4};
        vector<float> y = {3, 6, 9, 12};
        float lr = 0.01f;

        // fp32 training
        float w_fp32 = 1.0f;
        for (int step = 0; step < 20; step++) {
            float grad = 0.0f;
            int N = (int)x.size();
            for (int i = 0; i < N; i++) {
                grad += 2.0f * (w_fp32 * x[i] - y[i]) * x[i];
            }
            grad /= N;
            w_fp32 -= lr * grad;
        }

        // Mixed precision training
        float w_mp = 1.0f;
        for (int step = 0; step < 20; step++) {
            float grad = compute_mse_gradient_bf16(w_mp, x, y, 1.0f);
            mixed_precision_step(w_mp, grad, 1.0f, lr);
        }

        bool ok = fabs(w_fp32 - w_mp) < 0.1f;  // Should be close
        printf("%s [Mixed precision ~ fp32: fp32=%.4f, mp=%.4f]\n",
               ok ? "PASS" : "FAIL", w_fp32, w_mp);
        if (ok) passed++;
    }

    // Test 6: Zero gradients don't change weights
    {
        total++;
        float w = 2.5f;
        float w_orig = w;
        // y = w * x exactly, so gradient is zero
        vector<float> x = {1, 2, 3};
        vector<float> y = {2.5f, 5.0f, 7.5f};
        float grad = compute_mse_gradient_bf16(w, x, y, 1.0f);
        mixed_precision_step(w, grad, 1.0f, 0.01f);
        bool ok = fabs(w - w_orig) < 0.01f;  // Allow small bf16 rounding
        printf("%s [Zero gradient: w=%.4f, expected ~%.4f]\n",
               ok ? "PASS" : "FAIL", w, w_orig);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
