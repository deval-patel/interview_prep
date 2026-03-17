/*
 * Q02 — Memory Hierarchy Simulator (HBM / SRAM) [Medium]
 *
 * Topic: Hardware Architecture — Memory Hierarchy
 *
 * Problem:
 *   Simulate a two-level memory system with:
 *   - HBM (High Bandwidth Memory): large, slow — 100 cycle latency per access
 *   - SRAM (on-chip scratchpad): small, fast — 1 cycle latency per access
 *
 *   Implement two versions of vector addition (C[i] = A[i] + B[i]):
 *   1. Naive: each element loaded from HBM, computed, stored to HBM
 *   2. Tiled: load tiles into SRAM, compute from SRAM, store tile back to HBM
 *
 *   Track total simulated cycles and HBM accesses to demonstrate
 *   the benefit of tiled execution.
 *
 * Constraints:
 *   - SRAM capacity: 1024 floats (can hold parts of A, B, C tiles)
 *   - HBM latency: 100 cycles per load/store of a tile
 *   - SRAM latency: 1 cycle per element access
 *   - Tile size for tiled version: configurable, must fit 3 tiles in SRAM (A, B, C)
 *
 * Key Concepts:
 *   - Arithmetic intensity (ops / byte transferred)
 *   - Data movement minimization
 *   - Explicit scratchpad memory management
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>

using namespace std;

// Simulated memory system
struct MemorySystem {
    long long total_cycles;
    long long hbm_accesses;   // number of HBM load/store operations
    long long hbm_bytes;      // total bytes moved to/from HBM

    static const int HBM_LATENCY = 100;    // cycles per HBM tile transfer
    static const int SRAM_LATENCY = 1;     // cycles per SRAM element access
    static const int SRAM_CAPACITY = 1024; // max floats in SRAM

    vector<float> sram;  // on-chip SRAM buffer

    MemorySystem() : total_cycles(0), hbm_accesses(0), hbm_bytes(0),
                     sram(SRAM_CAPACITY, 0.0f) {}

    void reset() {
        total_cycles = 0;
        hbm_accesses = 0;
        hbm_bytes = 0;
    }

    // Simulate loading 'count' floats from HBM into SRAM starting at sram_offset
    void load_tile(const float* hbm_src, int sram_offset, int count) {
        // TODO: Copy data from hbm_src to sram[sram_offset..sram_offset+count)
        // Account for HBM_LATENCY cycles and track hbm_accesses/hbm_bytes
    }

    // Simulate storing 'count' floats from SRAM back to HBM
    void store_tile(float* hbm_dst, int sram_offset, int count) {
        // TODO: Copy data from sram[sram_offset..sram_offset+count) to hbm_dst
        // Account for HBM_LATENCY cycles and track hbm_accesses/hbm_bytes
    }

    // Read a single float from SRAM (1 cycle)
    float sram_read(int offset) {
        // TODO: Return sram[offset] and account for SRAM_LATENCY
        return 0.0f;
    }

    // Write a single float to SRAM (1 cycle)
    void sram_write(int offset, float val) {
        // TODO: Write val to sram[offset] and account for SRAM_LATENCY
    }
};

// ============ YOUR IMPLEMENTATION ============

/*
 * vector_add_naive: Per-element HBM access pattern
 * Each element of A and B is loaded individually from HBM,
 * added, and the result stored individually to HBM.
 *
 * This is the worst case — no data reuse, no tiling.
 * Cost: 3 * N * HBM_LATENCY cycles (load A[i], load B[i], store C[i])
 */
void vector_add_naive(MemorySystem& mem,
                      float* hbm_A, float* hbm_B, float* hbm_C,
                      int N) {
    // TODO: For each element:
    //   1. Load A[i] from HBM (use load_tile with count=1)
    //   2. Load B[i] from HBM
    //   3. Add them in SRAM
    //   4. Store result to HBM
}

/*
 * vector_add_tiled: Tiled access pattern with SRAM buffering
 * Load tiles of A and B into SRAM, compute in SRAM, store tile of C to HBM.
 *
 * SRAM layout: [A_tile | B_tile | C_tile]
 * tile_size = SRAM_CAPACITY / 3 (to fit all three tiles)
 *
 * Cost: 3 * ceil(N/tile_size) * HBM_LATENCY + N * SRAM_LATENCY cycles
 */
