"""
Q17 — 2D Convolution with im2col [Medium-Hard]

Topic: ML Context / Kernel Optimization

Problem:
  Implement 2D convolution in two ways:
  1. Naive: Direct nested-loop convolution
  2. im2col: Transform input into a column matrix, then use matrix multiplication

  The im2col approach converts convolution into a GEMM operation,
  which maps perfectly to systolic arrays and is the standard approach on TPUs.

Constraints:
  - Input: (H, W) single-channel image
  - Kernel: (KH, KW) filter
  - Support stride and padding
  - Output dimensions: ((H + 2*pad - KH) // stride + 1, ...)

Key Concepts:
  - im2col transformation
  - Converting convolution to matrix multiplication (GEMM)
  - Memory-compute tradeoff
"""

import random


# ============ YOUR IMPLEMENTATION ============

def compute_output_dim(input_dim, kernel_dim, stride, padding):
    """output = (input + 2*padding - kernel) // stride + 1"""
    # TODO: Implement
    return 0


def naive_conv2d(inp, kernel, H, W, KH, KW, stride, padding):
    """
    Direct nested-loop 2D convolution.
    For each output position (oh, ow):
      output[oh][ow] = sum over (kh, kw) of:
        input[oh*stride + kh - pad][ow*stride + kw - pad] * kernel[kh][kw]
    """
    OH = compute_output_dim(H, KH, stride, padding)
    OW = compute_output_dim(W, KW, stride, padding)
    output = [[0.0] * OW for _ in range(OH)]
    # TODO: Implement naive convolution
    return output


def im2col(inp, H, W, KH, KW, stride, padding, OH, OW):
    """
    Transform input into column matrix for GEMM-based convolution.
    col_matrix shape: [OH*OW][KH*KW]
    """
    col_matrix = [[0.0] * (KH * KW) for _ in range(OH * OW)]
    # TODO: Implement im2col transformation
    return col_matrix


def im2col_conv2d(inp, kernel, H, W, KH, KW, stride, padding):
    """
    Convolution using im2col + matrix multiplication.
    1. Create col_matrix using im2col
    2. Flatten kernel
    3. output_flat[i] = dot(col_matrix[i], kernel_flat)
    4. Reshape to [OH][OW]
    """
    OH = compute_output_dim(H, KH, stride, padding)
    OW = compute_output_dim(W, KW, stride, padding)
    output = [[0.0] * OW for _ in range(OH)]
    # TODO: Implement conv2d using im2col approach
    return output


# ============ TEST FRAMEWORK ============

def matrices_close(A, B, M, N, tol=1e-4):
    for i in range(M):
        for j in range(N):
            if abs(A[i][j] - B[i][j]) > tol:
                return False
    return True


if __name__ == "__main__":
    print("=== Q17: 2D Convolution with im2col ===\n")
    passed = 0
    total = 0

    # Test 1: Output dimension calculation
    total += 1
    od = compute_output_dim(5, 3, 1, 0)
    ok = od == 3
    print(f"{'PASS' if ok else 'FAIL'} [Output dim: 5,k=3,s=1,p=0 -> {od}, expected 3]")
    if ok: passed += 1

    # Test 2: Output dim with padding
    total += 1
    od = compute_output_dim(5, 3, 1, 1)
    ok = od == 5
    print(f"{'PASS' if ok else 'FAIL'} [Output dim: 5,k=3,s=1,p=1 -> {od}, expected 5]")
    if ok: passed += 1

    # Test 3: Naive conv2d basic
    total += 1
    inp = [[1,2,3],[4,5,6],[7,8,9]]
    kernel = [[1,0],[0,1]]
    output = naive_conv2d(inp, kernel, 3, 3, 2, 2, 1, 0)
    ok = (abs(output[0][0]-6) < 1e-4 and abs(output[0][1]-8) < 1e-4 and
          abs(output[1][0]-12) < 1e-4 and abs(output[1][1]-14) < 1e-4)
    print(f"{'PASS' if ok else 'FAIL'} [Naive conv2d 3x3 input, 2x2 kernel]")
    if ok: passed += 1

    # Test 4: im2col matches naive
    total += 1
    random.seed(42)
    H, W, KH, KW = 6, 6, 3, 3
    inp = [[random.randint(0, 9) for _ in range(W)] for _ in range(H)]
    kernel = [[random.randint(0, 4) for _ in range(KW)] for _ in range(KH)]
    out_naive = naive_conv2d(inp, kernel, H, W, KH, KW, 1, 0)
    out_im2col = im2col_conv2d(inp, kernel, H, W, KH, KW, 1, 0)
    OH = compute_output_dim(H, KH, 1, 0)
    OW = compute_output_dim(W, KW, 1, 0)
    ok = matrices_close(out_naive, out_im2col, OH, OW)
    print(f"{'PASS' if ok else 'FAIL'} [im2col matches naive (6x6, 3x3 kernel)]")
    if ok: passed += 1

    # Test 5: With stride > 1
    total += 1
    H, W, KH, KW, stride = 6, 6, 3, 3, 2
    inp = [[float(i * W + j + 1) for j in range(W)] for i in range(H)]
    kernel = [[1.0] * KW for _ in range(KH)]
    OH = compute_output_dim(H, KH, stride, 0)
    OW = compute_output_dim(W, KW, stride, 0)
    out_naive = naive_conv2d(inp, kernel, H, W, KH, KW, stride, 0)
    out_im2col = im2col_conv2d(inp, kernel, H, W, KH, KW, stride, 0)
    ok = matrices_close(out_naive, out_im2col, OH, OW)
    print(f"{'PASS' if ok else 'FAIL'} [Stride=2, im2col matches naive]")
    if ok: passed += 1

    # Test 6: With padding
    total += 1
    H, W, KH, KW, stride, pad = 4, 4, 3, 3, 1, 1
    inp = [[1.0] * W for _ in range(H)]
    kernel = [[1.0] * KW for _ in range(KH)]
    OH = compute_output_dim(H, KH, stride, pad)
    OW = compute_output_dim(W, KW, stride, pad)
    out_naive = naive_conv2d(inp, kernel, H, W, KH, KW, stride, pad)
    out_im2col = im2col_conv2d(inp, kernel, H, W, KH, KW, stride, pad)
    ok = matrices_close(out_naive, out_im2col, OH, OW)
    ok = ok and abs(out_naive[0][0] - 4.0) < 1e-4
    print(f"{'PASS' if ok else 'FAIL'} [Padding=1, im2col matches naive, corner={out_naive[0][0]:.0f}]")
    if ok: passed += 1

    print(f"\nPassed {passed}/{total} tests")
    exit(0 if passed == total else 1)
