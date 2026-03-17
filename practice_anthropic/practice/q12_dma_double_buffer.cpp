/*
 * Q12 — DMA Double Buffering [Medium-Hard]
 *
 * Topic: Kernel Optimization / Hardware Architecture
 *
 * Problem:
 *   Simulate asynchronous DMA with double buffering to hide memory latency.
 *
 *   Without double buffering (single buffer):
 *     For each tile:
 *       1. DMA load tile (LOAD_LATENCY cycles) — compute unit idle
 *       2. Compute on tile (COMPUTE_LATENCY cycles) — DMA idle
 *     Total = num_tiles * (LOAD_LATENCY + COMPUTE_LATENCY)
 *
 *   With double buffering (ping-pong buffers):
 *     1. DMA load tile 0 into buffer A (LOAD_LATENCY cycles)
 *     2. For each remaining tile:
 *        - DMA load tile N+1 into buffer B (overlapped with compute)
 *        - Compute on tile N in buffer A (overlapped with load)
 *        - Swap A and B
 *     3. Compute on last tile (COMPUTE_LATENCY cycles)
 *     Total ≈ LOAD_LATENCY + num_tiles * max(LOAD_LATENCY, COMPUTE_LATENCY) + COMPUTE_LATENCY
 *
 * Constraints:
 *   - SRAM has space for exactly 2 tile buffers (ping and pong)
 *   - DMA and compute can operate simultaneously on different buffers
 *   - Track total cycles for both approaches
 *
 * Key Concepts:
 *   - Double buffering / ping-pong buffers
 *   - Latency hiding through overlapping compute and memory access
 *   - Prefetching strategies
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

using namespace std;

const int LOAD_LATENCY = 100;     // cycles to DMA load one tile
const int COMPUTE_LATENCY = 80;   // cycles to compute on one tile
const int STORE_LATENCY = 100;    // cycles to DMA store one tile

struct DMAStats {
    long long total_cycles;
    long long compute_cycles;    // cycles spent computing
    long long idle_cycles;       // cycles where compute unit is idle
    int num_tiles;
};

// ============ YOUR IMPLEMENTATION ============

/*
 * single_buffer_pipeline: No overlapping — load, compute, store sequentially.
 *
 * For each tile:
 *   Load:    LOAD_LATENCY cycles (compute idle)
 *   Compute: COMPUTE_LATENCY cycles
 *   Store:   STORE_LATENCY cycles (compute idle)
 */
DMAStats single_buffer_pipeline(int num_tiles) {
    DMAStats stats = {0, 0, 0, num_tiles};

    // TODO: Calculate total_cycles, compute_cycles, idle_cycles
    // total = num_tiles * (LOAD_LATENCY + COMPUTE_LATENCY + STORE_LATENCY)
    // compute = num_tiles * COMPUTE_LATENCY
    // idle = total - compute

    return stats;
}

/*
 * double_buffer_pipeline: Overlap load of next tile with compute of current tile.
 *
 * Timeline:
 *   Cycle 0:                  Load tile 0 (LOAD_LATENCY cycles)
 *   Cycle LOAD_LATENCY:       Compute tile 0 || Load tile 1
 *   Cycle LOAD + max(C,L):    Compute tile 1 || Load tile 2, Store tile 0
 *   ...
 *   After last tile computed:  Store last tile
 *
 * For simplicity, model as:
 *   Phase 1: Initial load (LOAD_LATENCY)
 *   Phase 2: (num_tiles - 1) iterations of max(LOAD_LATENCY, COMPUTE_LATENCY, STORE_LATENCY)
 *            (load next tile, compute current tile, store previous tile — all overlapped)
 *   Phase 3: Compute last tile + store last tile
 *
 * Compute utilization is much higher because compute rarely waits for loads.
 */
