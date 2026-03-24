"""
Q04 — Tiled Matrix Multiplication [Medium]

Topic: Kernel Optimization — Tiling

Problem:
  Implement tiled (blocked) matrix multiplication to improve cache/SRAM utilization.
  Compare against naive triple-loop matmul.

  In a naive matmul, each element of B is loaded from HBM many times.
  By tiling, we load sub-blocks into fast SRAM and reuse them,
  dramatically reducing HBM traffic.

Constraints:
  - Matrix dimensions may not be divisible by tile size
  - Track HBM load count for both versions
  - SRAM can hold 3 tiles: A_tile, B_tile, C_tile
  - Default tile size: 16x16

Complexity: Both O(M*K*N) FLOPs, but tiled has O(M*K*N / TILE) HBM loads
"""

import random

TILE_SIZE = 16


class MatmulStats:
    def __init__(self):
        self.hbm_loads = 0
        self.hbm_stores = 0
        self.flops = 0


# ============ YOUR IMPLEMENTATION ============

def naive_matmul(A, B, M, K, N, stats):
    """
    Standard triple-loop matrix multiply with load tracking.
    C[M][N] = A[M][K] * B[K][N]
    Every access to A and B counts as an HBM load.
    Every write to C counts as an HBM store.
    """
    C = [[0.0] * N for _ in range(M)]
    # TODO: Implement triple-loop matmul
    # Count each A[i][k] read, B[k][j] read as hbm_loads
    # Count each C[i][j] write as hbm_stores
    # Count each multiply-add as 2 flops
    for m in range(M):
        for n in range(N):
            acc = 0.0
            for k in range(K):
                stats.hbm_loads += 2  # A[m][k] and B[k][n]
                stats.flops += 2
                acc += A[m][k] * B[k][n]
            C[m][n] = acc
            stats.hbm_stores += 1  # C[m][n]
    return C


def tiled_matmul(A, B, M, K, N, stats):
    """
    Blocked matrix multiply with SRAM tile buffers.

    For each tile of output C (of size TILE_SIZE x TILE_SIZE):
      For each tile along the K dimension:
        1. Load A_tile from A into SRAM (count as 1 HBM load per element)
        2. Load B_tile from B into SRAM
        3. Compute C_tile += A_tile * B_tile using SRAM (no HBM access)
      Store C_tile back to C (count as HBM stores)

    Handle edge tiles where dimensions don't divide evenly by TILE_SIZE.
    """
    C = [[0.0] * N for _ in range(M)]
    stats.hbm_loads = 0
    stats.hbm_stores = 0
    stats.flops = 0

    # Fix M for C tile
    for m in range(0, M, TILE_SIZE):
        m_end = min(m + TILE_SIZE, M)
        # Fix N for C tile
        for n in range(0, N, TILE_SIZE):
            n_end = min(n + TILE_SIZE, N)
            # Intialize C tile (TILE_SIZE, TILE_SIZE)
            C_tile = [[0.0] * TILE_SIZE for _ in range(TILE_SIZE)]

            # Iterate over all K tiles
            for k in range(0, K, TILE_SIZE):
                k_end = min(k + TILE_SIZE, K)

                # Load A_tile (m_end-m x k_end-k) and B_tile (k_end-k x n_end-n)
                A_tile = [[0.0] * (k_end - k) for _ in range(m_end - m)]
                B_tile = [[0.0] * (n_end - n) for _ in range(k_end - k)]
                
                for i in range(m_end - m):
                    for j in range(k_end - k):
                        A_tile[i][j] = A[m + i][k + j]
                        stats.hbm_loads += 1
                
                for i in range(k_end - k):
                    for j in range(n_end - n):
                        B_tile[i][j] = B[k + i][n + j]
                        stats.hbm_loads += 1

                # Compute C_tile with the current A and B tile (classic matmul dot product)
                for mm in range(m_end - m):
                    for nn in range(n_end - n):
                        acc = 0.0
                        for kk in range(k_end - k):
                            acc += A_tile[mm][kk] * B_tile[kk][nn] 
                            stats.flops += 1
                        C_tile[mm][nn] += acc

            # Store C_tile in C, since C_tile holds the complete result for that tile
            for mm in range(m_end - m):
                for nn in range(n_end - n):
                    C[m + mm][n + nn] = C_tile[mm][nn]
                    stats.hbm_stores += 1
    return C


