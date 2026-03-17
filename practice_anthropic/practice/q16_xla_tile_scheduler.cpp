/*
 * Q16 — XLA-Style Tile Scheduler [Medium-Hard]
 *
 * Topic: ML Context — Compiler Infrastructure
 *
 * Problem:
 *   Implement a tile operation scheduler that takes a DAG of tile operations
 *   and schedules them respecting:
 *   1. Data dependencies (operation B depends on output of operation A)
 *   2. SRAM capacity constraints (sum of live tile buffers <= SRAM capacity)
 *
 *   This simulates what an XLA compiler backend does when lowering HLO
 *   operations to hardware tile operations.
 *
 *   Implement:
 *   1. DAG construction with operations and dependencies
 *   2. Topological sort
 *   3. Greedy scheduler with resource constraints
 *
 * Constraints:
 *   - Each operation produces one output tile of known size
 *   - A tile is "live" from when it's produced until all consumers finish
 *   - SRAM capacity is limited; operations can't be scheduled if their
 *     output wouldn't fit in available SRAM
 *
 * Key Concepts:
 *   - DAG scheduling
 *   - Resource-constrained scheduling
 *   - Topological ordering
 *   - Liveness analysis
 */

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cstring>

using namespace std;

struct TileOp {
    int id;
    const char* name;
    int output_size;        // size of output tile in SRAM (in "units")
    int compute_cycles;     // how long this op takes
    vector<int> deps;       // IDs of operations this depends on
    vector<int> consumers;  // IDs of operations that use this output
};

struct ScheduleEntry {
    int op_id;
    int start_cycle;
    int end_cycle;
};

// ============ YOUR IMPLEMENTATION ============

struct TileScheduler {
    vector<TileOp> ops;
    int sram_capacity;

    TileScheduler(int sram_cap) : sram_capacity(sram_cap) {}

    /*
     * add_op: Add a tile operation to the DAG.
     * Returns the operation ID.
     */
    int add_op(const char* name, int output_size, int compute_cycles,
               vector<int> deps = {}) {
        // TODO: Create TileOp, add to ops vector
        // Also update consumers list for dependency ops
        return -1;
    }

    /*
     * topological_sort: Return a valid topological ordering of operations.
     * Uses Kahn's algorithm (BFS with in-degree tracking).
     */
    vector<int> topological_sort() {
        // TODO: Implement Kahn's algorithm
        // 1. Compute in-degree for each node
        // 2. Start with nodes having in-degree 0
        // 3. Process queue: for each node, decrement in-degree of consumers
        // 4. Return order
        return {};
    }

    /*
     * schedule_greedy: Schedule operations respecting dependencies and SRAM limits.
     *
     * Algorithm:
     *   1. Get topological order
     *   2. For each operation in topological order:
     *      a. Wait until all dependencies are completed
     *      b. Check if output tile fits in available SRAM
     *      c. If not, wait until some tiles are freed (consumers complete)
     *      d. Schedule the operation
     *   3. Free tiles when all their consumers have completed
     *
     * Returns: ordered list of ScheduleEntry with timing.
     */
    vector<ScheduleEntry> schedule_greedy() {
        // TODO: Implement greedy scheduler
        return {};
    }

    /*
     * compute_total_cycles: Return total execution time of the schedule.
     */
    int compute_total_cycles(const vector<ScheduleEntry>& schedule) {
        // TODO: Return max end_cycle across all entries
        return 0;
    }

    /*
     * peak_sram_usage: Return peak SRAM usage during execution.
     */
    int peak_sram_usage(const vector<ScheduleEntry>& schedule) {
        // TODO: Simulate the schedule and track peak SRAM usage
        // A tile is live from when its op finishes until all consumers finish
        return 0;
    }
};

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q16: XLA-Style Tile Scheduler ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Linear chain (A -> B -> C)
    {
        total++;
        TileScheduler sched(100);
        int a = sched.add_op("A", 10, 5);        // output=10, cycles=5
        int b = sched.add_op("B", 20, 10, {a});   // depends on A
        int c = sched.add_op("C", 15, 8, {b});    // depends on B

        auto order = sched.topological_sort();
        bool ok = (order.size() == 3 && order[0] == a && order[1] == b && order[2] == c);
        printf("%s [Linear chain topological sort]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 2: Diamond DAG (A -> B, A -> C, B -> D, C -> D)
    {
        total++;
        TileScheduler sched(100);
        int a = sched.add_op("A", 10, 5);
        int b = sched.add_op("B", 10, 5, {a});
        int c = sched.add_op("C", 10, 5, {a});
        int d = sched.add_op("D", 10, 5, {b, c});

        auto order = sched.topological_sort();
        bool ok = (order.size() == 4);
        // A must come first, D must come last
        ok = ok && (order[0] == a) && (order[3] == d);
        printf("%s [Diamond DAG topological sort]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 3: Schedule respects dependencies
    {
        total++;
        TileScheduler sched(100);
        int a = sched.add_op("A", 10, 5);
        int b = sched.add_op("B", 10, 10, {a});

        auto schedule = sched.schedule_greedy();
        bool ok = (schedule.size() == 2);
        if (ok) {
            // Find entries for A and B
            int a_end = -1, b_start = -1;
            for (auto& e : schedule) {
                if (e.op_id == a) a_end = e.end_cycle;
                if (e.op_id == b) b_start = e.start_cycle;
            }
            ok = (a_end >= 0 && b_start >= a_end);
        }
        printf("%s [Schedule respects dependencies]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 4: SRAM constraint
    {
        total++;
        TileScheduler sched(25);  // very limited SRAM
        int a = sched.add_op("A", 15, 5);   // 15 units
        int b = sched.add_op("B", 15, 5);   // 15 units — can't both fit with A's output live

        auto schedule = sched.schedule_greedy();
        int peak = sched.peak_sram_usage(schedule);
        bool ok = (peak <= 25);
        printf("%s [SRAM constraint respected: peak=%d, capacity=25]\n",
               ok ? "PASS" : "FAIL", peak);
        if (ok) passed++;
    }

    // Test 5: Total cycles for linear chain
    {
        total++;
        TileScheduler sched(100);
        sched.add_op("A", 10, 5);
        int b = sched.add_op("B", 10, 10, {0});
        int c = sched.add_op("C", 10, 8, {b});

        auto schedule = sched.schedule_greedy();
        int total_cycles = sched.compute_total_cycles(schedule);
        // Minimum: 5 + 10 + 8 = 23 cycles (sequential)
        bool ok = (total_cycles >= 23);
        printf("%s [Total cycles = %d (min 23)]\n", ok ? "PASS" : "FAIL", total_cycles);
        if (ok) passed++;
    }

    // Test 6: Independent ops can potentially run (tracked as sequential in greedy)
    {
        total++;
        TileScheduler sched(100);
        int a = sched.add_op("A", 10, 5);
        int b = sched.add_op("B", 10, 5);  // independent of A

        auto schedule = sched.schedule_greedy();
        bool ok = (schedule.size() == 2);
        printf("%s [Two independent ops scheduled]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
