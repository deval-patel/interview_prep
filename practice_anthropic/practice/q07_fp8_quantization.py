"""
Q07 -- FP8 Quantization and Dequantization [Hard]

Topic: Kernel Optimization -- Low-Precision Arithmetic

Problem:
  Implement FP8 (8-bit floating point) quantization and dequantization.
  Two common FP8 formats:
    - E4M3: 1 sign + 4 exponent + 3 mantissa (higher precision, lower range)
    - E5M2: 1 sign + 5 exponent + 2 mantissa (lower precision, higher range)

  Implement:
  1. Compute per-tensor scale factor to map float values into FP8 range
  2. Quantize float32 tensor to FP8 (with scaling)
  3. Dequantize FP8 tensor back to float32 (with scaling)
  4. Compare quantization error between E4M3 and E5M2

Constraints:
  - E4M3: max representable value = 448.0, bias = 7
  - E5M2: max representable value = 57344.0, bias = 15
  - FP8 stored as int (0-255)
  - Must handle overflow by clamping to max value

Key Concepts:
  - FP8 formats and their tradeoffs
  - Dynamic range vs precision
  - Per-tensor scaling strategies
"""

import struct
import math
import random


class FP8Format:
    def __init__(self, exp_bits, man_bits, exp_bias, max_val, name):
        self.exp_bits = exp_bits    # number of exponent bits
        self.man_bits = man_bits    # number of mantissa bits
        self.exp_bias = exp_bias    # exponent bias
        self.max_val = max_val      # maximum representable value
        self.name = name


E4M3 = FP8Format(4, 3, 7, 448.0, "E4M3")
E5M2 = FP8Format(5, 2, 15, 57344.0, "E5M2")


# ============ YOUR IMPLEMENTATION ============

def compute_scale(data, fmt):
    """
    Compute per-tensor scale factor.
    scale = max_abs_value / fp8_max_value
    This maps the tensor's range into FP8's representable range.

    Args:
        data: list of float values
        fmt: FP8Format instance

    Returns:
        float scale factor
    """
    # TODO: Find max absolute value in data
    # Return scale = max_abs / fmt.max_val
    # Handle edge case where max_abs is 0
    return 1.0


def float_to_fp8(value, scale, fmt):
    """
    Convert float to FP8.

    Steps:
      1. Scale: scaled_val = value / scale
      2. Clamp to [-max_val, max_val]
      3. Decompose into sign, exponent, mantissa
      4. Pack into 8 bits: [sign(1)][exp(exp_bits)][man(man_bits)]

    For the mantissa: extract the top man_bits of the float32 mantissa,
    with rounding.

    Args:
        value: float32 value
        scale: scale factor
        fmt: FP8Format instance

    Returns:
        int (0-255) representing the FP8 value
    """
    # TODO: Implement float32 to FP8 conversion
    return 0


def fp8_to_float(value, scale, fmt):
    """
    Convert FP8 back to float.

    Steps:
      1. Unpack sign, exponent, mantissa from 8-bit value
      2. Reconstruct float: (-1)^sign * 2^(exp-bias) * (1 + mantissa/2^man_bits)
      3. Unscale: result * scale

    Args:
        value: int (0-255) representing FP8 value
        scale: scale factor
        fmt: FP8Format instance

    Returns:
        float32 value
    """
    # TODO: Implement FP8 to float32 conversion
    return 0.0


def quantize_tensor(input_data, scale, fmt):
    """
    Quantize entire float tensor to FP8.

    Args:
        input_data: list of floats
        scale: scale factor
        fmt: FP8Format instance

    Returns:
        list of int (0-255) representing FP8 values
    """
    # TODO: Quantize each element
    return [0] * len(input_data)


def dequantize_tensor(input_data, scale, fmt):
    """
    Dequantize entire FP8 tensor back to float.

    Args:
        input_data: list of int (0-255) representing FP8 values
        scale: scale factor
        fmt: FP8Format instance

    Returns:
        list of floats
    """
    # TODO: Dequantize each element
    return [0.0] * len(input_data)