# ============ TEST FRAMEWORK ============

def matrices_equal(A, B, M, N, eps=1e-3):
    for i in range(M):
        for j in range(N):
            if abs(A[i][j] - B[i][j]) > eps:
                return False
    return True


if __name__ == "__main__":
    print("=== Q04: Tiled Matrix Multiplication ===\n")
    passed = 0
    total = 0

    random.seed(42)

    # Test 1: Square matrices (divisible by tile size)
    total += 1
    N = 64
    A = [[random.randint(0, 9) / 10.0 for _ in range(N)] for _ in range(N)]
    B = [[random.randint(0, 9) / 10.0 for _ in range(N)] for _ in range(N)]
    stats_naive = MatmulStats()
    stats_tiled = MatmulStats()
    C_naive = naive_matmul(A, B, N, N, N, stats_naive)
    C_tiled = tiled_matmul(A, B, N, N, N, stats_tiled)
    correct = matrices_equal(C_naive, C_tiled, N, N)
    print(f"{'PASS' if correct else 'FAIL'} [64x64 correctness]")
    if correct: passed += 1
    print(f"  Naive loads: {stats_naive.hbm_loads}, Tiled loads: {stats_tiled.hbm_loads}")

    # Test 2: Non-square, non-divisible by tile size
    total += 1
    M, K, N2 = 37, 43, 29
    A = [[random.randint(0, 9) for _ in range(K)] for _ in range(M)]
    B = [[random.randint(0, 9) for _ in range(N2)] for _ in range(K)]
    s1 = MatmulStats()
    s2 = MatmulStats()
    C_naive = naive_matmul(A, B, M, K, N2, s1)
    C_tiled = tiled_matmul(A, B, M, K, N2, s2)
    correct = matrices_equal(C_naive, C_tiled, M, N2)
    print(f"{'PASS' if correct else 'FAIL'} [37x43 * 43x29 correctness]")
    if correct: passed += 1

    # Test 3: Tiled should have fewer HBM loads
    total += 1
    N = 64
    A = [[1.0] * N for _ in range(N)]
    B = [[1.0] * N for _ in range(N)]
    s1 = MatmulStats()
    s2 = MatmulStats()
    naive_matmul(A, B, N, N, N, s1)
    tiled_matmul(A, B, N, N, N, s2)
    fewer = s2.hbm_loads < s1.hbm_loads
    print(f"{'PASS' if fewer else 'FAIL'} [Tiled has fewer HBM loads]")
    if fewer: passed += 1
    ratio = s1.hbm_loads / max(s2.hbm_loads, 1)
    print(f"  Reduction: {ratio:.1f}x fewer loads")

    # Test 4: Small matrix (smaller than tile size)
    total += 1
    A = [[1,2,3],[4,5,6],[7,8,9],[10,11,12]]
    B = [[1,2,3,4,5],[6,7,8,9,10],[11,12,13,14,15]]
    s1 = MatmulStats()
    s2 = MatmulStats()
    C_naive = naive_matmul(A, B, 4, 3, 5, s1)
    C_tiled = tiled_matmul(A, B, 4, 3, 5, s2)
    correct = matrices_equal(C_naive, C_tiled, 4, 5) and abs(C_naive[0][0] - 46.0) < 1e-3
    print(f"{'PASS' if correct else 'FAIL'} [Small matrix (4x3 * 3x5)]")
    if correct: passed += 1

    print(f"\nPassed {passed}/{total} tests")
    exit(0 if passed == total else 1)
