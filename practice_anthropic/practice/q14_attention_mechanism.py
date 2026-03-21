"""
Q14 -- Scaled Dot-Product Attention [Hard]

Topic: ML Context -- Transformer Architecture

Problem:
  Implement scaled dot-product attention used in transformers:
    Attention(Q, K, V) = softmax(Q * K^T / sqrt(d_k)) * V

  Two implementations:
  1. Naive: Materializes the full NxN attention matrix (O(N^2) memory)
  2. Tiled: Processes attention in blocks to reduce peak memory (Flash Attention concept)

  Also support causal masking (lower-triangular mask) for autoregressive models.

Constraints:
  - Q, K, V are (seq_len x d_model) matrices (list of lists)
  - Track peak memory usage (bytes) for each implementation
  - Causal mask: position i can only attend to positions <= i

Key Concepts:
  - Attention mechanism internals
  - O(N^2) memory bottleneck
  - Flash Attention tiling strategy
  - Causal masking for autoregressive generation
"""

import math
import random


class AttentionStats:
    def __init__(self):
        self.peak_memory_bytes = 0  # peak additional memory beyond Q, K, V, O
        self.flops = 0


def row_softmax(row):
    """Helper: stable softmax on a row (list of floats). Returns new list."""
    max_val = max(row)
    exps = [math.exp(v - max_val) for v in row]
    s = sum(exps)
    return [e / s for e in exps]


# ============ YOUR IMPLEMENTATION ============

def naive_attention(Q, K, V, seq_len, d_model, causal, stats):
    """
    naive_attention: Standard attention with full NxN matrix materialization.

    Steps:
      1. Compute S = Q * K^T / sqrt(d_k)  -- (seq_len x seq_len) matrix
      2. Apply causal mask (if enabled): S[i][j] = -inf for j > i
      3. P = softmax(S, dim=-1)           -- row-wise softmax
      4. O = P * V                        -- (seq_len x d_model) output

    Peak memory: O(seq_len^2) for the S/P matrix.

    Args:
        Q: list[list[float]], shape [seq_len][d_model]
        K: list[list[float]], shape [seq_len][d_model]
        V: list[list[float]], shape [seq_len][d_model]
        seq_len: int
        d_model: int
        causal: bool
        stats: AttentionStats (set peak_memory_bytes)

    Returns:
        O: list[list[float]], shape [seq_len][d_model]
    """
    # TODO: Implement naive attention
    # Track peak memory as seq_len * seq_len * 4 (bytes) for attention matrix
    O = [[0.0] * d_model for _ in range(seq_len)]
    return O


def tiled_attention(Q, K, V, seq_len, d_model, causal, block_size, stats):
    """
    tiled_attention: Block-wise attention to reduce peak memory.

    Instead of materializing the full NxN attention matrix:
      For each block of query rows (block_q):
        For each block of key/value rows (block_kv):
          1. Compute partial attention scores for this block pair
          2. Track running max and sum for online softmax correction
          3. Accumulate weighted values into output

    Peak memory: O(block_size^2) instead of O(N^2)

    This is a simplified version of the Flash Attention algorithm.

    Args:
        Q: list[list[float]], shape [seq_len][d_model]
        K: list[list[float]], shape [seq_len][d_model]
        V: list[list[float]], shape [seq_len][d_model]
        seq_len: int
        d_model: int
        causal: bool
        block_size: int
        stats: AttentionStats (set peak_memory_bytes)

    Returns:
        O: list[list[float]], shape [seq_len][d_model]
    """
    # TODO: Implement tiled attention (simplified Flash Attention)
    #
    # For each query block bq (rows bq*BS to (bq+1)*BS):
    #   Initialize: row_max[i] = -inf, row_sum[i] = 0, O[i] = 0
    #   For each KV block bkv (rows bkv*BS to (bkv+1)*BS):
    #     1. Compute S_block = Q_block * K_block^T / sqrt(d_k)
    #     2. Apply causal mask if needed
    #     3. Find new max, compute exp(), new sum
    #     4. Rescale previous O accumulation if max changed
    #     5. Accumulate: O += P_block * V_block
    #   Normalize: O[i] /= row_sum[i]
    #
    # Peak memory: block_size * block_size * 4 (bytes)
    O = [[0.0] * d_model for _ in range(seq_len)]
    return O


# ============ TEST FRAMEWORK ============

