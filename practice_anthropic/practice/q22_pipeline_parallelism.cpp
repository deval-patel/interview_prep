/*
 * Q22 — Pipeline Parallelism Simulator [Medium-Hard]
 *
 * Topic: Parallel Programming — Pipeline Parallelism
 *
 * Problem:
 *   Simulate pipeline parallelism for model training across multiple stages
 *   (GPipe-style). In pipeline parallelism, a model is split into stages
 *   across devices. Microbatches flow through the pipeline, overlapping
 *   computation across stages.
 *
 *   Implement:
 *   1. PipelineScheduler(num_stages, num_microbatches) — setup pipeline
 *   2. Each stage processes one microbatch at a time, takes stage_latencies[i] cycles
 *   3. Naive (sequential) vs pipelined execution
 *   4. Compute: bubble ratio, throughput, speedup vs naive
 *
 *   In pipelined execution:
 *   - Stage i can start microbatch m+1 only after stage i finishes microbatch m
 *   - Stage i+1 can start microbatch m only after stage i finishes microbatch m
 *   - The "bubble" is idle time where stages sit empty at pipeline start/end
 *
 * Constraints:
 *   - num_stages >= 1, num_microbatches >= 1
 *   - stage_latencies[i] > 0 for all i
 *   - Track per-stage busy and idle time
 *
 * Key Concepts:
 *   - Pipeline parallelism (GPipe, PipeDream)
 *   - Bubble overhead = (num_stages - 1) / (num_stages - 1 + num_microbatches)
 *   - Throughput approaches 1/max(stage_latency) as microbatches -> infinity
 *   - Speedup over naive execution
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

struct PipelineStats {
    int total_time;                  // total cycles for all microbatches
    double bubble_ratio;             // fraction of total slot-time that is idle
    double throughput;               // microbatches per cycle
    vector<int> stage_busy_time;     // per-stage busy cycles
    vector<int> stage_idle_time;     // per-stage idle cycles
};

// ============ YOUR IMPLEMENTATION ============

/*
 * naive_execution: No pipelining. Each microbatch runs through all stages
 * sequentially before the next microbatch begins.
 *
 * Total time = num_microbatches * sum(stage_latencies)
 */
int naive_execution(const vector<int>& stage_latencies, int num_microbatches) {
    // TODO: Compute total execution time without any pipelining
    return 0;
}

/*
 * PipelineScheduler: Simulates GPipe-style pipeline parallelism.
 *
 * For pipelined execution, track when each (stage, microbatch) pair starts
 * and finishes. The start time for (stage s, microbatch m) is:
 *   start[s][m] = max(finish[s][m-1], finish[s-1][m])
 *   finish[s][m] = start[s][m] + stage_latencies[s]
 *
 * Bubble ratio = total_idle_time / (num_stages * total_time)
 * where total_idle_time = num_stages * total_time - sum(all busy times)
 */
struct PipelineScheduler {
    int num_stages;
    int num_microbatches;
    vector<int> stage_latencies;

    // start_time[s][m] = cycle when stage s begins processing microbatch m
    // finish_time[s][m] = cycle when stage s finishes processing microbatch m
    vector<vector<int>> start_time;
    vector<vector<int>> finish_time;

    PipelineScheduler(int num_stages, int num_microbatches,
                      const vector<int>& stage_latencies)
        : num_stages(num_stages), num_microbatches(num_microbatches),
          stage_latencies(stage_latencies),
          start_time(num_stages, vector<int>(num_microbatches, 0)),
          finish_time(num_stages, vector<int>(num_microbatches, 0)) {}

    /*
     * simulate: Fill in start_time and finish_time for all (stage, microbatch)
     * pairs using the pipeline scheduling constraints.
     *
     * Constraints:
     *   start_time[0][0] = 0
     *   start_time[s][0] = finish_time[s-1][0]  (first microbatch flows through)
     *   start_time[0][m] = finish_time[0][m-1]  (stage 0 processes sequentially)
     *   start_time[s][m] = max(finish_time[s][m-1], finish_time[s-1][m])
     *   finish_time[s][m] = start_time[s][m] + stage_latencies[s]
     */
    void simulate() {
        // TODO: Fill start_time and finish_time arrays
    }