void vector_add_tiled(MemorySystem& mem,
                      float* hbm_A, float* hbm_B, float* hbm_C,
                      int N) {
    // TODO:
    // 1. Compute tile_size = SRAM_CAPACITY / 3
    // 2. For each tile:
    //    a. Load tile of A into sram[0..tile_size)
    //    b. Load tile of B into sram[tile_size..2*tile_size)
    //    c. Compute C_tile[i] = A_tile[i] + B_tile[i] in SRAM
    //    d. Store C_tile from sram[2*tile_size..3*tile_size) to HBM
    // 3. Handle last tile if N % tile_size != 0
}

/*
 * compute_arithmetic_intensity: Calculate ops per byte transferred
 * For vector add: 1 FLOP per element, 12 bytes per element (load A, load B, store C)
 */
float compute_arithmetic_intensity(int N) {
    // TODO: Return FLOPs / bytes_transferred
    return 0.0f;
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q02: Memory Hierarchy Simulator ===\n\n");
    int passed = 0, total = 0;

    const int N = 2048;
    vector<float> A(N), B(N), C_naive(N, 0), C_tiled(N, 0), C_expected(N);

    for (int i = 0; i < N; i++) {
        A[i] = (float)(i + 1);
        B[i] = (float)(N - i);
        C_expected[i] = A[i] + B[i];
    }

    MemorySystem mem;

    // Test 1: Naive correctness
    {
        total++;
        mem.reset();
        vector_add_naive(mem, A.data(), B.data(), C_naive.data(), N);
        bool correct = true;
        for (int i = 0; i < N; i++) {
            if (fabs(C_naive[i] - C_expected[i]) > 1e-5) { correct = false; break; }
        }
        printf("%s [Naive correctness]\n", correct ? "PASS" : "FAIL");
        if (correct) passed++;
        printf("  Naive: %lld cycles, %lld HBM accesses\n", mem.total_cycles, mem.hbm_accesses);
    }

    // Test 2: Tiled correctness
    {
        total++;
        mem.reset();
        vector_add_tiled(mem, A.data(), B.data(), C_tiled.data(), N);
        bool correct = true;
        for (int i = 0; i < N; i++) {
            if (fabs(C_tiled[i] - C_expected[i]) > 1e-5) { correct = false; break; }
        }
        printf("%s [Tiled correctness]\n", correct ? "PASS" : "FAIL");
        if (correct) passed++;
        printf("  Tiled: %lld cycles, %lld HBM accesses\n", mem.total_cycles, mem.hbm_accesses);
    }

    // Test 3: Tiled should use fewer HBM accesses than naive
    {
        total++;
        MemorySystem mem_naive, mem_tiled;
        vector<float> cn(N, 0), ct(N, 0);
        vector_add_naive(mem_naive, A.data(), B.data(), cn.data(), N);
        vector_add_tiled(mem_tiled, A.data(), B.data(), ct.data(), N);
        bool fewer = mem_tiled.hbm_accesses < mem_naive.hbm_accesses;
        printf("%s [Tiled uses fewer HBM accesses: %lld vs %lld]\n",
               fewer ? "PASS" : "FAIL", mem_tiled.hbm_accesses, mem_naive.hbm_accesses);
        if (fewer) passed++;
    }

    // Test 4: Tiled should use fewer total cycles
    {
        total++;
        MemorySystem mem_naive, mem_tiled;
        vector<float> cn(N, 0), ct(N, 0);
        vector_add_naive(mem_naive, A.data(), B.data(), cn.data(), N);
        vector_add_tiled(mem_tiled, A.data(), B.data(), ct.data(), N);
        bool faster = mem_tiled.total_cycles < mem_naive.total_cycles;
        printf("%s [Tiled is faster: %lld vs %lld cycles]\n",
               faster ? "PASS" : "FAIL", mem_tiled.total_cycles, mem_naive.total_cycles);
        if (faster) passed++;
    }

    // Test 5: Arithmetic intensity
    {
        total++;
        float ai = compute_arithmetic_intensity(N);
        // vector add: 1 FLOP per element, 12 bytes (4 bytes * 3 arrays)
        float expected_ai = 1.0f / 12.0f;
        bool ok = fabs(ai - expected_ai) < 1e-6;
        printf("%s [Arithmetic intensity = %.4f, expected %.4f]\n",
               ok ? "PASS" : "FAIL", ai, expected_ai);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