def compute_quantization_error(original, reconstructed):
    """
    Compute mean squared error between original and
    quantized-then-dequantized tensor.

    Args:
        original: list of floats
        reconstructed: list of floats

    Returns:
        float MSE
    """
    # TODO: Return MSE
    return 0.0


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q07: FP8 Quantization ===\n")
    passed = 0
    total = 0

    # Test 1: Scale computation
    total += 1
    data = [1.0, -3.0, 2.5, -0.5]
    scale_e4m3 = compute_scale(data, E4M3)
    expected = 3.0 / 448.0
    ok = abs(scale_e4m3 - expected) < 1e-6
    print(f"{'PASS' if ok else 'FAIL'} [Scale computation E4M3: {scale_e4m3:.6f}, expected {expected:.6f}]")
    if ok:
        passed += 1

    # Test 2: Roundtrip simple values (E4M3)
    total += 1
    values = [0.0, 1.0, -1.0, 2.0, 0.5]
    scale = compute_scale(values, E4M3)
    ok = True
    for v in values:
        q = float_to_fp8(v, scale, E4M3)
        dq = fp8_to_float(q, scale, E4M3)
        err = abs(dq - v)
        if err > 0.2:
            ok = False
    print(f"{'PASS' if ok else 'FAIL'} [E4M3 roundtrip simple values]")
    if ok:
        passed += 1

    # Test 3: Roundtrip simple values (E5M2)
    total += 1
    values = [0.0, 1.0, -1.0, 2.0, 0.5]
    scale = compute_scale(values, E5M2)
    ok = True
    for v in values:
        q = float_to_fp8(v, scale, E5M2)
        dq = fp8_to_float(q, scale, E5M2)
        err = abs(dq - v)
        if err > 0.5:
            ok = False
    print(f"{'PASS' if ok else 'FAIL'} [E5M2 roundtrip simple values]")
    if ok:
        passed += 1

    # Test 4: Tensor quantization
    total += 1
    N = 256
    random.seed(42)
    input_data = [((random.randint(0, 1999)) - 1000) / 100.0 for _ in range(N)]
    scale = compute_scale(input_data, E4M3)
    quantized = quantize_tensor(input_data, scale, E4M3)
    output = dequantize_tensor(quantized, scale, E4M3)
    mse = compute_quantization_error(input_data, output)
    ok = mse < 1.0  # reasonable error for FP8
    print(f"{'PASS' if ok else 'FAIL'} [E4M3 tensor quant MSE = {mse:.4f}]")
    if ok:
        passed += 1

    # Test 5: E4M3 should have lower error than E5M2 for same data (more mantissa bits)
    total += 1
    input_data = [((random.randint(0, 199)) - 100) / 100.0 for _ in range(N)]
    scale_e4 = compute_scale(input_data, E4M3)
    scale_e5 = compute_scale(input_data, E5M2)
    q_e4 = quantize_tensor(input_data, scale_e4, E4M3)
    q_e5 = quantize_tensor(input_data, scale_e5, E5M2)
    out_e4 = dequantize_tensor(q_e4, scale_e4, E4M3)
    out_e5 = dequantize_tensor(q_e5, scale_e5, E5M2)
    mse_e4 = compute_quantization_error(input_data, out_e4)
    mse_e5 = compute_quantization_error(input_data, out_e5)
    ok = mse_e4 < mse_e5
    print(f"{'PASS' if ok else 'FAIL'} [E4M3 error ({mse_e4:.6f}) < E5M2 error ({mse_e5:.6f})]")
    if ok:
        passed += 1

    # Test 6: Zero preservation
    total += 1
    q = float_to_fp8(0.0, 1.0, E4M3)
    dq = fp8_to_float(q, 1.0, E4M3)
    ok = dq == 0.0
    print(f"{'PASS' if ok else 'FAIL'} [Zero preservation]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
