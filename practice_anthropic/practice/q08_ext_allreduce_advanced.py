"""
Q08-EXT — Advanced All-Reduce: Topologies, Bandwidth & Fault Tolerance [Hard]

Topic: Distributed Systems — Collective Communications

Problem:
  Extend the ring all-reduce to support multiple topologies, realistic
  bandwidth/latency modeling, and node failure recovery.

  Implement:
  1. ring_allreduce(node_data, num_nodes) — baseline ring (from Q08)
  2. tree_allreduce(node_data, num_nodes) — binary tree reduce + broadcast
  3. butterfly_allreduce(node_data, num_nodes) — recursive doubling (power-of-2 nodes)
  4. BandwidthModel — track latency + bandwidth costs for each algorithm
  5. FaultTolerantAllReduce — handle node failures by detecting and excluding
     failed nodes, then running allreduce on surviving nodes

  For each algorithm, track:
  - Total data transferred (bytes, assuming 4 bytes per float)
  - Number of communication steps (latency-sensitive metric)
  - Wall-clock cost under a bandwidth model: cost = steps * alpha + bytes_per_step * beta
    where alpha = per-message latency, beta = per-byte transfer cost

  Algorithm details:

  Tree All-Reduce:
    Phase 1 (Reduce): Binary tree from leaves to root.
      At each level, half the nodes send their full vector to a partner.
      The partner adds element-wise. After log2(N) steps, root has global sum.
    Phase 2 (Broadcast): Root sends result down the tree.
      After log2(N) steps, all nodes have the result.
    Steps: 2 * log2(N)
    Data per step: V * 4 bytes (full vector)

  Butterfly (Recursive Doubling):
    At step k (0-indexed), node i communicates with node i XOR 2^k.
    Both nodes exchange their full vectors and add element-wise.
    After log2(N) steps, all nodes have the global sum.
    Steps: log2(N)
    Data per step: V * 4 bytes (each node sends full vector)

Constraints:
  - Ring works for any N >= 2
  - Tree and butterfly require N to be a power of 2
  - Bandwidth model: alpha >= 0, beta >= 0
  - Fault tolerance: at least 2 nodes must survive for allreduce to proceed

Key Concepts:
  - Latency-optimal (tree/butterfly) vs bandwidth-optimal (ring) trade-offs
  - Alpha-beta cost model for collective communication
  - Fault tolerance via node exclusion and re-mapping
  - Real-world choice depends on message size and network topology
"""

import math
from collections import defaultdict


class CommStats:
    """Communication statistics for an all-reduce operation."""
    def __init__(self):
        self.total_bytes = 0        # total bytes transferred across all nodes
        self.num_steps = 0          # number of synchronous communication rounds
        self.bytes_per_step = []    # bytes transferred in each step


class BandwidthModel:
    """
    Alpha-beta model for communication cost.
    Total cost = num_steps * alpha + total_bytes * beta

    alpha = latency per message (microseconds)
    beta  = transfer cost per byte (microseconds/byte)
    """

    def __init__(self, alpha=10.0, beta=0.001):
        """
        Args:
            alpha: per-message latency in microseconds
            beta: per-byte cost in microseconds
        """
        self.alpha = alpha
        self.beta = beta

    def cost(self, stats):
        """
        Compute total communication cost from CommStats.

        Args:
            stats: CommStats object

        Returns:
            float: total cost in microseconds
        """
        # TODO: cost = steps * alpha + total_bytes * beta
        pass

    def compare(self, stats_dict):
        """
        Compare costs across multiple algorithms.

        Args:
            stats_dict: dict of algorithm_name -> CommStats

        Returns:
            dict of algorithm_name -> cost (float, microseconds)
        """
        # TODO: Compute cost for each algorithm and return dict
        pass


# ============ YOUR IMPLEMENTATION ============

def ring_allreduce_ext(node_data, num_nodes):
    """
    Ring all-reduce (same as Q08 but returning CommStats).

    Algorithm:
      Phase 1 (Reduce-Scatter, N-1 steps):
        Node i sends chunk (i-s)%N to node (i+1)%N,
        receives chunk (i-s-1)%N from (i-1)%N, adds to local.
      Phase 2 (All-Gather, N-1 steps):
        Node i sends chunk (i-s+1)%N to (i+1)%N,
        receives chunk (i-s)%N from (i-1)%N, copies.

    Each step transfers N chunks of size (V/N)*4 bytes (one per node).

    Args:
        node_data: list of lists (num_nodes x V), modified in place
        num_nodes: int

    Returns:
        CommStats
    """
    stats = CommStats()
    # TODO: Implement ring all-reduce
    # Track bytes per step: each step, every node sends V/N floats = V/N * 4 bytes
    # Total per step across all nodes = N * (V/N) * 4 = V * 4
    return stats


