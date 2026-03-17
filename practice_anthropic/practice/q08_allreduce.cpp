/*
 * Q08 — All-Reduce Ring Algorithm [Medium-Hard]
 *
 * Topic: Kernel Optimization — Collective Communications
 *
 * Problem:
 *   Simulate a ring all-reduce across N nodes. Each node starts with a local
 *   vector of values, and after all-reduce, every node should have the
 *   element-wise sum of all vectors.
 *
 *   Ring all-reduce has two phases:
 *     Phase 1 — Reduce-Scatter: Each node sends a chunk to the next node in the ring.
 *       After N-1 steps, each node holds one fully-reduced chunk.
 *     Phase 2 — All-Gather: Each node broadcasts its fully-reduced chunk around the ring.
 *       After N-1 steps, every node has all fully-reduced chunks.
 *
 * Constraints:
 *   - N nodes arranged in a ring (node i sends to node (i+1) % N)
 *   - Vector length V must be divisible by N (each chunk = V/N elements)
 *   - Count total data transferred (should be 2*(N-1)*(V/N) per node)
 *
 * Key Concepts:
 *   - Ring all-reduce decomposition
 *   - Bandwidth-optimal collective communication
 *   - Used in distributed training (gradient averaging)
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>

using namespace std;

struct AllReduceStats {
    long long total_bytes_transferred;  // total across all nodes
    int num_steps;                      // total communication steps
};

// ============ YOUR IMPLEMENTATION ============

/*
 * ring_allreduce: Perform ring all-reduce across num_nodes nodes.
 *
 * Input: node_data[i] is node i's local vector (all same length V).
 * Output: After completion, every node_data[i] should contain the
 *         element-wise sum of all original vectors.
 *
 * Algorithm:
 *
 * Phase 1 — Reduce-Scatter (N-1 steps):
 *   Divide each vector into N chunks of size V/N.
 *   At step s:
 *     - Node i sends chunk (i - s) % N to node (i + 1) % N
 *     - Node i receives chunk (i - s - 1) % N from node (i - 1) % N
 *     - Node i adds received chunk to its own chunk (i - s - 1) % N
 *   After N-1 steps, node i has fully reduced chunk i.
 *
 * Phase 2 — All-Gather (N-1 steps):
 *   At step s:
 *     - Node i sends chunk (i - s + 1) % N to node (i + 1) % N
 *     - Node i receives chunk (i - s) % N from node (i - 1) % N
 *     - Node i copies received chunk (already fully reduced)
 *   After N-1 steps, all nodes have all fully reduced chunks.
 */
AllReduceStats ring_allreduce(vector<vector<float>>& node_data, int num_nodes) {
    AllReduceStats stats = {0, 0};

    // TODO: Implement ring all-reduce
    // int V = node_data[0].size();
    // int chunk_size = V / num_nodes;
    //
    // Phase 1: Reduce-Scatter
    //   for step = 0 to num_nodes - 2:
    //     for each node i:
    //       determine send_chunk and recv_chunk indices
    //       "send" data to (i+1) % num_nodes
    //       "receive" data from (i-1+num_nodes) % num_nodes
    //       add received data to local chunk
    //     update stats
    //
    // Phase 2: All-Gather
    //   for step = 0 to num_nodes - 2:
    //     for each node i:
    //       determine send_chunk and recv_chunk indices
    //       "send" fully reduced chunk to next node
    //       "receive" and store fully reduced chunk from prev node
    //     update stats

    return stats;
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q08: All-Reduce Ring Algorithm ===\n\n");
    int passed = 0, total = 0;

    // Test 1: 4 nodes, simple values
    {
        total++;
        int N = 4, V = 8;
        vector<vector<float>> data(N, vector<float>(V));
        vector<float> expected(V, 0);

        for (int i = 0; i < N; i++)
            for (int j = 0; j < V; j++) {
                data[i][j] = (float)(i * V + j + 1);
                expected[j] += data[i][j];
            }

        auto stats = ring_allreduce(data, N);

        bool all_correct = true;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < V; j++)
                if (fabs(data[i][j] - expected[j]) > 1e-3) {
                    all_correct = false;
                    break;
                }

        printf("%s [4 nodes, V=8: all nodes have global sum]\n",
               all_correct ? "PASS" : "FAIL");
        if (all_correct) passed++;
        if (!all_correct) {
            printf("  Expected[0..3]: %.0f %.0f %.0f %.0f\n",
                   expected[0], expected[1], expected[2], expected[3]);
            printf("  Node 0 got:     %.0f %.0f %.0f %.0f\n",
                   data[0][0], data[0][1], data[0][2], data[0][3]);
        }
    }

    // Test 2: All nodes should have identical results
    {
        total++;
        int N = 3, V = 6;
        vector<vector<float>> data(N, vector<float>(V));
        for (int i = 0; i < N; i++)
            for (int j = 0; j < V; j++)
                data[i][j] = (float)((i + 1) * (j + 1));

        ring_allreduce(data, N);

        bool identical = true;
        for (int i = 1; i < N; i++)
            for (int j = 0; j < V; j++)
                if (fabs(data[i][j] - data[0][j]) > 1e-3) { identical = false; break; }

        printf("%s [3 nodes: all results identical]\n", identical ? "PASS" : "FAIL");
        if (identical) passed++;
    }

    // Test 3: Bandwidth optimal — total transfer should be 2*(N-1)*V*sizeof(float)
    {
        total++;
        int N = 4, V = 16;
        vector<vector<float>> data(N, vector<float>(V, 1.0f));
        auto stats = ring_allreduce(data, N);

        // Each node sends (N-1) chunks of size V/N in each phase (2 phases)
        // Total per node = 2 * (N-1) * (V/N) * sizeof(float)
        // Total across all nodes = N * 2 * (N-1) * (V/N) * sizeof(float) = 2*(N-1)*V*sizeof(float)
        long long expected_bytes = 2LL * (N - 1) * V * sizeof(float);
        bool ok = (stats.total_bytes_transferred == expected_bytes);
        printf("%s [Bandwidth optimal: %lld bytes, expected %lld]\n",
               ok ? "PASS" : "FAIL", stats.total_bytes_transferred, expected_bytes);
        if (ok) passed++;
    }

    // Test 4: Total communication steps should be 2*(N-1)
    {
        total++;
        int N = 5, V = 10;
        vector<vector<float>> data(N, vector<float>(V, 1.0f));
        auto stats = ring_allreduce(data, N);
        int expected_steps = 2 * (N - 1);
        bool ok = (stats.num_steps == expected_steps);
        printf("%s [Steps: %d, expected %d]\n",
               ok ? "PASS" : "FAIL", stats.num_steps, expected_steps);
        if (ok) passed++;
    }

    // Test 5: 2 nodes (simplest ring)
    {
        total++;
        int N = 2, V = 4;
        vector<vector<float>> data = {{1, 2, 3, 4}, {10, 20, 30, 40}};
        vector<float> expected = {11, 22, 33, 44};

        ring_allreduce(data, N);

        bool ok = true;
        for (int i = 0; i < N; i++)
            for (int j = 0; j < V; j++)
                if (fabs(data[i][j] - expected[j]) > 1e-3) { ok = false; break; }

        printf("%s [2 nodes: [1,2,3,4] + [10,20,30,40] = [11,22,33,44]]\n",
               ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
