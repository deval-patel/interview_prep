/*
 * Q09 — Reduce-Scatter with Recursive Halving [Medium]
 *
 * Topic: Kernel Optimization — Collective Communications
 *
 * Problem:
 *   Implement reduce-scatter using the recursive halving algorithm.
 *   Unlike ring reduce-scatter (linear steps), recursive halving uses
 *   a tree topology to complete in O(log N) steps.
 *
 *   After reduce-scatter, node i holds the i-th chunk of the globally
 *   reduced (summed) vector.
 *
 *   Recursive Halving Algorithm:
 *     At each step, nodes pair up:
 *     - Step 0: nodes (0,N/2), (1,N/2+1), ... exchange data
 *     - Each pair: one node gets the reduced lower half, other gets reduced upper half
 *     - Recurse on each half independently
 *     - After log2(N) steps, each node has one fully-reduced chunk
 *
 * Constraints:
 *   - N must be a power of 2
 *   - Vector length V must be divisible by N
 *   - Compare steps and bytes transferred vs ring algorithm
 *
 * Key Concepts:
 *   - Tree-based collective algorithms
 *   - Recursive halving / doubling
 *   - Latency vs bandwidth tradeoffs in collectives
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>

using namespace std;

struct ReduceScatterStats {
    long long total_bytes_transferred;
    int num_steps;  // communication rounds
};

// ============ YOUR IMPLEMENTATION ============

/*
 * recursive_halving_reduce_scatter:
 *   Perform reduce-scatter using recursive halving.
 *
 *   Input: node_data[i] is node i's local vector (length V).
 *   Output: node_data[i] contains just chunk i of the global sum
 *           (i.e., elements [i*chunk_size .. (i+1)*chunk_size) of sum(all vectors))
 *           Only the first chunk_size elements of node_data[i] are valid.
 *
 *   Algorithm (for N=8, V=8):
 *   Step 1 (stride=4): Pairs (0,4),(1,5),(2,6),(3,7)
 *     - Node 0 sends upper half to node 4, receives node 4's upper half
 *     - Node 0 reduces lower half, node 4 reduces upper half
 *   Step 2 (stride=2): Pairs (0,2),(1,3) and (4,6),(5,7)
 *     - Continue halving within each group
 *   Step 3 (stride=1): Pairs (0,1),(2,3),(4,5),(6,7)
 *     - Final halving, each node has 1 chunk
 */
ReduceScatterStats recursive_halving_reduce_scatter(
    vector<vector<float>>& node_data, int num_nodes) {
    ReduceScatterStats stats = {0, 0};

    // TODO: Implement recursive halving reduce-scatter
    // int V = node_data[0].size();
    // int chunk_size = V / num_nodes;
    //
    // For log2(num_nodes) steps:
    //   stride = num_nodes / 2, then /= 2
    //   For each pair of nodes separated by stride:
    //     Exchange appropriate half of active data
    //     Reduce (sum) the received half with local data
    //   Track bytes transferred and steps

    return stats;
}

/*
 * ring_reduce_scatter: For comparison — linear reduce-scatter from ring all-reduce.
 * N-1 steps, each node sends V/N elements per step.
 */
ReduceScatterStats ring_reduce_scatter(
    vector<vector<float>>& node_data, int num_nodes) {
    ReduceScatterStats stats = {0, 0};

    // TODO: Implement ring reduce-scatter (Phase 1 of ring all-reduce)
    // This is the same as Phase 1 from Q08

    return stats;
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q09: Reduce-Scatter (Recursive Halving) ===\n\n");
    int passed = 0, total = 0;

    // Test 1: 4 nodes, recursive halving correctness
    {
        total++;
        int N = 4, V = 8;
        vector<vector<float>> data(N, vector<float>(V));
        vector<float> global_sum(V, 0);

        for (int i = 0; i < N; i++)
            for (int j = 0; j < V; j++) {
                data[i][j] = (float)(i * 10 + j + 1);
                global_sum[j] += data[i][j];
            }

        auto stats = recursive_halving_reduce_scatter(data, N);

        int chunk_size = V / N;
        bool ok = true;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < chunk_size; j++) {
                float expected = global_sum[i * chunk_size + j];
                if (fabs(data[i][j] - expected) > 1e-3) {
                    ok = false;
                    printf("  Node %d, elem %d: got %.1f, expected %.1f\n",
                           i, j, data[i][j], expected);
                }
            }
        }
        printf("%s [4 nodes recursive halving correctness]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 2: Recursive halving uses log2(N) steps
    {
        total++;
        int N = 8, V = 16;
        vector<vector<float>> data(N, vector<float>(V, 1.0f));
        auto stats = recursive_halving_reduce_scatter(data, N);

        int expected_steps = 3;  // log2(8) = 3
        bool ok = (stats.num_steps == expected_steps);
        printf("%s [8 nodes: %d steps, expected %d (log2)]\n",
               ok ? "PASS" : "FAIL", stats.num_steps, expected_steps);
        if (ok) passed++;
    }

    // Test 3: Ring reduce-scatter uses N-1 steps
    {
        total++;
        int N = 4, V = 8;
        vector<vector<float>> data(N, vector<float>(V, 1.0f));
        auto stats = ring_reduce_scatter(data, N);

        int expected_steps = N - 1;
        bool ok = (stats.num_steps == expected_steps);
        printf("%s [Ring: %d steps, expected %d (N-1)]\n",
               ok ? "PASS" : "FAIL", stats.num_steps, expected_steps);
        if (ok) passed++;
    }

    // Test 4: Recursive halving fewer steps than ring for large N
    {
        total++;
        int N = 8, V = 16;
        vector<vector<float>> data_rh(N, vector<float>(V, 1.0f));
        vector<vector<float>> data_ring(N, vector<float>(V, 1.0f));

        auto stats_rh = recursive_halving_reduce_scatter(data_rh, N);
        auto stats_ring = ring_reduce_scatter(data_ring, N);

        bool ok = (stats_rh.num_steps < stats_ring.num_steps);
        printf("%s [Recursive halving (%d steps) < Ring (%d steps)]\n",
               ok ? "PASS" : "FAIL", stats_rh.num_steps, stats_ring.num_steps);
        if (ok) passed++;
    }

    // Test 5: 2 nodes (simplest case)
    {
        total++;
        int N = 2, V = 4;
        vector<vector<float>> data = {{1, 2, 3, 4}, {10, 20, 30, 40}};
        auto stats = recursive_halving_reduce_scatter(data, N);

        // Node 0 should have global_sum[0..1] = {11, 22}
        // Node 1 should have global_sum[2..3] = {33, 44}
        bool ok = fabs(data[0][0] - 11.0f) < 1e-3 &&
                  fabs(data[0][1] - 22.0f) < 1e-3 &&
                  fabs(data[1][0] - 33.0f) < 1e-3 &&
                  fabs(data[1][1] - 44.0f) < 1e-3;
        printf("%s [2 nodes: node0=[11,22], node1=[33,44]]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
