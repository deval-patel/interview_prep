/*
 * Q04 — Tiled Matrix Multiplication [Medium]
 *
 * Topic: Kernel Optimization — Tiling
 *
 * Problem:
 *   Implement tiled (blocked) matrix multiplication to improve cache/SRAM utilization.
 *   Compare against naive triple-loop matmul.
 *
 *   In a naive matmul, each element of B is loaded from HBM many times.
 *   By tiling, we load sub-blocks into fast SRAM and reuse them,
 *   dramatically reducing HBM traffic.
 *
 * Constraints:
 *   - Matrix dimensions may not be divisible by tile size
 *   - Track HBM load count for both versions
 *   - SRAM can hold 3 tiles: A_tile, B_tile, C_tile
 *   - Default tile size: 16x16
 *
 * Approach:
 *   Naive: C[i][j] += A[i][k] * B[k][j] — loads B[k][j] M times
 *   Tiled: For each tile of C, load corresponding tiles of A and B into SRAM,
 *          compute partial products, accumulate
 *
 * Complexity: Both O(M*K*N) FLOPs, but tiled has O(M*K*N / TILE) HBM loads
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>

using namespace std;

const int TILE_SIZE = 16;

struct MatmulStats {
    long long hbm_loads;    // number of individual float loads from HBM
    long long hbm_stores;   // number of individual float stores to HBM
    long long flops;        // number of multiply-add operations (each counts as 2)
};

// ============ YOUR IMPLEMENTATION ============

/*
 * naive_matmul: Standard triple-loop matrix multiply with load tracking.
 * C[M][N] = A[M][K] * B[K][N]
 * Every access to A and B counts as an HBM load.
 * Every write to C counts as an HBM store.
 */
void naive_matmul(const vector<vector<float>>& A,
                  const vector<vector<float>>& B,
                  vector<vector<float>>& C,
                  int M, int K, int N,
                  MatmulStats& stats) {
    // TODO: Implement triple-loop matmul
    // Count each A[i][k] read, B[k][j] read as hbm_loads
    // Count each C[i][j] write as hbm_stores
    // Count each multiply-add as 2 flops
}

/*
 * tiled_matmul: Blocked matrix multiply with SRAM tile buffers.
 *
 * For each tile of output C (of size TILE_SIZE x TILE_SIZE):
 *   For each tile along the K dimension:
 *     1. Load A_tile[TILE_SIZE][TILE_SIZE] from A into SRAM (count as 1 HBM load per element)
 *     2. Load B_tile[TILE_SIZE][TILE_SIZE] from B into SRAM
 *     3. Compute C_tile += A_tile * B_tile using SRAM (no HBM access)
 *   Store C_tile back to C (count as HBM stores)
 *
 * Handle edge tiles where dimensions don't divide evenly by TILE_SIZE.
 */
void tiled_matmul(const vector<vector<float>>& A,
                  const vector<vector<float>>& B,
                  vector<vector<float>>& C,
                  int M, int K, int N,
                  MatmulStats& stats) {
    // TODO: Implement tiled matmul
    // Use local arrays for A_tile, B_tile, C_tile (simulating SRAM)
    // Track HBM loads for tile loading, HBM stores for tile storing
}

// ============ TEST FRAMEWORK ============

bool matrices_equal(const vector<vector<float>>& A,
                    const vector<vector<float>>& B,
                    int M, int N, float eps = 1e-3) {
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (fabs(A[i][j] - B[i][j]) > eps) return false;
    return true;
}

int main() {
    printf("=== Q04: Tiled Matrix Multiplication ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Square matrices (divisible by tile size)
    {
        total++;
        const int N = 64;
        vector<vector<float>> A(N, vector<float>(N));
        vector<vector<float>> B(N, vector<float>(N));
        vector<vector<float>> C_naive(N, vector<float>(N, 0));
        vector<vector<float>> C_tiled(N, vector<float>(N, 0));

        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++) {
                A[i][j] = (float)(rand() % 10) / 10.0f;
                B[i][j] = (float)(rand() % 10) / 10.0f;
            }

        MatmulStats stats_naive = {}, stats_tiled = {};
        naive_matmul(A, B, C_naive, N, N, N, stats_naive);
        tiled_matmul(A, B, C_tiled, N, N, N, stats_tiled);

        bool correct = matrices_equal(C_naive, C_tiled, N, N);
        printf("%s [64x64 correctness]\n", correct ? "PASS" : "FAIL");
        if (correct) passed++;
        printf("  Naive loads: %lld, Tiled loads: %lld\n", stats_naive.hbm_loads, stats_tiled.hbm_loads);
    }

    // Test 2: Non-square, non-divisible by tile size
    {
        total++;
        int M = 37, K = 43, N = 29;
        vector<vector<float>> A(M, vector<float>(K));
        vector<vector<float>> B(K, vector<float>(N));
        vector<vector<float>> C_naive(M, vector<float>(N, 0));
        vector<vector<float>> C_tiled(M, vector<float>(N, 0));

        for (int i = 0; i < M; i++)
            for (int j = 0; j < K; j++) A[i][j] = (float)(rand() % 10);
        for (int i = 0; i < K; i++)
            for (int j = 0; j < N; j++) B[i][j] = (float)(rand() % 10);

        MatmulStats s1 = {}, s2 = {};
        naive_matmul(A, B, C_naive, M, K, N, s1);
        tiled_matmul(A, B, C_tiled, M, K, N, s2);

        bool correct = matrices_equal(C_naive, C_tiled, M, N);
        printf("%s [37x43 * 43x29 correctness]\n", correct ? "PASS" : "FAIL");
        if (correct) passed++;
    }

    // Test 3: Tiled should have fewer HBM loads than naive for large matrices
    {
        total++;
        const int N = 64;
        vector<vector<float>> A(N, vector<float>(N, 1.0f));
        vector<vector<float>> B(N, vector<float>(N, 1.0f));
        vector<vector<float>> C1(N, vector<float>(N, 0));
        vector<vector<float>> C2(N, vector<float>(N, 0));

        MatmulStats s1 = {}, s2 = {};
        naive_matmul(A, B, C1, N, N, N, s1);
        tiled_matmul(A, B, C2, N, N, N, s2);

        bool fewer = s2.hbm_loads < s1.hbm_loads;
        printf("%s [Tiled has fewer HBM loads]\n", fewer ? "PASS" : "FAIL");
        if (fewer) passed++;
        printf("  Reduction: %.1fx fewer loads\n",
               (float)s1.hbm_loads / (float)(s2.hbm_loads > 0 ? s2.hbm_loads : 1));
    }

    // Test 4: Small matrix (smaller than tile size)
    {
        total++;
        int M = 4, K = 3, N = 5;
        vector<vector<float>> A = {{1,2,3},{4,5,6},{7,8,9},{10,11,12}};
        vector<vector<float>> B = {{1,2,3,4,5},{6,7,8,9,10},{11,12,13,14,15}};
        vector<vector<float>> C_naive(M, vector<float>(N, 0));
        vector<vector<float>> C_tiled(M, vector<float>(N, 0));

        MatmulStats s1 = {}, s2 = {};
        naive_matmul(A, B, C_naive, M, K, N, s1);
        tiled_matmul(A, B, C_tiled, M, K, N, s2);

        // C[0][0] = 1*1 + 2*6 + 3*11 = 1+12+33 = 46
        bool correct = matrices_equal(C_naive, C_tiled, M, N) && fabs(C_naive[0][0] - 46.0f) < 1e-3;
        printf("%s [Small matrix (4x3 * 3x5)]\n", correct ? "PASS" : "FAIL");
        if (correct) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
