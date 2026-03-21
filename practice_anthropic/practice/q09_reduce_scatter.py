"""
Q09 -- Reduce-Scatter with Recursive Halving [Medium]

Topic: Kernel Optimization -- Collective Communications

Problem:
  Implement reduce-scatter using the recursive halving algorithm.
  Unlike ring reduce-scatter (linear steps), recursive halving uses
  a tree topology to complete in O(log N) steps.

  After reduce-scatter, node i holds the i-th chunk of the globally
  reduced (summed) vector.

  Recursive Halving Algorithm:
    At each step, nodes pair up:
    - Step 0: nodes (0,N/2), (1,N/2+1), ... exchange data
    - Each pair: one node gets the reduced lower half, other gets reduced upper half
    - Recurse on each half independently
    - After log2(N) steps, each node has one fully-reduced chunk

Constraints:
  - N must be a power of 2
  - Vector length V must be divisible by N
  - Compare steps and bytes transferred vs ring algorithm

Key Concepts:
  - Tree-based collective algorithms
  - Recursive halving / doubling
  - Latency vs bandwidth tradeoffs in collectives
"""

import math


class ReduceScatterStats:
    def __init__(self):
        self.total_bytes_transferred = 0
        self.num_steps = 0  # communication rounds


# ============ YOUR IMPLEMENTATION ============

def recursive_halving_reduce_scatter(node_data, num_nodes):
    """
    Perform reduce-scatter using recursive halving.

    Input: node_data[i] is node i's local vector (length V).
    Output: node_data[i] contains just chunk i of the global sum
            (i.e., elements [i*chunk_size .. (i+1)*chunk_size) of sum(all vectors))
            Only the first chunk_size elements of node_data[i] are valid.

    Algorithm (for N=8, V=8):
    Step 1 (stride=4): Pairs (0,4),(1,5),(2,6),(3,7)
      - Node 0 sends upper half to node 4, receives node 4's upper half
      - Node 0 reduces lower half, node 4 reduces upper half
    Step 2 (stride=2): Pairs (0,2),(1,3) and (4,6),(5,7)
      - Continue halving within each group
    Step 3 (stride=1): Pairs (0,1),(2,3),(4,5),(6,7)
      - Final halving, each node has 1 chunk

    Args:
        node_data: list of lists of floats (num_nodes x V)
        num_nodes: int (must be power of 2)

    Returns:
        ReduceScatterStats with total_bytes_transferred and num_steps
    """
    stats = ReduceScatterStats()

    # TODO: Implement recursive halving reduce-scatter
    # V = len(node_data[0])
    # chunk_size = V // num_nodes
    #
    # For log2(num_nodes) steps:
    #   stride = num_nodes // 2, then //= 2
    #   For each pair of nodes separated by stride:
    #     Exchange appropriate half of active data
    #     Reduce (sum) the received half with local data
    #   Track bytes transferred and steps

    return stats


def ring_reduce_scatter(node_data, num_nodes):
    """
    For comparison -- linear reduce-scatter from ring all-reduce.
    N-1 steps, each node sends V/N elements per step.
    This is the same as Phase 1 from Q08.

    Args:
        node_data: list of lists of floats (num_nodes x V)
        num_nodes: int

    Returns:
        ReduceScatterStats with total_bytes_transferred and num_steps
    """
    stats = ReduceScatterStats()

    # TODO: Implement ring reduce-scatter (Phase 1 of ring all-reduce)

    return stats


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q09: Reduce-Scatter (Recursive Halving) ===\n")
    passed = 0
    total = 0

    # Test 1: 4 nodes, recursive halving correctness
    total += 1
    N, V = 4, 8
    data = [[0.0] * V for _ in range(N)]
    global_sum = [0.0] * V
    for i in range(N):
        for j in range(V):
            data[i][j] = float(i * 10 + j + 1)
            global_sum[j] += data[i][j]

    stats = recursive_halving_reduce_scatter(data, N)

    chunk_size = V // N
    ok = True
    for i in range(N):
        for j in range(chunk_size):
            expected = global_sum[i * chunk_size + j]
            if abs(data[i][j] - expected) > 1e-3:
                ok = False
                print(f"  Node {i}, elem {j}: got {data[i][j]:.1f}, expected {expected:.1f}")

    print(f"{'PASS' if ok else 'FAIL'} [4 nodes recursive halving correctness]")
    if ok:
        passed += 1

    # Test 2: Recursive halving uses log2(N) steps
    total += 1
    N, V = 8, 16
    data = [[1.0] * V for _ in range(N)]
    stats = recursive_halving_reduce_scatter(data, N)

    expected_steps = 3  # log2(8) = 3
    ok = stats.num_steps == expected_steps
    print(f"{'PASS' if ok else 'FAIL'} [8 nodes: {stats.num_steps} steps, expected {expected_steps} (log2)]")
    if ok:
        passed += 1

    # Test 3: Ring reduce-scatter uses N-1 steps
    total += 1
    N, V = 4, 8
    data = [[1.0] * V for _ in range(N)]
    stats = ring_reduce_scatter(data, N)

    expected_steps = N - 1
    ok = stats.num_steps == expected_steps
    print(f"{'PASS' if ok else 'FAIL'} [Ring: {stats.num_steps} steps, expected {expected_steps} (N-1)]")
    if ok:
        passed += 1

    # Test 4: Recursive halving fewer steps than ring for large N
    total += 1
    N, V = 8, 16
    data_rh = [[1.0] * V for _ in range(N)]
    data_ring = [[1.0] * V for _ in range(N)]

    stats_rh = recursive_halving_reduce_scatter(data_rh, N)
    stats_ring = ring_reduce_scatter(data_ring, N)

    ok = stats_rh.num_steps < stats_ring.num_steps
    print(f"{'PASS' if ok else 'FAIL'} [Recursive halving ({stats_rh.num_steps} steps) < Ring ({stats_ring.num_steps} steps)]")
    if ok:
        passed += 1

    # Test 5: 2 nodes (simplest case)
    total += 1
    N, V = 2, 4
    data = [[1.0, 2.0, 3.0, 4.0], [10.0, 20.0, 30.0, 40.0]]
    stats = recursive_halving_reduce_scatter(data, N)

    # Node 0 should have global_sum[0..1] = [11, 22]
    # Node 1 should have global_sum[2..3] = [33, 44]
    ok = (abs(data[0][0] - 11.0) < 1e-3 and
          abs(data[0][1] - 22.0) < 1e-3 and
          abs(data[1][0] - 33.0) < 1e-3 and
          abs(data[1][1] - 44.0) < 1e-3)
    print(f"{'PASS' if ok else 'FAIL'} [2 nodes: node0=[11,22], node1=[33,44]]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