DMAStats double_buffer_pipeline(int num_tiles) {
    DMAStats stats = {0, 0, 0, num_tiles};

    // TODO: Calculate total_cycles, compute_cycles, idle_cycles
    // Phase 1: load first tile = LOAD_LATENCY (compute idle)
    // Phase 2: (num_tiles - 1) * max(LOAD, COMPUTE, STORE) (overlapped)
    // Phase 3: compute last tile + store last tile
    //   = COMPUTE_LATENCY + STORE_LATENCY (if not overlapped)
    //   or max(COMPUTE, STORE) (if overlapped)
    // compute_cycles = num_tiles * COMPUTE_LATENCY
    // idle = total - compute

    return stats;
}

/*
 * compute_speedup: Return speedup of double buffer over single buffer.
 */
float compute_speedup(int num_tiles) {
    // TODO: Return single_total / double_total
    return 0.0f;
}

/*
 * compute_utilization: Return fraction of time the compute unit is active.
 */
float compute_utilization(DMAStats stats) {
    // TODO: Return compute_cycles / total_cycles
    return 0.0f;
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q12: DMA Double Buffering ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Single buffer total cycles
    {
        total++;
        auto stats = single_buffer_pipeline(10);
        long long expected = 10LL * (LOAD_LATENCY + COMPUTE_LATENCY + STORE_LATENCY);
        bool ok = (stats.total_cycles == expected);
        printf("%s [Single buffer 10 tiles: %lld cycles, expected %lld]\n",
               ok ? "PASS" : "FAIL", stats.total_cycles, expected);
        if (ok) passed++;
    }

    // Test 2: Double buffer should be faster
    {
        total++;
        auto single = single_buffer_pipeline(10);
        auto dbl = double_buffer_pipeline(10);
        bool ok = (dbl.total_cycles < single.total_cycles);
        printf("%s [Double buffer faster: %lld vs %lld cycles]\n",
               ok ? "PASS" : "FAIL", dbl.total_cycles, single.total_cycles);
        if (ok) passed++;
    }

    // Test 3: Double buffer utilization > single buffer utilization
    {
        total++;
        auto single = single_buffer_pipeline(10);
        auto dbl = double_buffer_pipeline(10);
        float util_s = compute_utilization(single);
        float util_d = compute_utilization(dbl);
        bool ok = (util_d > util_s);
        printf("%s [Double buffer utilization %.1f%% > single %.1f%%]\n",
               ok ? "PASS" : "FAIL", util_d * 100, util_s * 100);
        if (ok) passed++;
    }

    // Test 4: Speedup should increase with more tiles (amortize startup)
    {
        total++;
        float speedup_5 = compute_speedup(5);
        float speedup_100 = compute_speedup(100);
        bool ok = (speedup_100 > speedup_5) && (speedup_5 > 1.0f);
        printf("%s [Speedup increases: 5 tiles=%.2fx, 100 tiles=%.2fx]\n",
               ok ? "PASS" : "FAIL", speedup_5, speedup_100);
        if (ok) passed++;
    }

    // Test 5: Single tile — double buffer has no benefit (same or slightly more)
    {
        total++;
        auto single = single_buffer_pipeline(1);
        auto dbl = double_buffer_pipeline(1);
        // With 1 tile, double buffer can't overlap anything
        bool ok = (dbl.total_cycles >= single.total_cycles - 1);  // allow small differences
        printf("%s [1 tile: double=%lld, single=%lld (no overlap possible)]\n",
               ok ? "PASS" : "FAIL", dbl.total_cycles, single.total_cycles);
        if (ok) passed++;
    }

    // Test 6: Compute cycles same for both
    {
        total++;
        auto single = single_buffer_pipeline(10);
        auto dbl = double_buffer_pipeline(10);
        bool ok = (single.compute_cycles == dbl.compute_cycles);
        printf("%s [Same compute work: single=%lld, double=%lld]\n",
               ok ? "PASS" : "FAIL", single.compute_cycles, dbl.compute_cycles);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
