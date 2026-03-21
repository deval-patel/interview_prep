"""
Q24 — Gradient Accumulation & Mixed Precision Training Sim [Medium]

Topic: ML Context — Training Optimization

Problem:
  Simulate gradient accumulation with mixed precision training.
  Mixed precision uses fp16/bf16 for forward/backward passes to save memory
  and bandwidth, while maintaining fp32 master weights for numerical stability.

  Implement:
  1. Forward pass in bf16 (simulated as reduced precision floats)
  2. Loss scaling to prevent underflow in fp16 gradients
  3. Gradient accumulation across microsteps in fp32
  4. Optimizer step (simple SGD) in fp32
  5. Copy updated weights back to bf16 for next forward pass

  Functions:
  - to_bf16(x) — round to bf16 precision
  - mixed_precision_step(w_fp32, grad_scaled, loss_scale, lr)
  - gradient_accumulation(w_init, data_x, data_y, loss_scale, lr, accum_steps)
  - find_loss_scale(w, x, y, initial_scale, growth_interval)

  Use simple MSE loss and linear model (y = w * x) for testing.

Constraints:
  - bf16 has 1 sign bit, 8 exponent bits, 7 mantissa bits (vs fp32's 23)
  - Loss scaling multiplies loss by a large factor before backward pass
  - Gradients are unscaled (divided by loss_scale) before accumulation
  - Overflow detection: if any gradient is inf or nan, skip the step

Key Concepts:
  - Mixed precision training (Micikevicius et al.)
  - Loss scaling for numerical stability
  - Gradient accumulation to simulate larger batch sizes
  - Dynamic loss scaling
"""

import struct
import math


# ============ YOUR IMPLEMENTATION ============

def to_bf16(x: float) -> float:
    """
    Simulate bfloat16 by truncating the mantissa of an fp32 value.

    bf16 keeps the upper 16 bits of fp32 (1 sign + 8 exponent + 7 mantissa).
    Implementation: pack as fp32, zero out bottom 16 bits, unpack back.
    Use struct module for bit manipulation.

    Edge cases to handle before packing:
      - nan: return float('nan')
      - x > ~3.4e38 (fp32 max): return float('inf')   -- struct.pack('>f') overflows
      - x < ~-3.4e38:           return float('-inf')
    """
    # TODO: Handle nan and values exceeding fp32 range, then truncate mantissa
    return x


def has_overflow(values: list[float]) -> bool:
    """Check if any value in the list is inf or nan."""
    # TODO: Return True if any value is inf or nan
    return False


def compute_mse_gradient_bf16(w: float, x: list[float], y: list[float],
                               loss_scale: float) -> float:
    """
    For a simple linear model y_pred = w * x,
    compute gradient of MSE loss = mean((y_pred - y_target)^2) w.r.t. w.

    grad_w = (2/N) * sum((w*x_i - y_i) * x_i)

    Simulate forward pass in bf16:
      - Cast w to bf16 for forward pass
      - Compute predictions in bf16
      - Compute gradient (which would be in fp16 in real training)
      - Multiply gradient by loss_scale before returning

    Returns: scaled gradient (caller must divide by loss_scale)
    """
    # TODO: Compute MSE gradient with bf16 simulation and loss scaling
    return 0.0


def mixed_precision_step(w_fp32: float, grad_scaled: float,
                         loss_scale: float, lr: float) -> tuple[float, bool]:
    """
    One optimizer step with mixed precision.

    1. Unscale gradient: grad_fp32 = grad_scaled / loss_scale
    2. Check for overflow in unscaled gradient
    3. If no overflow: w_fp32 -= lr * grad_fp32
    4. Return (updated_w, step_applied)
    """
    # TODO: Implement one SGD step with overflow checking
    return w_fp32, False


def gradient_accumulation(w_init: float,
                          data_x: list[list[float]],
                          data_y: list[list[float]],
                          loss_scale: float, lr: float,
                          accum_steps: int) -> float:
    """
    Accumulate gradients over multiple microbatches before stepping.

    For each group of accum_steps microbatches:
      1. Compute gradient for each microbatch (with loss scaling, in bf16)
      2. Unscale and accumulate in fp32: accumulated += grad_scaled / loss_scale
      3. After accum_steps microbatches, apply: w -= lr * (accumulated / accum_steps)
      4. Reset accumulator

    Returns final weight after processing all data_batches.
    """
    # TODO: Implement gradient accumulation with mixed precision
    return w_init


