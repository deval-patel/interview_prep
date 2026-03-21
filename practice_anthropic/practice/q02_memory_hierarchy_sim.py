"""
Q02 — Memory Hierarchy Simulator (HBM / SRAM) [Medium]

Topic: Hardware Architecture — Memory Hierarchy

Problem:
  Simulate a two-level memory system with:
  - HBM (High Bandwidth Memory): large, slow — 100 cycle latency per access
  - SRAM (on-chip scratchpad): small, fast — 1 cycle latency per access

  Implement two versions of vector addition (C[i] = A[i] + B[i]):
  1. Naive: each element loaded from HBM, computed, stored to HBM
  2. Tiled: load tiles into SRAM, compute from SRAM, store tile back to HBM

  Track total simulated cycles and HBM accesses to demonstrate
  the benefit of tiled execution.

Constraints:
  - SRAM capacity: 1024 floats
  - HBM latency: 100 cycles per load/store of a tile
  - SRAM latency: 1 cycle per element access
  - Tile size for tiled version: configurable, must fit 3 tiles in SRAM (A, B, C)

Key Concepts:
  - Arithmetic intensity (ops / byte transferred)
  - Data movement minimization
  - Explicit scratchpad memory management
"""


class MemorySystem:
    HBM_LATENCY = 100
    SRAM_LATENCY = 1
    SRAM_CAPACITY = 1024

    def __init__(self):
        self.total_cycles = 0
        self.hbm_accesses = 0
        self.hbm_bytes = 0
        self.sram = [0.0] * self.SRAM_CAPACITY

    def reset(self):
        self.total_cycles = 0
        self.hbm_accesses = 0
        self.hbm_bytes = 0

    def load_tile(self, hbm_src, src_offset, sram_offset, count):
        """Simulate loading 'count' floats from HBM into SRAM."""
        # TODO: Copy data from hbm_src[src_offset:] to sram[sram_offset:]
        # Account for HBM_LATENCY cycles and track hbm_accesses/hbm_bytes
        pass

    def store_tile(self, hbm_dst, dst_offset, sram_offset, count):
        """Simulate storing 'count' floats from SRAM back to HBM."""
        # TODO: Copy data from sram[sram_offset:] to hbm_dst[dst_offset:]
        # Account for HBM_LATENCY cycles and track hbm_accesses/hbm_bytes
        pass

    def sram_read(self, offset):
        """Read a single float from SRAM (1 cycle)."""
        # TODO: Return sram[offset] and account for SRAM_LATENCY
        return 0.0

    def sram_write(self, offset, val):
        """Write a single float to SRAM (1 cycle)."""
        # TODO: Write val to sram[offset] and account for SRAM_LATENCY
        pass


# ============ YOUR IMPLEMENTATION ============

def vector_add_naive(mem, hbm_A, hbm_B, hbm_C, N):
    """
    Per-element HBM access pattern.
    Each element of A and B is loaded individually from HBM,
    added, and the result stored individually to HBM.
    Cost: 3 * N * HBM_LATENCY cycles
    """
    # TODO: For each element:
    #   1. Load A[i] from HBM (use load_tile with count=1)
    #   2. Load B[i] from HBM
    #   3. Add them in SRAM
    #   4. Store result to HBM
    pass


def vector_add_tiled(mem, hbm_A, hbm_B, hbm_C, N):
    """
    Tiled access pattern with SRAM buffering.
    SRAM layout: [A_tile | B_tile | C_tile]
    tile_size = SRAM_CAPACITY / 3

    Cost: 3 * ceil(N/tile_size) * HBM_LATENCY + N * SRAM_LATENCY cycles
    """
    # TODO:
    # 1. Compute tile_size = SRAM_CAPACITY // 3
    # 2. For each tile:
    #    a. Load tile of A into sram[0..tile_size)
    #    b. Load tile of B into sram[tile_size..2*tile_size)
    #    c. Compute C_tile[i] = A_tile[i] + B_tile[i] in SRAM
    #    d. Store C_tile from sram[2*tile_size..3*tile_size) to HBM
    # 3. Handle last tile if N % tile_size != 0
    pass


def compute_arithmetic_intensity(N):
    """
    Calculate ops per byte transferred.
    For vector add: 1 FLOP per element, 12 bytes per element (load A, B; store C)
    """
    # TODO: Return FLOPs / bytes_transferred
    return 0.0


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q02: Memory Hierarchy Simulator ===\n")
    passed = 0
    total = 0

    N = 2048
    A = [float(i + 1) for i in range(N)]
    B = [float(N - i) for i in range(N)]
    C_expected = [A[i] + B[i] for i in range(N)]

    mem = MemorySystem()

    # Test 1: Naive correctness
    total += 1
    mem.reset()
    C_naive = [0.0] * N
    vector_add_naive(mem, A, B, C_naive, N)
    correct = all(abs(C_naive[i] - C_expected[i]) < 1e-5 for i in range(N))
    print(f"{'PASS' if correct else 'FAIL'} [Naive correctness]")
    if correct:
        passed += 1
    print(f"  Naive: {mem.total_cycles} cycles, {mem.hbm_accesses} HBM accesses")

    # Test 2: Tiled correctness
    total += 1
    mem.reset()
    C_tiled = [0.0] * N
    vector_add_tiled(mem, A, B, C_tiled, N)
    correct = all(abs(C_tiled[i] - C_expected[i]) < 1e-5 for i in range(N))
    print(f"{'PASS' if correct else 'FAIL'} [Tiled correctness]")
    if correct:
        passed += 1
    print(f"  Tiled: {mem.total_cycles} cycles, {mem.hbm_accesses} HBM accesses")

    # Test 3: Tiled should use fewer HBM accesses
    total += 1
    mem_naive = MemorySystem()
    mem_tiled = MemorySystem()
    cn = [0.0] * N
    ct = [0.0] * N
    vector_add_naive(mem_naive, A, B, cn, N)
    vector_add_tiled(mem_tiled, A, B, ct, N)
    fewer = mem_tiled.hbm_accesses < mem_naive.hbm_accesses
    print(f"{'PASS' if fewer else 'FAIL'} [Tiled uses fewer HBM accesses: {mem_tiled.hbm_accesses} vs {mem_naive.hbm_accesses}]")
    if fewer:
        passed += 1

    # Test 4: Tiled should use fewer total cycles
    total += 1
    faster = mem_tiled.total_cycles < mem_naive.total_cycles
    print(f"{'PASS' if faster else 'FAIL'} [Tiled is faster: {mem_tiled.total_cycles} vs {mem_naive.total_cycles} cycles]")
    if faster:
        passed += 1

    # Test 5: Arithmetic intensity
    total += 1
    ai = compute_arithmetic_intensity(N)
    expected_ai = 1.0 / 12.0
    ok = abs(ai - expected_ai) < 1e-6
    print(f"{'PASS' if ok else 'FAIL'} [Arithmetic intensity = {ai:.4f}, expected {expected_ai:.4f}]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
    exit(0 if passed == total else 1)
