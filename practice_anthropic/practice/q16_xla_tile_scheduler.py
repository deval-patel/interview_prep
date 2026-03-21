"""
Q16 — XLA-Style Tile Scheduler [Medium-Hard]

Topic: ML Context — Compiler Infrastructure

Problem:
  Implement a tile operation scheduler that takes a DAG of tile operations
  and schedules them respecting:
  1. Data dependencies (operation B depends on output of operation A)
  2. SRAM capacity constraints (sum of live tile buffers <= SRAM capacity)

  This simulates what an XLA compiler backend does when lowering HLO
  operations to hardware tile operations.

  Implement:
  1. DAG construction with operations and dependencies
  2. Topological sort (Kahn's algorithm)
  3. Greedy scheduler with resource constraints

Constraints:
  - Each operation produces one output tile of known size
  - A tile is "live" from when it's produced until all consumers finish
  - SRAM capacity is limited

Key Concepts:
  - DAG scheduling
  - Resource-constrained scheduling
  - Topological ordering
  - Liveness analysis
"""

from collections import deque


class TileOp:
    def __init__(self, op_id, name, output_size, compute_cycles, deps=None):
        self.id = op_id
        self.name = name
        self.output_size = output_size
        self.compute_cycles = compute_cycles
        self.deps = deps or []
        self.consumers = []


class ScheduleEntry:
    def __init__(self, op_id, start_cycle, end_cycle):
        self.op_id = op_id
        self.start_cycle = start_cycle
        self.end_cycle = end_cycle


# ============ YOUR IMPLEMENTATION ============

class TileScheduler:
    def __init__(self, sram_capacity):
        self.ops = []
        self.sram_capacity = sram_capacity

    def add_op(self, name, output_size, compute_cycles, deps=None):
        """Add a tile operation to the DAG. Returns the operation ID."""
        # TODO: Create TileOp, add to ops list
        # Also update consumers list for dependency ops
        return -1

    def topological_sort(self):
        """
        Return a valid topological ordering of operations.
        Uses Kahn's algorithm (BFS with in-degree tracking).
        """
        # TODO: Implement Kahn's algorithm
        return []

    def schedule_greedy(self):
        """
        Schedule operations respecting dependencies and SRAM limits.

        1. Get topological order
        2. For each operation in order:
           a. Wait until all deps completed
           b. Check SRAM availability
           c. Schedule the operation
        3. Free tiles when all consumers complete
        """
        # TODO: Implement greedy scheduler
        return []

    def compute_total_cycles(self, schedule):
        """Return total execution time of the schedule."""
        # TODO: Return max end_cycle
        return 0

    def peak_sram_usage(self, schedule):
        """Return peak SRAM usage during execution."""
        # TODO: Track peak SRAM usage
        return 0


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q16: XLA-Style Tile Scheduler ===\n")
    passed = 0
    total = 0

    # Test 1: Linear chain (A -> B -> C)
    total += 1
    sched = TileScheduler(100)
    a = sched.add_op("A", 10, 5)
    b = sched.add_op("B", 20, 10, [a])
    c = sched.add_op("C", 15, 8, [b])
    order = sched.topological_sort()
    ok = len(order) == 3 and order[0] == a and order[1] == b and order[2] == c
    print(f"{'PASS' if ok else 'FAIL'} [Linear chain topological sort]")
    if ok: passed += 1

    # Test 2: Diamond DAG
    total += 1
    sched = TileScheduler(100)
    a = sched.add_op("A", 10, 5)
    b = sched.add_op("B", 10, 5, [a])
    c = sched.add_op("C", 10, 5, [a])
    d = sched.add_op("D", 10, 5, [b, c])
    order = sched.topological_sort()
    ok = len(order) == 4 and order[0] == a and order[3] == d
    print(f"{'PASS' if ok else 'FAIL'} [Diamond DAG topological sort]")
    if ok: passed += 1

    # Test 3: Schedule respects dependencies
    total += 1
    sched = TileScheduler(100)
    a = sched.add_op("A", 10, 5)
    b = sched.add_op("B", 10, 10, [a])
    schedule = sched.schedule_greedy()
    ok = len(schedule) == 2
    if ok:
        a_end = b_start = -1
        for e in schedule:
            if e.op_id == a: a_end = e.end_cycle
            if e.op_id == b: b_start = e.start_cycle
        ok = a_end >= 0 and b_start >= a_end
    print(f"{'PASS' if ok else 'FAIL'} [Schedule respects dependencies]")
    if ok: passed += 1

    # Test 4: SRAM constraint
    total += 1
    sched = TileScheduler(25)
    a = sched.add_op("A", 15, 5)
    b = sched.add_op("B", 15, 5)
    schedule = sched.schedule_greedy()
    peak = sched.peak_sram_usage(schedule)
    ok = peak <= 25
    print(f"{'PASS' if ok else 'FAIL'} [SRAM constraint respected: peak={peak}, capacity=25]")
    if ok: passed += 1

    # Test 5: Total cycles for linear chain
    total += 1
    sched = TileScheduler(100)
    sched.add_op("A", 10, 5)
    b = sched.add_op("B", 10, 10, [0])
    c = sched.add_op("C", 10, 8, [b])
    schedule = sched.schedule_greedy()
    total_cycles = sched.compute_total_cycles(schedule)
    ok = total_cycles >= 23  # 5 + 10 + 8
    print(f"{'PASS' if ok else 'FAIL'} [Total cycles = {total_cycles} (min 23)]")
    if ok: passed += 1

    # Test 6: Independent ops scheduled
    total += 1
    sched = TileScheduler(100)
    sched.add_op("A", 10, 5)
    sched.add_op("B", 10, 5)
    schedule = sched.schedule_greedy()
    ok = len(schedule) == 2
    print(f"{'PASS' if ok else 'FAIL'} [Two independent ops scheduled]")
    if ok: passed += 1

    print(f"\nPassed {passed}/{total} tests")
    exit(0 if passed == total else 1)