    /*
     * get_stats: Compute pipeline statistics after simulation.
     *
     * total_time = finish_time[num_stages-1][num_microbatches-1]
     * stage_busy_time[s] = num_microbatches * stage_latencies[s]
     * stage_idle_time[s] = total_time - stage_busy_time[s]
     * bubble_ratio = sum(idle_times) / (num_stages * total_time)
     * throughput = num_microbatches / total_time
     */
    PipelineStats get_stats() {
        PipelineStats stats;
        stats.total_time = 0;
        stats.bubble_ratio = 0.0;
        stats.throughput = 0.0;
        stats.stage_busy_time.resize(num_stages, 0);
        stats.stage_idle_time.resize(num_stages, 0);
        // TODO: Compute all stats from start_time/finish_time
        return stats;
    }
};

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q22: Pipeline Parallelism Simulator ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Naive total time = num_microbatches * sum(stage_latencies)
    {
        total++;
        vector<int> latencies = {10, 20, 15};
        int naive = naive_execution(latencies, 4);
        bool ok = (naive == 4 * (10 + 20 + 15));  // 4 * 45 = 180
        printf("%s [Naive total time = mb * sum(latencies)]\n", ok ? "PASS" : "FAIL");
        if (!ok) printf("  Expected %d, got %d\n", 180, naive);
        if (ok) passed++;
    }

    // Test 2: Pipelined is faster than naive for >1 microbatch
    {
        total++;
        vector<int> latencies = {10, 10, 10};
        int naive = naive_execution(latencies, 4);
        PipelineScheduler ps(3, 4, latencies);
        ps.simulate();
        PipelineStats stats = ps.get_stats();
        bool ok = (stats.total_time < naive) && (stats.total_time > 0);
        printf("%s [Pipelined faster than naive (pipe=%d < naive=%d)]\n",
               ok ? "PASS" : "FAIL", stats.total_time, naive);
        if (ok) passed++;
    }

    // Test 3: Bubble ratio decreases with more microbatches
    {
        total++;
        vector<int> latencies = {10, 10, 10};
        PipelineScheduler ps4(3, 4, latencies);
        ps4.simulate();
        PipelineStats stats4 = ps4.get_stats();

        PipelineScheduler ps16(3, 16, latencies);
        ps16.simulate();
        PipelineStats stats16 = ps16.get_stats();

        bool ok = (stats16.bubble_ratio < stats4.bubble_ratio) &&
                  (stats4.bubble_ratio > 0.0);
        printf("%s [Bubble ratio decreases: 4mb=%.3f > 16mb=%.3f]\n",
               ok ? "PASS" : "FAIL", stats4.bubble_ratio, stats16.bubble_ratio);
        if (ok) passed++;
    }

    // Test 4: Equal stage latencies -> optimal pipeline (known formula)
    // With equal latencies L, total_time = (S + M - 1) * L
    {
        total++;
        vector<int> latencies = {10, 10, 10};
        PipelineScheduler ps(3, 5, latencies);
        ps.simulate();
        PipelineStats stats = ps.get_stats();
        int expected = (3 + 5 - 1) * 10;  // 70
        bool ok = (stats.total_time == expected);
        printf("%s [Equal latencies: total=%d, expected=%d]\n",
               ok ? "PASS" : "FAIL", stats.total_time, expected);
        if (ok) passed++;
    }

    // Test 5: Unequal stages -> bottleneck is slowest stage
    // total_time = sum(latencies) + (M-1) * max(latencies)
    {
        total++;
        vector<int> latencies = {5, 20, 10};
        PipelineScheduler ps(3, 6, latencies);
        ps.simulate();
        PipelineStats stats = ps.get_stats();
        int expected = (5 + 20 + 10) + (6 - 1) * 20;  // 35 + 100 = 135
        bool ok = (stats.total_time == expected);
        printf("%s [Unequal stages bottleneck: total=%d, expected=%d]\n",
               ok ? "PASS" : "FAIL", stats.total_time, expected);
        if (ok) passed++;
    }

    // Test 6: Throughput approaches 1/max(stage_latency) with many microbatches
    {
        total++;
        vector<int> latencies = {5, 20, 10};
        PipelineScheduler ps(3, 100, latencies);
        ps.simulate();
        PipelineStats stats = ps.get_stats();
        double ideal_throughput = 1.0 / 20.0;  // 1/max(latency)
        bool ok = fabs(stats.throughput - ideal_throughput) < 0.005;
        printf("%s [Throughput -> 1/max(lat): actual=%.4f, ideal=%.4f]\n",
               ok ? "PASS" : "FAIL", stats.throughput, ideal_throughput);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