def tree_allreduce(node_data, num_nodes):
    """
    Binary tree all-reduce. N must be power of 2.

    Phase 1 (Reduce, log2(N) steps):
      Step k: stride = 2^(k+1)
      Active nodes: multiples of stride
      Node i receives from node i + stride/2, adds vectors element-wise
      After log2(N) steps, node 0 has global sum.

    Phase 2 (Broadcast, log2(N) steps):
      Reverse of reduce. Node 0 sends down the tree.
      Step k (counting from top): stride = 2^(log2(N) - k)
      Active senders: multiples of stride
      Node i sends full vector to node i + stride/2

    Args:
        node_data: list of lists (num_nodes x V), modified in place
        num_nodes: int (must be power of 2)

    Returns:
        CommStats
    """
    stats = CommStats()
    # TODO: Implement tree all-reduce
    # Each step transfers full vectors: V * 4 bytes per (sender, receiver) pair
    # Number of pairs per step varies by level
    return stats


def butterfly_allreduce(node_data, num_nodes):
    """
    Butterfly (recursive doubling) all-reduce. N must be power of 2.

    At step k (0 to log2(N)-1):
      Node i exchanges full vector with node i XOR 2^k.
      Both nodes add the received vector to their own.

    After log2(N) steps, all nodes have the global sum.

    Each step: every node sends V*4 bytes -> N*V*4 bytes total per step.

    Args:
        node_data: list of lists (num_nodes x V), modified in place
        num_nodes: int (must be power of 2)

    Returns:
        CommStats
    """
    stats = CommStats()
    # TODO: Implement butterfly all-reduce
    return stats


class FaultTolerantAllReduce:
    """
    All-reduce that handles node failures.

    When a node fails:
    1. Detect which nodes are alive
    2. Remap alive nodes to contiguous indices [0, num_alive)
    3. Run ring all-reduce on surviving nodes
    4. Map results back to original node indices (failed nodes get None)
    """

    def __init__(self, num_nodes):
        self.num_nodes = num_nodes
        self.failed_nodes = set()

    def fail_node(self, node_id):
        """Mark a node as failed."""
        # TODO
        pass

    def recover_node(self, node_id):
        """Mark a node as recovered (back online)."""
        # TODO
        pass

    def get_alive_nodes(self):
        """Return sorted list of alive node IDs."""
        # TODO
        pass

    def execute(self, node_data):
        """
        Run fault-tolerant all-reduce.

        Args:
            node_data: list of (num_nodes) vectors. Failed nodes' data is ignored.

        Returns:
            tuple: (results, stats)
            - results: list of length num_nodes.
              results[i] = reduced vector if node i is alive, None if failed.
            - stats: CommStats from the underlying all-reduce

        Raises:
            ValueError if fewer than 2 nodes are alive.
        """
        # TODO:
        # 1. Get alive nodes
        # 2. Validate at least 2 alive
        # 3. Build compact node_data for alive nodes only
        # 4. Run ring_allreduce_ext on compact data
        # 5. Map results back: alive nodes get the result, failed get None
        pass


# ============ TEST FRAMEWORK ============

def _run_test(name, fn):
    """Run a single test, catching exceptions from unimplemented stubs."""
    try:
        return fn()
    except Exception as e:
        print(f"FAIL [{name}] — {type(e).__name__}: {e}")
        return False


passed = 0
total = 0

