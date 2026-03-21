"""
Q13 -- Softmax Kernel with Numerical Stability [Medium]

Topic: ML Context -- XLA-style Operations

Problem:
  Implement multiple versions of softmax: softmax(x)_i = exp(x_i) / sum(exp(x_j))

  1. Naive softmax: Direct computation (overflows for large values)
  2. Stable softmax: Subtract max before exp (standard trick)
  3. Online softmax: Single-pass algorithm (compute max and sum simultaneously)
  4. Tiled softmax: Process data in tiles with correction factors

Constraints:
  - Must produce valid probability distributions (sum to 1, all >= 0)
  - Stable versions must handle inputs up to +/-500
  - Online version must use a single pass over the data

Key Concepts:
  - Numerical stability in floating point
  - Log-sum-exp trick
  - Online algorithms (single-pass computation)
"""

import math


# ============ YOUR IMPLEMENTATION ============

def naive_softmax(input_):
    """
    naive_softmax: Direct computation, no numerical stability.
    softmax(x)_i = exp(x_i) / sum(exp(x_j))
    WARNING: Will overflow for large input values!

    Args:
        input_: list of floats

    Returns:
        list of floats (softmax output)
    """
    # TODO: Implement direct softmax
    # 1. Compute s = sum(exp(input_[i]))
    # 2. output[i] = exp(input_[i]) / s
    return [0.0] * len(input_)


def stable_softmax(input_):
    """
    stable_softmax: Numerically stable version.
    softmax(x)_i = exp(x_i - max(x)) / sum(exp(x_j - max(x)))

    By subtracting max(x), the largest exp() argument is 0,
    preventing overflow. This doesn't change the result because:
      exp(x_i - c) / sum(exp(x_j - c)) = exp(x_i)/sum(exp(x_j))

    Args:
        input_: list of floats

    Returns:
        list of floats (softmax output)
    """
    # TODO: Implement stable softmax
    # Pass 1: Find max
    # Pass 2: Compute sum of exp(x_i - max)
    # Pass 3: Normalize
    return [0.0] * len(input_)


def online_softmax(input_):
    """
    online_softmax: Single-pass algorithm.

    Maintains running max and running sum, correcting the sum
    when a new max is found.

    Algorithm:
      max_val = -inf, sum_val = 0
      For each x_i:
        if x_i > max_val:
          sum_val = sum_val * exp(max_val - x_i) + exp(0)  # correct old sum
          max_val = x_i
        else:
          sum_val += exp(x_i - max_val)
      Then: output[i] = exp(input_[i] - max_val) / sum_val

    This computes max and denominator in one pass (useful when data is streamed).

    Args:
        input_: list of floats

    Returns:
        list of floats (softmax output)
    """
    # TODO: Implement online (single-pass) softmax
    return [0.0] * len(input_)


def tiled_softmax(input_, tile_size):
    """
    tiled_softmax: Process in tiles, then combine.

    For each tile, compute local max and local sum.
    Then combine across tiles:
      global_max = max of all tile maxes
      global_sum = sum of (tile_sum * exp(tile_max - global_max)) for all tiles
      output[i] = exp(input_[i] - global_max) / global_sum

    This is how softmax is implemented on hardware with limited SRAM.

    Args:
        input_: list of floats
        tile_size: int, number of elements per tile

    Returns:
        list of floats (softmax output)
    """
    # TODO: Implement tiled softmax
    # 1. For each tile: compute local max and local sum_exp
    # 2. Compute global max across all tiles
    # 3. Compute global sum by adjusting each tile's local sum
    # 4. Compute final outputs
    return [0.0] * len(input_)


# ============ TEST FRAMEWORK ============

def is_valid_distribution(output, tol=1e-5):
    for v in output:
        if v < 0:
            return False
    return abs(sum(output) - 1.0) < tol


def arrays_close(a, b, tol=1e-5):
    if len(a) != len(b):
        return False
    for x, y in zip(a, b):
        if abs(x - y) > tol:
            return False
    return True


if __name__ == "__main__":
    print("=== Q13: Softmax Kernel ===\n")
    passed = 0
    total = 0

    # Test 1: Stable softmax basic correctness
    total += 1
    inp = [1.0, 2.0, 3.0, 4.0]
    out = stable_softmax(inp)
    ok = is_valid_distribution(out)
    # exp(4) dominates: out[3] should be largest
    ok = ok and out[3] > out[2] > out[1] > out[0]
    print(f"{'PASS' if ok else 'FAIL'} [Stable softmax basic]")
    if ok:
        passed += 1

    # Test 2: Uniform input -> uniform output
    total += 1
    inp = [5.0, 5.0, 5.0, 5.0]
    out = stable_softmax(inp)
    ok = all(abs(v - 0.25) < 1e-5 for v in out)
    print(f"{'PASS' if ok else 'FAIL'} [Uniform input -> uniform output]")
    if ok:
        passed += 1

    # Test 3: Stable softmax handles large values (naive would overflow)
    total += 1
    inp = [500.0, 499.0, 498.0]
    out_stable = stable_softmax(inp)
    ok = is_valid_distribution(out_stable)
    ok = ok and out_stable[0] > 0.5  # 500 should dominate
    print(f"{'PASS' if ok else 'FAIL'} [Large values: stable handles +/-500]")
    if ok:
        passed += 1

    # Test 4: Online softmax matches stable
    total += 1
    N = 32
    inp = [float(i * 3 - 40) for i in range(N)]
    out_stable = stable_softmax(inp)
    out_online = online_softmax(inp)
    ok = arrays_close(out_stable, out_online, tol=1e-5)
    ok = ok and is_valid_distribution(out_online)
    print(f"{'PASS' if ok else 'FAIL'} [Online matches stable]")
    if ok:
        passed += 1

    # Test 5: Tiled softmax matches stable
    total += 1
    import random
    random.seed(0)
    N = 64
    inp = [float(random.randint(-50, 49)) for _ in range(N)]
    out_stable = stable_softmax(inp)
    out_tiled = tiled_softmax(inp, 16)
    ok = arrays_close(out_stable, out_tiled, tol=1e-4)
    ok = ok and is_valid_distribution(out_tiled)
    print(f"{'PASS' if ok else 'FAIL'} [Tiled matches stable (tile=16)]")
    if ok:
        passed += 1

    # Test 6: Single element
    total += 1
    inp = [42.0]
    out = stable_softmax(inp)
    ok = abs(out[0] - 1.0) < 1e-6
    print(f"{'PASS' if ok else 'FAIL'} [Single element -> 1.0]")
    if ok:
        passed += 1

    # Test 7: Negative values
    total += 1
    inp = [-10.0, -20.0, -5.0, -15.0]
    out = stable_softmax(inp)
    ok = is_valid_distribution(out)
    ok = ok and out[2] > out[0]  # -5 > -10 -> larger softmax
    print(f"{'PASS' if ok else 'FAIL'} [Negative values]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
