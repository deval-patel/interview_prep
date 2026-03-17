/*
 * Q05 — Operator Fusion (ReLU + Bias + MatMul) [Medium]
 *
 * Topic: Kernel Optimization — Operator Fusion
 *
 * Problem:
 *   Compare unfused vs fused execution of: Y = ReLU(MatMul(A, B) + bias)
 *
 *   Unfused pipeline (each step reads/writes HBM):
 *     1. T1 = MatMul(A, B)    — write T1 to HBM
 *     2. T2 = T1 + bias       — read T1, write T2 to HBM
 *     3. Y  = ReLU(T2)        — read T2, write Y to HBM
 *
 *   Fused pipeline (single kernel, intermediates stay in SRAM):
 *     1. For each tile of output:
 *        a. Compute tile of MatMul(A, B) in SRAM
 *        b. Add bias in SRAM
 *        c. Apply ReLU in SRAM
 *        d. Write final result to HBM
 *
 *   Track HBM reads/writes to show fusion saves memory bandwidth.
 *
 * Key Concepts:
 *   - Operator fusion eliminates intermediate HBM round-trips
 *   - In unfused: 2 extra HBM write + 2 extra HBM read for intermediates
 *   - Fusion is critical for TPU/GPU kernel performance
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

struct FusionStats {
    long long hbm_reads;
    long long hbm_writes;
    long long total_hbm_bytes() { return (hbm_reads + hbm_writes) * sizeof(float); }
};

// ============ YOUR IMPLEMENTATION ============

/*
 * unfused_matmul_bias_relu:
 *   Step 1: T1 = A * B           (read A, B from HBM; write T1 to HBM)
 *   Step 2: T2 = T1 + bias       (read T1, bias from HBM; write T2 to HBM)
 *   Step 3: Y  = ReLU(T2)        (read T2 from HBM; write Y to HBM)
 *
 * Count every element read/written from/to HBM.
 */
void unfused_matmul_bias_relu(const vector<vector<float>>& A,     // M x K
                               const vector<vector<float>>& B,     // K x N
                               const vector<float>& bias,          // N
                               vector<vector<float>>& Y,           // M x N
                               int M, int K, int N,
                               FusionStats& stats) {
    // TODO: Implement 3-step unfused pipeline
    // Track all HBM reads and writes
}

/*
 * fused_matmul_bias_relu:
 *   For each output element Y[i][j]:
 *     1. Compute dot product A[i][:] * B[:][j] (reads from HBM)
 *     2. Add bias[j] (read from HBM once, or cache)
 *     3. Apply ReLU
 *     4. Write Y[i][j] to HBM
 *
 *   Key: No intermediate matrices written to HBM!
 */
void fused_matmul_bias_relu(const vector<vector<float>>& A,
                             const vector<vector<float>>& B,
                             const vector<float>& bias,
                             vector<vector<float>>& Y,
                             int M, int K, int N,
                             FusionStats& stats) {
    // TODO: Implement fused single-pass computation
    // Only HBM reads: A, B, bias
    // Only HBM writes: Y (final output)
    // No intermediate buffers written to HBM
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q05: Operator Fusion ===\n\n");
    int passed = 0, total = 0;

    const int M = 32, K = 32, N = 32;
    vector<vector<float>> A(M, vector<float>(K));
    vector<vector<float>> B(K, vector<float>(N));
    vector<float> bias(N);

    srand(42);
    for (int i = 0; i < M; i++)
        for (int j = 0; j < K; j++)
            A[i][j] = ((float)(rand() % 200) - 100) / 100.0f;
    for (int i = 0; i < K; i++)
        for (int j = 0; j < N; j++)
            B[i][j] = ((float)(rand() % 200) - 100) / 100.0f;
    for (int j = 0; j < N; j++)
        bias[j] = ((float)(rand() % 100) - 50) / 100.0f;

    vector<vector<float>> Y_unfused(M, vector<float>(N, 0));
    vector<vector<float>> Y_fused(M, vector<float>(N, 0));
    FusionStats stats_unfused = {}, stats_fused = {};

    unfused_matmul_bias_relu(A, B, bias, Y_unfused, M, K, N, stats_unfused);
    fused_matmul_bias_relu(A, B, bias, Y_fused, M, K, N, stats_fused);

    // Test 1: Both produce same result
    {
        total++;
        bool same = true;
        for (int i = 0; i < M && same; i++)
            for (int j = 0; j < N && same; j++)
                if (fabs(Y_unfused[i][j] - Y_fused[i][j]) > 1e-3) same = false;
        printf("%s [Unfused and fused produce same result]\n", same ? "PASS" : "FAIL");
        if (same) passed++;
    }

    // Test 2: All outputs are >= 0 (ReLU applied)
    {
        total++;
        bool ok = true;
        for (int i = 0; i < M && ok; i++)
            for (int j = 0; j < N && ok; j++)
                if (Y_fused[i][j] < -1e-7) ok = false;
        printf("%s [All outputs >= 0 (ReLU)]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 3: Fused has fewer total HBM bytes
    {
        total++;
        bool fewer = stats_fused.total_hbm_bytes() < stats_unfused.total_hbm_bytes();
        printf("%s [Fused uses less HBM bandwidth]\n", fewer ? "PASS" : "FAIL");
        if (fewer) passed++;
        printf("  Unfused: %lld reads + %lld writes = %lld bytes\n",
               stats_unfused.hbm_reads, stats_unfused.hbm_writes, stats_unfused.total_hbm_bytes());
        printf("  Fused:   %lld reads + %lld writes = %lld bytes\n",
               stats_fused.hbm_reads, stats_fused.hbm_writes, stats_fused.total_hbm_bytes());
    }

    // Test 4: Fused has fewer HBM writes specifically
    {
        total++;
        bool fewer = stats_fused.hbm_writes < stats_unfused.hbm_writes;
        printf("%s [Fused has fewer HBM writes: %lld vs %lld]\n",
               fewer ? "PASS" : "FAIL", stats_fused.hbm_writes, stats_unfused.hbm_writes);
        if (fewer) passed++;
    }

    // Test 5: Simple known result
    {
        total++;
        vector<vector<float>> a = {{1, 2}, {3, 4}};
        vector<vector<float>> b = {{1, 0}, {0, 1}};
        vector<float> bi = {-5, 0};
        vector<vector<float>> y(2, vector<float>(2, 0));
        FusionStats s = {};
        fused_matmul_bias_relu(a, b, bi, y, 2, 2, 2, s);
        // MatMul: [[1,2],[3,4]], +bias[-5,0]: [[-4,2],[-2,4]], ReLU: [[0,2],[0,4]]
        bool ok = fabs(y[0][0]) < 1e-5 && fabs(y[0][1] - 2.0f) < 1e-5 &&
                  fabs(y[1][0]) < 1e-5 && fabs(y[1][1] - 4.0f) < 1e-5;
        printf("%s [Simple 2x2 known result]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