def find_loss_scale(w: float, x: list[float], y: list[float],
                    initial_scale: float, growth_interval: int = 10) -> float:
    """
    Dynamic loss scaling.

    Start with initial_scale. For each data point (treated as microbatch):
      - Compute gradient with current scale
      - If overflow detected: halve the scale, skip step
      - If no overflow for growth_interval consecutive steps: double the scale

    Returns the final loss scale after processing all data.
    """
    # TODO: Implement dynamic loss scaling
    return initial_scale


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q24: Gradient Accumulation & Mixed Precision Training ===\n")
    passed = 0
    total = 0

    # Test 1: Weights update in correct direction
    # y = 3*x, initial w = 1.0, gradient should push w toward 3.0
    total += 1
    w = 1.0
    x = [1.0, 2.0, 3.0, 4.0]
    y = [3.0, 6.0, 9.0, 12.0]
    grad = compute_mse_gradient_bf16(w, x, y, 1.0)
    w, applied = mixed_precision_step(w, grad, 1.0, 0.01)
    if w > 1.0:
        print(f"PASS [Weights update toward target: w={w:.4f} > 1.0]")
        passed += 1
    else:
        print(f"FAIL [Weights update toward target: w={w:.4f}]")

    # Test 2: Gradient accumulation equivalent to full batch (within precision)
    total += 1
    # Full batch
    w_full = 1.0
    x_all = [1, 2, 3, 4, 5, 6, 7, 8]
    y_all = [3, 6, 9, 12, 15, 18, 21, 24]
    grad_full = compute_mse_gradient_bf16(w_full, x_all, y_all, 1.0)
    w_full, _ = mixed_precision_step(w_full, grad_full, 1.0, 0.01)

    # Accumulated over 2 microbatches
    w_acc = 1.0
    data_x = [[1, 2, 3, 4], [5, 6, 7, 8]]
    data_y = [[3, 6, 9, 12], [15, 18, 21, 24]]
    w_acc = gradient_accumulation(w_acc, data_x, data_y, 1.0, 0.01, 2)

    if abs(w_full - w_acc) < 0.05:
        print(f"PASS [Accum matches full batch: full={w_full:.4f}, accum={w_acc:.4f}]")
        passed += 1
    else:
        print(f"FAIL [Accum matches full batch: full={w_full:.4f}, accum={w_acc:.4f}]")

    # Test 3: Loss scale reduces on overflow detection
    total += 1
    w = 1.0
    x = [1e10, 2e10]
    y = [3e10, 6e10]
    initial_scale = 1e20
    final_scale = find_loss_scale(w, x, y, initial_scale, 10)
    if final_scale < initial_scale:
        print(f"PASS [Loss scale reduces on overflow: {initial_scale:.2e} -> {final_scale:.2e}]")
        passed += 1
    else:
        print(f"FAIL [Loss scale reduces on overflow: {initial_scale:.2e} -> {final_scale:.2e}]")

    # Test 4: Loss scale increases after stable steps
    total += 1
    w = 2.5
    x = [(i + 1) * 0.1 for i in range(50)]
    y = [(i + 1) * 0.3 for i in range(50)]
    initial_scale = 1.0
    final_scale = find_loss_scale(w, x, y, initial_scale, 5)
    if final_scale > initial_scale:
        print(f"PASS [Loss scale increases when stable: {initial_scale:.2f} -> {final_scale:.2f}]")
        passed += 1
    else:
        print(f"FAIL [Loss scale increases when stable: {initial_scale:.2f} -> {final_scale:.2f}]")

    # Test 5: Mixed precision matches fp32 training approximately
    total += 1
    x = [1, 2, 3, 4]
    y = [3, 6, 9, 12]
    lr = 0.01

    # fp32 training
    w_fp32 = 1.0
    for step in range(20):
        grad = 0.0
        N = len(x)
        for i in range(N):
            grad += 2.0 * (w_fp32 * x[i] - y[i]) * x[i]
        grad /= N
        w_fp32 -= lr * grad

    # Mixed precision training
    w_mp = 1.0
    for step in range(20):
        grad = compute_mse_gradient_bf16(w_mp, x, y, 1.0)
        w_mp, _ = mixed_precision_step(w_mp, grad, 1.0, lr)

    if abs(w_fp32 - w_mp) < 0.1:
        print(f"PASS [Mixed precision ~ fp32: fp32={w_fp32:.4f}, mp={w_mp:.4f}]")
        passed += 1
    else:
        print(f"FAIL [Mixed precision ~ fp32: fp32={w_fp32:.4f}, mp={w_mp:.4f}]")

    # Test 6: Zero gradients don't change weights
    total += 1
    w = 2.5
    w_orig = w
    x = [1, 2, 3]
    y = [2.5, 5.0, 7.5]
    grad = compute_mse_gradient_bf16(w, x, y, 1.0)
    w, _ = mixed_precision_step(w, grad, 1.0, 0.01)
    if abs(w - w_orig) < 0.01:
        print(f"PASS [Zero gradient: w={w:.4f}, expected ~{w_orig:.4f}]")
        passed += 1
    else:
        print(f"FAIL [Zero gradient: w={w:.4f}, expected ~{w_orig:.4f}]")

    print(f"\nPassed {passed}/{total} tests")
