/*
 * Q14 — Scaled Dot-Product Attention [Hard]
 *
 * Topic: ML Context — Transformer Architecture
 *
 * Problem:
 *   Implement scaled dot-product attention used in transformers:
 *     Attention(Q, K, V) = softmax(Q * K^T / sqrt(d_k)) * V
 *
 *   Two implementations:
 *   1. Naive: Materializes the full NxN attention matrix (O(N^2) memory)
 *   2. Tiled: Processes attention in blocks to reduce peak memory (Flash Attention concept)
 *
 *   Also support causal masking (lower-triangular mask) for autoregressive models.
 *
 * Constraints:
 *   - Q, K, V are (seq_len x d_model) matrices
 *   - Track peak memory usage (bytes) for each implementation
 *   - Causal mask: position i can only attend to positions <= i
 *
 * Key Concepts:
 *   - Attention mechanism internals
 *   - O(N^2) memory bottleneck
 *   - Flash Attention tiling strategy
 *   - Causal masking for autoregressive generation
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cfloat>
#include <algorithm>

using namespace std;

struct AttentionStats {
    long long peak_memory_bytes;  // peak additional memory beyond Q, K, V, O
    long long flops;
};

// Helper: stable softmax on a row
void row_softmax(vector<float>& row, int len) {
    float max_val = -FLT_MAX;
    for (int i = 0; i < len; i++) max_val = max(max_val, row[i]);
    float sum = 0;
    for (int i = 0; i < len; i++) {
        row[i] = expf(row[i] - max_val);
        sum += row[i];
    }
    for (int i = 0; i < len; i++) row[i] /= sum;
}

// ============ YOUR IMPLEMENTATION ============

/*
 * naive_attention: Standard attention with full NxN matrix materialization.
 *
 * Steps:
 *   1. Compute S = Q * K^T / sqrt(d_k)  — (seq_len x seq_len) matrix
 *   2. Apply causal mask (if enabled): S[i][j] = -inf for j > i
 *   3. P = softmax(S, dim=-1)           — row-wise softmax
 *   4. O = P * V                        — (seq_len x d_model) output
 *
 * Peak memory: O(seq_len^2) for the S/P matrix.
 */
void naive_attention(const vector<vector<float>>& Q,   // [seq_len x d_model]
                     const vector<vector<float>>& K,
                     const vector<vector<float>>& V,
                     vector<vector<float>>& O,          // [seq_len x d_model]
                     int seq_len, int d_model,
                     bool causal,
                     AttentionStats& stats) {
    // TODO: Implement naive attention
    // Track peak memory as seq_len * seq_len * sizeof(float) for attention matrix
}

/*
 * tiled_attention: Block-wise attention to reduce peak memory.
 *
 * Instead of materializing the full NxN attention matrix:
 *   For each block of query rows (block_q):
 *     For each block of key/value rows (block_kv):
 *       1. Compute partial attention scores for this block pair
 *       2. Track running max and sum for online softmax correction
 *       3. Accumulate weighted values into output
 *
 * Peak memory: O(block_size^2) instead of O(N^2)
 *
 * This is a simplified version of the Flash Attention algorithm.
 */
void tiled_attention(const vector<vector<float>>& Q,
                     const vector<vector<float>>& K,
                     const vector<vector<float>>& V,
                     vector<vector<float>>& O,
                     int seq_len, int d_model,
                     bool causal,
                     int block_size,
                     AttentionStats& stats) {
    // TODO: Implement tiled attention (simplified Flash Attention)
    //
    // For each query block bq (rows bq*BS to (bq+1)*BS):
    //   Initialize: row_max[i] = -inf, row_sum[i] = 0, O[i] = 0
    //   For each KV block bkv (rows bkv*BS to (bkv+1)*BS):
    //     1. Compute S_block = Q_block * K_block^T / sqrt(d_k)
    //     2. Apply causal mask if needed
    //     3. Find new max, compute exp(), new sum
    //     4. Rescale previous O accumulation if max changed
    //     5. Accumulate: O += P_block * V_block
    //   Normalize: O[i] /= row_sum[i]
    //
    // Peak memory: block_size * block_size * sizeof(float)
}

// ============ TEST FRAMEWORK ============

bool matrices_close(const vector<vector<float>>& A,
                    const vector<vector<float>>& B,
                    int M, int N, float tol = 1e-3) {
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (fabs(A[i][j] - B[i][j]) > tol) return false;
    return true;
}