if __name__ == "__main__":
    print("=== Q08-EXT: Advanced All-Reduce ===\n")

    def _tests():
        global passed, total

        total += 1
        def t1():
            N, V = 4, 8
            data = [[float(i * V + j + 1) for j in range(V)] for i in range(N)]
            expected = [sum(data[i][j] for i in range(N)) for j in range(V)]
            ring_allreduce_ext(data, N)
            ok = all(abs(data[n][j] - expected[j]) < 1e-6 for n in range(N) for j in range(V))
            print(f"{'PASS' if ok else 'FAIL'} [Ring: 4 nodes, V=8 correctness]")
            return ok
        if _run_test("Ring correctness", t1): passed += 1

        total += 1
        def t2():
            N, V = 4, 16
            data = [[1.0] * V for _ in range(N)]
            stats = ring_allreduce_ext(data, N)
            expected_steps = 2 * (N - 1)
            expected_bytes = 2 * (N - 1) * V * 4
            ok = (stats.num_steps == expected_steps and stats.total_bytes == expected_bytes)
            print(f"{'PASS' if ok else 'FAIL'} [Ring stats: steps={stats.num_steps}(exp {expected_steps}), "
                  f"bytes={stats.total_bytes}(exp {expected_bytes})]")
            return ok
        if _run_test("Ring stats", t2): passed += 1

        total += 1
        def t3():
            N, V = 8, 4
            data = [[float(i + 1)] * V for i in range(N)]
            expected_val = sum(range(1, N + 1))
            tree_allreduce(data, N)
            ok = all(abs(data[n][j] - expected_val) < 1e-6 for n in range(N) for j in range(V))
            print(f"{'PASS' if ok else 'FAIL'} [Tree: 8 nodes, each element should be {expected_val}]")
            return ok
        if _run_test("Tree correctness", t3): passed += 1

        total += 1
        def t4():
            N, V = 8, 4
            data = [[1.0] * V for _ in range(N)]
            stats = tree_allreduce(data, N)
            expected_steps = 2 * int(math.log2(N))
            ok = stats.num_steps == expected_steps
            print(f"{'PASS' if ok else 'FAIL'} [Tree stats: steps={stats.num_steps}(exp {expected_steps})]")
            return ok
        if _run_test("Tree stats", t4): passed += 1

        total += 1
        def t5():
            N, V = 4, 6
            data = [[float(i * 10 + j) for j in range(V)] for i in range(N)]
            expected = [sum(data[i][j] for i in range(N)) for j in range(V)]
            butterfly_allreduce(data, N)
            ok = all(abs(data[n][j] - expected[j]) < 1e-6 for n in range(N) for j in range(V))
            print(f"{'PASS' if ok else 'FAIL'} [Butterfly: 4 nodes, V=6 correctness]")
            return ok
        if _run_test("Butterfly correctness", t5): passed += 1

        total += 1
        def t6():
            N, V = 8, 4
            data = [[1.0] * V for _ in range(N)]
            stats = butterfly_allreduce(data, N)
            expected_steps = int(math.log2(N))
            ok = stats.num_steps == expected_steps
            print(f"{'PASS' if ok else 'FAIL'} [Butterfly stats: steps={stats.num_steps}(exp {expected_steps})]")
            return ok
        if _run_test("Butterfly stats", t6): passed += 1

        total += 1
        def t7():
            N, V = 16, 1024
            bw = BandwidthModel(alpha=10.0, beta=0.001)
            data_r = [[1.0] * V for _ in range(N)]
            data_t = [[1.0] * V for _ in range(N)]
            data_b = [[1.0] * V for _ in range(N)]
            s_r = ring_allreduce_ext(data_r, N)
            s_t = tree_allreduce(data_t, N)
            s_b = butterfly_allreduce(data_b, N)
            costs = bw.compare({"ring": s_r, "tree": s_t, "butterfly": s_b})
            ok = all(c > 0 for c in costs.values())
            print(f"{'PASS' if ok else 'FAIL'} [BW model: ring={costs['ring']:.1f}us, "
                  f"tree={costs['tree']:.1f}us, butterfly={costs['butterfly']:.1f}us]")
            return ok
        if _run_test("BW model", t7): passed += 1

        total += 1
        def t8():
            N, V = 16, 4
            bw = BandwidthModel(alpha=100.0, beta=0.0001)
            data_r = [[1.0] * V for _ in range(N)]
            data_b = [[1.0] * V for _ in range(N)]
            s_r = ring_allreduce_ext(data_r, N)
            s_b = butterfly_allreduce(data_b, N)
            costs = bw.compare({"ring": s_r, "butterfly": s_b})
            ok = costs["butterfly"] < costs["ring"]
            print(f"{'PASS' if ok else 'FAIL'} [Latency-dominated: butterfly({costs['butterfly']:.1f}) "
                  f"< ring({costs['ring']:.1f})]")
            return ok
        if _run_test("Latency tradeoff", t8): passed += 1

        total += 1
        def t9():
            N, V = 4, 4
            ft = FaultTolerantAllReduce(N)
            data = [[float(i + 1)] * V for i in range(N)]
            ft.fail_node(2)
            results, stats = ft.execute(data)
            ok = (results[2] is None and
                  all(abs(results[i][j] - 7.0) < 1e-6 for i in [0, 1, 3] for j in range(V)))
            print(f"{'PASS' if ok else 'FAIL'} [Fault tolerant: node 2 failed, alive sum=7]")
            return ok
        if _run_test("Fault tolerant", t9): passed += 1

        total += 1
        def t10():
            N, V = 4, 4
            ft = FaultTolerantAllReduce(N)
            ft.fail_node(2)
            ft.recover_node(2)
            data = [[float(i + 1)] * V for i in range(N)]
            results, stats = ft.execute(data)
            ok = all(abs(results[i][j] - 10.0) < 1e-6 for i in range(N) for j in range(V))
            print(f"{'PASS' if ok else 'FAIL'} [Recovered: all 4 nodes, sum=10]")
            return ok
        if _run_test("Recovery", t10): passed += 1

        total += 1
        def t11():
            ft2 = FaultTolerantAllReduce(3)
            ft2.fail_node(0)
            ft2.fail_node(1)
            data = [[1.0] * 4 for _ in range(3)]
            try:
                ft2.execute(data)
                ok = False
            except ValueError:
                ok = True
            print(f"{'PASS' if ok else 'FAIL'} [Too many failures raises ValueError]")
            return ok
        if _run_test("Failure threshold", t11): passed += 1

    _tests()
    print(f"\nPassed {passed}/{total} tests")