def matrices_close(A, B, M, N, tol=1e-3):
    for i in range(M):
        for j in range(N):
            if abs(A[i][j] - B[i][j]) > tol:
                return False
    return True


if __name__ == "__main__":
    print("=== Q14: Scaled Dot-Product Attention ===\n")
    passed = 0
    total = 0

    random.seed(42)

    # Test 1: Naive attention basic (no causal mask)
    total += 1
    S, D = 8, 4
    Q = [[(random.randint(0, 99) - 50) / 50.0 for _ in range(D)] for _ in range(S)]
    K = [[(random.randint(0, 99) - 50) / 50.0 for _ in range(D)] for _ in range(S)]
    V = [[(random.randint(0, 99) - 50) / 50.0 for _ in range(D)] for _ in range(S)]

    stats = AttentionStats()
    O = naive_attention(Q, K, V, S, D, False, stats)

    # Check that output rows are non-zero
    ok = True
    for i in range(S):
        norm = sum(v * v for v in O[i])
        if norm < 1e-10:
            ok = False
    print(f"{'PASS' if ok else 'FAIL'} [Naive attention, seq_len={S}, d={D}]")
    if ok:
        passed += 1

    # Test 2: Tiled matches naive (no causal)
    total += 1
    S, D = 16, 8
    Q = [[(random.randint(0, 99) - 50) / 100.0 for _ in range(D)] for _ in range(S)]
    K = [[(random.randint(0, 99) - 50) / 100.0 for _ in range(D)] for _ in range(S)]
    V = [[(random.randint(0, 99) - 50) / 100.0 for _ in range(D)] for _ in range(S)]

    s1 = AttentionStats()
    s2 = AttentionStats()
    O_naive = naive_attention(Q, K, V, S, D, False, s1)
    O_tiled = tiled_attention(Q, K, V, S, D, False, 4, s2)

    ok = matrices_close(O_naive, O_tiled, S, D, tol=1e-2)
    print(f"{'PASS' if ok else 'FAIL'} [Tiled matches naive (no causal)]")
    if ok:
        passed += 1

    # Test 3: Causal masking
    total += 1
    S, D = 8, 4
    Q = [[1.0] * D for _ in range(S)]
    K = [[1.0] * D for _ in range(S)]
    V = [[float(i + 1)] * D for i in range(S)]

    stats = AttentionStats()
    O = naive_attention(Q, K, V, S, D, True, stats)

    # With causal mask, row 0 can only attend to position 0
    # So O[0] should be close to V[0]
    ok = True
    for j in range(D):
        if abs(O[0][j] - V[0][j]) > 1e-3:
            ok = False
    print(f"{'PASS' if ok else 'FAIL'} [Causal: row 0 attends only to position 0]")
    if ok:
        passed += 1

    # Test 4: Tiled matches naive with causal mask
    total += 1
    S, D = 16, 8
    Q = [[(random.randint(0, 99) - 50) / 100.0 for _ in range(D)] for _ in range(S)]
    K = [[(random.randint(0, 99) - 50) / 100.0 for _ in range(D)] for _ in range(S)]
    V = [[(random.randint(0, 99) - 50) / 100.0 for _ in range(D)] for _ in range(S)]

    s1 = AttentionStats()
    s2 = AttentionStats()
    O_naive = naive_attention(Q, K, V, S, D, True, s1)
    O_tiled = tiled_attention(Q, K, V, S, D, True, 4, s2)

    ok = matrices_close(O_naive, O_tiled, S, D, tol=1e-2)
    print(f"{'PASS' if ok else 'FAIL'} [Tiled matches naive (causal)]")
    if ok:
        passed += 1

    # Test 5: Tiled uses less peak memory
    total += 1
    S, D = 32, 16
    Q = [[0.1] * D for _ in range(S)]
    K = [[0.1] * D for _ in range(S)]
    V = [[0.1] * D for _ in range(S)]

    s1 = AttentionStats()
    s2 = AttentionStats()
    O1 = naive_attention(Q, K, V, S, D, False, s1)
    O2 = tiled_attention(Q, K, V, S, D, False, 8, s2)

    ok = s2.peak_memory_bytes < s1.peak_memory_bytes
    print(f"{'PASS' if ok else 'FAIL'} [Tiled peak memory {s2.peak_memory_bytes} < naive {s1.peak_memory_bytes} bytes]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
