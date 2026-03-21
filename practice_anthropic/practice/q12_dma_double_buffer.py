"""
Q12 — DMA Double Buffering [Medium-Hard]

Topic: Kernel Optimization / Hardware Architecture

Problem:
  Simulate asynchronous DMA with double buffering to hide memory latency.

  Without double buffering (single buffer):
    For each tile: Load -> Compute -> Store (sequential)
    Total = num_tiles * (LOAD + COMPUTE + STORE)

  With double buffering (ping-pong buffers):
    Overlap load of next tile with compute of current tile.
    Total ~ LOAD + num_tiles * max(LOAD, COMPUTE, STORE) + COMPUTE + STORE

Constraints:
  - SRAM has space for exactly 2 tile buffers
  - DMA and compute can operate simultaneously on different buffers
  - Track total cycles for both approaches

Key Concepts:
  - Double buffering / ping-pong buffers
  - Latency hiding through overlapping compute and memory access
  - Prefetching strategies
"""

LOAD_LATENCY = 100
COMPUTE_LATENCY = 80
STORE_LATENCY = 100


class DMAStats:
    def __init__(self):
        self.total_cycles = 0
        self.compute_cycles = 0
        self.idle_cycles = 0
        self.num_tiles = 0


# ============ YOUR IMPLEMENTATION ============

def single_buffer_pipeline(num_tiles):
    """
    No overlapping — load, compute, store sequentially.
    total = num_tiles * (LOAD + COMPUTE + STORE)
    """
    stats = DMAStats()
    stats.num_tiles = num_tiles
    # TODO: Calculate total_cycles, compute_cycles, idle_cycles
    return stats


def double_buffer_pipeline(num_tiles):
    """
    Overlap load of next tile with compute of current tile.
    Phase 1: load first tile = LOAD_LATENCY
    Phase 2: (num_tiles - 1) * max(LOAD, COMPUTE, STORE)
    Phase 3: compute + store last tile
    """
    stats = DMAStats()
    stats.num_tiles = num_tiles
    # TODO: Calculate total_cycles, compute_cycles, idle_cycles
    return stats


def compute_speedup(num_tiles):
    """Return speedup of double buffer over single buffer."""
    # TODO: Return single_total / double_total
    return 0.0


def compute_utilization(stats):
    """Return fraction of time the compute unit is active."""
    # TODO: Return compute_cycles / total_cycles
    return 0.0


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q12: DMA Double Buffering ===\n")
    passed = 0
    total = 0

    # Test 1: Single buffer total cycles
    total += 1
    stats = single_buffer_pipeline(10)
    expected = 10 * (LOAD_LATENCY + COMPUTE_LATENCY + STORE_LATENCY)
    ok = stats.total_cycles == expected
    print(f"{'PASS' if ok else 'FAIL'} [Single buffer 10 tiles: {stats.total_cycles} cycles, expected {expected}]")
    if ok: passed += 1

    # Test 2: Double buffer should be faster
    total += 1
    single = single_buffer_pipeline(10)
    dbl = double_buffer_pipeline(10)
    ok = dbl.total_cycles < single.total_cycles
    print(f"{'PASS' if ok else 'FAIL'} [Double buffer faster: {dbl.total_cycles} vs {single.total_cycles} cycles]")
    if ok: passed += 1

    # Test 3: Double buffer utilization > single buffer utilization
    total += 1
    util_s = compute_utilization(single)
    util_d = compute_utilization(dbl)
    ok = util_d > util_s
    print(f"{'PASS' if ok else 'FAIL'} [Double buffer utilization {util_d*100:.1f}% > single {util_s*100:.1f}%]")
    if ok: passed += 1

    # Test 4: Speedup increases with more tiles
    total += 1
    speedup_5 = compute_speedup(5)
    speedup_100 = compute_speedup(100)
    ok = speedup_100 > speedup_5 and speedup_5 > 1.0
    print(f"{'PASS' if ok else 'FAIL'} [Speedup increases: 5 tiles={speedup_5:.2f}x, 100 tiles={speedup_100:.2f}x]")
    if ok: passed += 1

    # Test 5: Single tile — no overlap benefit
    total += 1
    single = single_buffer_pipeline(1)
    dbl = double_buffer_pipeline(1)
    ok = dbl.total_cycles >= single.total_cycles - 1
    print(f"{'PASS' if ok else 'FAIL'} [1 tile: double={dbl.total_cycles}, single={single.total_cycles}]")
    if ok: passed += 1

    # Test 6: Compute cycles same for both
    total += 1
    single = single_buffer_pipeline(10)
    dbl = double_buffer_pipeline(10)
    ok = single.compute_cycles == dbl.compute_cycles
    print(f"{'PASS' if ok else 'FAIL'} [Same compute work: single={single.compute_cycles}, double={dbl.compute_cycles}]")
    if ok: passed += 1

    print(f"\nPassed {passed}/{total} tests")
    exit(0 if passed == total else 1)
