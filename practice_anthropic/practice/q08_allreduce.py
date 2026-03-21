"""
Q08 -- All-Reduce Ring Algorithm [Medium-Hard]

Topic: Kernel Optimization -- Collective Communications

Problem:
  Simulate a ring all-reduce across N nodes. Each node starts with a local
  vector of values, and after all-reduce, every node should have the
  element-wise sum of all vectors.

  Ring all-reduce has two phases:
    Phase 1 -- Reduce-Scatter: Each node sends a chunk to the next node in the ring.
      After N-1 steps, each node holds one fully-reduced chunk.
    Phase 2 -- All-Gather: Each node broadcasts its fully-reduced chunk around the ring.
      After N-1 steps, every node has all fully-reduced chunks.

Constraints:
  - N nodes arranged in a ring (node i sends to node (i+1) % N)
  - Vector length V must be divisible by N (each chunk = V/N elements)
  - Count total data transferred (should be 2*(N-1)*(V/N) per node)

Key Concepts:
  - Ring all-reduce decomposition
  - Bandwidth-optimal collective communication
  - Used in distributed training (gradient averaging)
"""

import math


class AllReduceStats:
    def __init__(self):
        self.total_bytes_transferred = 0  # total across all nodes
        self.num_steps = 0                # total communication steps


# ============ YOUR IMPLEMENTATION ============

def ring_allreduce(node_data, num_nodes):
    """
    Perform ring all-reduce across num_nodes nodes.

    Input: node_data[i] is node i's local vector (all same length V).
    Output: After completion, every node_data[i] should contain the
            element-wise sum of all original vectors.

    Algorithm:

    Phase 1 -- Reduce-Scatter (N-1 steps):
      Divide each vector into N chunks of size V/N.
      At step s:
        - Node i sends chunk (i - s) % N to node (i + 1) % N
        - Node i receives chunk (i - s - 1) % N from node (i - 1) % N
        - Node i adds received chunk to its own chunk (i - s - 1) % N
      After N-1 steps, node i has fully reduced chunk i.

    Phase 2 -- All-Gather (N-1 steps):
      At step s:
        - Node i sends chunk (i - s + 1) % N to node (i + 1) % N
        - Node i receives chunk (i - s) % N from node (i - 1) % N
        - Node i copies received chunk (already fully reduced)
      After N-1 steps, all nodes have all fully reduced chunks.

    Args:
        node_data: list of lists of floats (num_nodes x V)
        num_nodes: int

    Returns:
        AllReduceStats with total_bytes_transferred and num_steps
    """
    stats = AllReduceStats()

    # TODO: Implement ring all-reduce
    # V = len(node_data[0])
    # chunk_size = V // num_nodes
    #
    # Phase 1: Reduce-Scatter
    #   for step in range(num_nodes - 1):
    #     for each node i:
    #       determine send_chunk and recv_chunk indices
    #       "send" data to (i+1) % num_nodes
    #       "receive" data from (i-1+num_nodes) % num_nodes
    #       add received data to local chunk
    #     update stats
    #
    # Phase 2: All-Gather
    #   for step in range(num_nodes - 1):
    #     for each node i:
    #       determine send_chunk and recv_chunk indices
    #       "send" fully reduced chunk to next node
    #       "receive" and store fully reduced chunk from prev node
    #     update stats

    return stats


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q08: All-Reduce Ring Algorithm ===\n")
    passed = 0
    total = 0

    # Test 1: 4 nodes, simple values
    total += 1
    N, V = 4, 8
    data = [[0.0] * V for _ in range(N)]
    expected = [0.0] * V
    for i in range(N):
        for j in range(V):
            data[i][j] = float(i * V + j + 1)
            expected[j] += data[i][j]

    stats = ring_allreduce(data, N)

    all_correct = True
    for i in range(N):
        for j in range(V):
            if abs(data[i][j] - expected[j]) > 1e-3:
                all_correct = False
                break

    print(f"{'PASS' if all_correct else 'FAIL'} [4 nodes, V=8: all nodes have global sum]")
    if all_correct:
        passed += 1
    if not all_correct:
        print(f"  Expected[0..3]: {expected[0]:.0f} {expected[1]:.0f} {expected[2]:.0f} {expected[3]:.0f}")
        print(f"  Node 0 got:     {data[0][0]:.0f} {data[0][1]:.0f} {data[0][2]:.0f} {data[0][3]:.0f}")

    # Test 2: All nodes should have identical results
    total += 1
    N, V = 3, 6
    data = [[0.0] * V for _ in range(N)]
    for i in range(N):
        for j in range(V):
            data[i][j] = float((i + 1) * (j + 1))

    ring_allreduce(data, N)

    identical = True
    for i in range(1, N):
        for j in range(V):
            if abs(data[i][j] - data[0][j]) > 1e-3:
                identical = False
                break

    print(f"{'PASS' if identical else 'FAIL'} [3 nodes: all results identical]")
    if identical:
        passed += 1

    # Test 3: Bandwidth optimal -- total transfer should be 2*(N-1)*V*4 bytes
    total += 1
    N, V = 4, 16
    data = [[1.0] * V for _ in range(N)]
    stats = ring_allreduce(data, N)

    # Each node sends (N-1) chunks of size V/N in each phase (2 phases)
    # Total per node = 2 * (N-1) * (V/N) * 4 (sizeof float)
    # Total across all nodes = N * 2 * (N-1) * (V/N) * 4 = 2*(N-1)*V*4
    expected_bytes = 2 * (N - 1) * V * 4  # 4 bytes per float
    ok = stats.total_bytes_transferred == expected_bytes
    print(f"{'PASS' if ok else 'FAIL'} [Bandwidth optimal: {stats.total_bytes_transferred} bytes, expected {expected_bytes}]")
    if ok:
        passed += 1

    # Test 4: Total communication steps should be 2*(N-1)
    total += 1
    N, V = 5, 10
    data = [[1.0] * V for _ in range(N)]
    stats = ring_allreduce(data, N)
    expected_steps = 2 * (N - 1)
    ok = stats.num_steps == expected_steps
    print(f"{'PASS' if ok else 'FAIL'} [Steps: {stats.num_steps}, expected {expected_steps}]")
    if ok:
        passed += 1

    # Test 5: 2 nodes (simplest ring)
    total += 1
    N, V = 2, 4
    data = [[1.0, 2.0, 3.0, 4.0], [10.0, 20.0, 30.0, 40.0]]
    expected = [11.0, 22.0, 33.0, 44.0]

    ring_allreduce(data, N)

    ok = True
    for i in range(N):
        for j in range(V):
            if abs(data[i][j] - expected[j]) > 1e-3:
                ok = False
                break

    print(f"{'PASS' if ok else 'FAIL'} [2 nodes: [1,2,3,4] + [10,20,30,40] = [11,22,33,44]]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
