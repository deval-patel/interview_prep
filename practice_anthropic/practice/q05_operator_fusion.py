"""
Q05 -- Operator Fusion (ReLU + Bias + MatMul) [Medium]

Topic: Kernel Optimization -- Operator Fusion

Problem:
  Compare unfused vs fused execution of: Y = ReLU(MatMul(A, B) + bias)

  Unfused pipeline (each step reads/writes HBM):
    1. T1 = MatMul(A, B)    -- write T1 to HBM
    2. T2 = T1 + bias       -- read T1, write T2 to HBM
    3. Y  = ReLU(T2)        -- read T2, write Y to HBM

  Fused pipeline (single kernel, intermediates stay in SRAM):
    1. For each tile of output:
       a. Compute tile of MatMul(A, B) in SRAM
       b. Add bias in SRAM
       c. Apply ReLU in SRAM
       d. Write final result to HBM

  Track HBM reads/writes to show fusion saves memory bandwidth.

Key Concepts:
  - Operator fusion eliminates intermediate HBM round-trips
  - In unfused: 2 extra HBM write + 2 extra HBM read for intermediates
  - Fusion is critical for TPU/GPU kernel performance
"""


class FusionStats:
    def __init__(self):
        self.hbm_reads = 0
        self.hbm_writes = 0

    def total_hbm_bytes(self):
        return (self.hbm_reads + self.hbm_writes) * 4  # sizeof(float) = 4


# ============ YOUR IMPLEMENTATION ============

def unfused_matmul_bias_relu(A, B, bias, M, K, N, stats):
    """
    unfused_matmul_bias_relu:
      Step 1: T1 = A * B           (read A, B from HBM; write T1 to HBM)
      Step 2: T2 = T1 + bias       (read T1, bias from HBM; write T2 to HBM)
      Step 3: Y  = ReLU(T2)        (read T2 from HBM; write Y to HBM)

    Count every element read/written from/to HBM.

    A: M x K matrix (list of lists)
    B: K x N matrix (list of lists)
    bias: list of length N
    Returns Y: M x N matrix (list of lists)

    TODO: Implement 3-step unfused pipeline
    Track all HBM reads and writes
    """
    Y = [[0.0] * N for _ in range(M)]
    return Y


def fused_matmul_bias_relu(A, B, bias, M, K, N, stats):
    """
    fused_matmul_bias_relu:
      For each output element Y[i][j]:
        1. Compute dot product A[i][:] * B[:][j] (reads from HBM)
        2. Add bias[j] (read from HBM once, or cache)
        3. Apply ReLU
        4. Write Y[i][j] to HBM

      Key: No intermediate matrices written to HBM!

    A: M x K matrix (list of lists)
    B: K x N matrix (list of lists)
    bias: list of length N
    Returns Y: M x N matrix (list of lists)

    TODO: Implement fused single-pass computation
    Only HBM reads: A, B, bias
    Only HBM writes: Y (final output)
    No intermediate buffers written to HBM
    """
    Y = [[0.0] * N for _ in range(M)]
    return Y


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    import random
    random.seed(42)

    print("=== Q05: Operator Fusion ===\n")
    passed = 0
    total = 0

    M, K, N = 32, 32, 32
    A = [[(random.randint(0, 199) - 100) / 100.0 for _ in range(K)] for _ in range(M)]
    B = [[(random.randint(0, 199) - 100) / 100.0 for _ in range(N)] for _ in range(K)]
    bias = [(random.randint(0, 99) - 50) / 100.0 for _ in range(N)]

    stats_unfused = FusionStats()
    stats_fused = FusionStats()

    Y_unfused = unfused_matmul_bias_relu(A, B, bias, M, K, N, stats_unfused)
    Y_fused = fused_matmul_bias_relu(A, B, bias, M, K, N, stats_fused)

    # Test 1: Both produce same result
    total += 1
    same = True
    for i in range(M):
        for j in range(N):
            if abs(Y_unfused[i][j] - Y_fused[i][j]) > 1e-3:
                same = False
                break
        if not same:
            break
    print(f"{'PASS' if same else 'FAIL'} [Unfused and fused produce same result]")
    if same:
        passed += 1

    # Test 2: All outputs are >= 0 (ReLU applied)
    total += 1
    ok = True
    for i in range(M):
        for j in range(N):
            if Y_fused[i][j] < -1e-7:
                ok = False
                break
        if not ok:
            break
    print(f"{'PASS' if ok else 'FAIL'} [All outputs >= 0 (ReLU)]")
    if ok:
        passed += 1

    # Test 3: Fused has fewer total HBM bytes
    total += 1
    fewer = stats_fused.total_hbm_bytes() < stats_unfused.total_hbm_bytes()
    print(f"{'PASS' if fewer else 'FAIL'} [Fused uses less HBM bandwidth]")
    if fewer:
        passed += 1
    print(f"  Unfused: {stats_unfused.hbm_reads} reads + {stats_unfused.hbm_writes} writes = {stats_unfused.total_hbm_bytes()} bytes")
    print(f"  Fused:   {stats_fused.hbm_reads} reads + {stats_fused.hbm_writes} writes = {stats_fused.total_hbm_bytes()} bytes")

    # Test 4: Fused has fewer HBM writes specifically
    total += 1
    fewer = stats_fused.hbm_writes < stats_unfused.hbm_writes
    print(f"{'PASS' if fewer else 'FAIL'} [Fused has fewer HBM writes: {stats_fused.hbm_writes} vs {stats_unfused.hbm_writes}]")
    if fewer:
        passed += 1

    # Test 5: Simple known result
    total += 1
    a = [[1, 2], [3, 4]]
    b = [[1, 0], [0, 1]]
    bi = [-5, 0]
    s = FusionStats()
    y = fused_matmul_bias_relu(a, b, bi, 2, 2, 2, s)
    # MatMul: [[1,2],[3,4]], +bias[-5,0]: [[-4,2],[-2,4]], ReLU: [[0,2],[0,4]]
    ok = (abs(y[0][0]) < 1e-5 and abs(y[0][1] - 2.0) < 1e-5 and
          abs(y[1][0]) < 1e-5 and abs(y[1][1] - 4.0) < 1e-5)
    print(f"{'PASS' if ok else 'FAIL'} [Simple 2x2 known result]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