int main() {
    printf("=== Q14: Scaled Dot-Product Attention ===\n\n");
    int passed = 0, total = 0;

    srand(42);

    // Test 1: Naive attention basic (no causal mask)
    {
        total++;
        int S = 8, D = 4;
        vector<vector<float>> Q(S, vector<float>(D));
        vector<vector<float>> K(S, vector<float>(D));
        vector<vector<float>> V(S, vector<float>(D));
        vector<vector<float>> O(S, vector<float>(D, 0));

        for (int i = 0; i < S; i++)
            for (int j = 0; j < D; j++) {
                Q[i][j] = ((float)(rand() % 100) - 50) / 50.0f;
                K[i][j] = ((float)(rand() % 100) - 50) / 50.0f;
                V[i][j] = ((float)(rand() % 100) - 50) / 50.0f;
            }

        AttentionStats stats = {};
        naive_attention(Q, K, V, O, S, D, false, stats);

        // Check that output rows are weighted combinations of V rows
        // Sum of attention weights per row should be 1 (softmax property)
        bool ok = true;
        for (int i = 0; i < S; i++) {
            float norm = 0;
            for (int j = 0; j < D; j++) norm += O[i][j] * O[i][j];
            if (norm < 1e-10) ok = false;  // output should be non-zero
        }
        printf("%s [Naive attention, seq_len=%d, d=%d]\n", ok ? "PASS" : "FAIL", S, D);
        if (ok) passed++;
    }

    // Test 2: Tiled matches naive (no causal)
    {
        total++;
        int S = 16, D = 8;
        vector<vector<float>> Q(S, vector<float>(D));
        vector<vector<float>> K(S, vector<float>(D));
        vector<vector<float>> V(S, vector<float>(D));
        vector<vector<float>> O_naive(S, vector<float>(D, 0));
        vector<vector<float>> O_tiled(S, vector<float>(D, 0));

        for (int i = 0; i < S; i++)
            for (int j = 0; j < D; j++) {
                Q[i][j] = ((float)(rand() % 100) - 50) / 100.0f;
                K[i][j] = ((float)(rand() % 100) - 50) / 100.0f;
                V[i][j] = ((float)(rand() % 100) - 50) / 100.0f;
            }

        AttentionStats s1 = {}, s2 = {};
        naive_attention(Q, K, V, O_naive, S, D, false, s1);
        tiled_attention(Q, K, V, O_tiled, S, D, false, 4, s2);

        bool ok = matrices_close(O_naive, O_tiled, S, D, 1e-2);
        printf("%s [Tiled matches naive (no causal)]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 3: Causal masking
    {
        total++;
        int S = 8, D = 4;
        vector<vector<float>> Q(S, vector<float>(D, 1.0f));
        vector<vector<float>> K(S, vector<float>(D, 1.0f));
        vector<vector<float>> V(S, vector<float>(D));
        vector<vector<float>> O(S, vector<float>(D, 0));

        // Make each V row distinct
        for (int i = 0; i < S; i++)
            for (int j = 0; j < D; j++)
                V[i][j] = (float)(i + 1);

        AttentionStats stats = {};
        naive_attention(Q, K, V, O, S, D, true, stats);

        // With causal mask, row 0 can only attend to position 0
        // So O[0] should be close to V[0]
        bool ok = true;
        for (int j = 0; j < D; j++)
            if (fabs(O[0][j] - V[0][j]) > 1e-3) ok = false;

        printf("%s [Causal: row 0 attends only to position 0]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 4: Tiled matches naive with causal mask
    {
        total++;
        int S = 16, D = 8;
        vector<vector<float>> Q(S, vector<float>(D));
        vector<vector<float>> K(S, vector<float>(D));
        vector<vector<float>> V(S, vector<float>(D));
        vector<vector<float>> O_naive(S, vector<float>(D, 0));
        vector<vector<float>> O_tiled(S, vector<float>(D, 0));

        for (int i = 0; i < S; i++)
            for (int j = 0; j < D; j++) {
                Q[i][j] = ((float)(rand() % 100) - 50) / 100.0f;
                K[i][j] = ((float)(rand() % 100) - 50) / 100.0f;
                V[i][j] = ((float)(rand() % 100) - 50) / 100.0f;
            }

        AttentionStats s1 = {}, s2 = {};
        naive_attention(Q, K, V, O_naive, S, D, true, s1);
        tiled_attention(Q, K, V, O_tiled, S, D, true, 4, s2);

        bool ok = matrices_close(O_naive, O_tiled, S, D, 1e-2);
        printf("%s [Tiled matches naive (causal)]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 5: Tiled uses less peak memory
    {
        total++;
        int S = 32, D = 16;
        vector<vector<float>> Q(S, vector<float>(D, 0.1f));
        vector<vector<float>> K(S, vector<float>(D, 0.1f));
        vector<vector<float>> V(S, vector<float>(D, 0.1f));
        vector<vector<float>> O1(S, vector<float>(D, 0));
        vector<vector<float>> O2(S, vector<float>(D, 0));

        AttentionStats s1 = {}, s2 = {};
        naive_attention(Q, K, V, O1, S, D, false, s1);
        tiled_attention(Q, K, V, O2, S, D, false, 8, s2);

        bool ok = (s2.peak_memory_bytes < s1.peak_memory_bytes);
        printf("%s [Tiled peak memory %lld < naive %lld bytes]\n",
               ok ? "PASS" : "FAIL", s2.peak_memory_bytes, s1.peak_memory_bytes);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
