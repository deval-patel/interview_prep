/*
 * Q20 — Thread Pool with Work Stealing [Medium-Hard]
 *
 * Topic: Parallel Programming — Concurrency
 *
 * Problem:
 *   Simulate a thread pool with work stealing. No actual threads are needed —
 *   simulate the scheduling algorithm step by step.
 *
 *   Implement:
 *   1. ThreadPool(num_workers) — create pool with N worker queues
 *   2. submit(task_id, duration, dependencies) — add task with dependencies
 *   3. run() — simulate execution with work stealing, return schedule
 *
 *   Work stealing policy:
 *     When a worker's local queue is empty and it is idle, it steals a task
 *     from the worker with the longest queue. If multiple workers have the
 *     same longest queue length, steal from the lowest-indexed one.
 *
 *   Dependency handling:
 *     A task can only begin execution once ALL of its dependencies have
 *     completed. Tasks with unmet dependencies remain in a pending set.
 *
 *   Task assignment:
 *     Tasks without dependencies are distributed round-robin to workers
 *     upon submission (worker = task_index % num_workers).
 *
 *   Track:
 *     - Total makespan (time when the last task finishes)
 *     - Per-worker utilization (busy_time / makespan)
 *     - Number of steals that occurred
 *     - Schedule: list of (task_id, worker_id, start_time, end_time)
 *
 * Constraints:
 *   - Task IDs are unique non-negative integers
 *   - Duration is a positive integer (time units)
 *   - Dependencies reference previously submitted task IDs
 *   - A stolen task is removed from the victim's queue
 *
 * Key Concepts:
 *   - Work stealing for load balancing
 *   - Dependency-aware task scheduling
 *   - Critical path analysis
 *   - Worker utilization metrics
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>

using namespace std;

struct Task {
    int id;
    int duration;
    vector<int> dependencies;
};

struct ScheduleEntry {
    int task_id;
    int worker_id;
    int start_time;
    int end_time;
};

struct RunResult {
    int makespan;                       // time when last task finishes
    vector<double> utilization;         // per-worker utilization (busy / makespan)
    int num_steals;                     // total number of work steals
    vector<ScheduleEntry> schedule;     // execution log
};

// ============ YOUR IMPLEMENTATION ============

struct ThreadPool {
    int num_workers;

    // TODO: Add data structures for:
    //   - Per-worker task queues
    //   - Pending tasks (dependencies not yet met)
    //   - Completed task set
    //   - Per-worker current task and remaining time
    //   - Round-robin counter for initial assignment

    ThreadPool(int num_workers) : num_workers(num_workers) {
        // TODO: Initialize worker queues and tracking structures
    }

    /*
     * submit: Add a task to the pool.
     *   If the task has no unmet dependencies, assign it round-robin to a
     *   worker queue. Otherwise, add it to the pending set.
     */
    void submit(int task_id, int duration, vector<int> dependencies = {}) {
        // TODO: Create Task and either enqueue or defer based on dependencies
    }

    /*
     * run: Simulate execution step by step.
     *
     * Each time step:
     *   1. Advance running tasks. If a task completes this step:
     *      - Mark it as completed
     *      - Check pending tasks: any task whose deps are all met gets
     *        enqueued to its assigned worker (round-robin at submit time,
     *        or the worker that freed up — use round-robin for simplicity)
     *   2. For each idle worker (no current task):
     *      a. If its local queue is non-empty, dequeue and start it
     *      b. Else, try to steal: find the worker with the longest queue,
     *         steal the FRONT task from that queue. Increment steal count.
     *   3. If all tasks completed and no tasks running, stop.
     *
     * Return the RunResult with makespan, utilization, steals, and schedule.
     */
    RunResult run() {
        RunResult result;
        result.makespan = 0;
        result.num_steals = 0;
        result.utilization.resize(num_workers, 0.0);

        // TODO: Implement simulation loop
        //
        // int current_time = 0;
        // vector<int> worker_busy_time(num_workers, 0);
        //
        // while (tasks_remain) {
        //     advance running tasks, check completions
        //     release newly-ready tasks from pending
        //     assign/steal tasks for idle workers
        //     current_time++
        // }
        //
        // result.makespan = current_time;
        // for each worker: result.utilization[w] = busy_time[w] / makespan;

        return result;
    }
};

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q20: Thread Pool with Work Stealing ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Independent tasks distributed evenly
    {
        total++;
        ThreadPool pool(2);
        pool.submit(0, 3);  // -> worker 0
        pool.submit(1, 3);  // -> worker 1
        pool.submit(2, 3);  // -> worker 0
        pool.submit(3, 3);  // -> worker 1
        auto result = pool.run();

        // 2 workers, 4 tasks of duration 3 each -> each worker gets 2 -> makespan = 6
        bool ok = (result.makespan == 6);
        printf("%s [4 independent tasks, 2 workers: makespan=%d (exp 6)]\n",
               ok ? "PASS" : "FAIL", result.makespan);
        if (ok) passed++;
    }

    // Test 2: Dependencies respected
    {
        total++;
        ThreadPool pool(2);
        pool.submit(0, 2);           // no deps
        pool.submit(1, 2, {0});      // depends on 0
        pool.submit(2, 2, {1});      // depends on 1
        auto result = pool.run();

        // Chain: 0 -> 1 -> 2, critical path = 6
        bool ok = (result.makespan == 6);

        // Verify ordering in schedule
        map<int, int> end_times;
        for (auto& e : result.schedule) end_times[e.task_id] = e.end_time;
        map<int, int> start_times;
        for (auto& e : result.schedule) start_times[e.task_id] = e.start_time;

        bool deps_ok = true;
        if (start_times.count(1) && end_times.count(0))
            deps_ok = deps_ok && (start_times[1] >= end_times[0]);
        if (start_times.count(2) && end_times.count(1))
            deps_ok = deps_ok && (start_times[2] >= end_times[1]);

        ok = ok && deps_ok;
        printf("%s [Dependency chain 0->1->2: makespan=%d (exp 6), deps respected=%s]\n",
               ok ? "PASS" : "FAIL", result.makespan, deps_ok ? "yes" : "no");
        if (ok) passed++;
    }

    // Test 3: Work stealing occurs with imbalanced load
    {
        total++;
        ThreadPool pool(2);
        pool.submit(0, 1);  // -> worker 0
        pool.submit(1, 1);  // -> worker 1
        pool.submit(2, 1);  // -> worker 0
        pool.submit(3, 1);  // -> worker 1
        pool.submit(4, 1);  // -> worker 0 (3 tasks)
        // Worker 0 has 3 tasks, worker 1 has 2. Worker 1 finishes first, should steal.
        auto result = pool.run();

        bool ok = (result.num_steals > 0);
        printf("%s [Imbalanced load: steals=%d (exp >0), makespan=%d]\n",
               ok ? "PASS" : "FAIL", result.num_steals, result.makespan);
        if (ok) passed++;
    }

    // Test 4: Makespan matches critical path
    {
        total++;
        // Diamond dependency:  0 -> {1, 2} -> 3
        ThreadPool pool(2);
        pool.submit(0, 1);
        pool.submit(1, 3, {0});
        pool.submit(2, 2, {0});
        pool.submit(3, 1, {1, 2});
        auto result = pool.run();

        // Critical path: 0(1) + max(1(3), 2(2)) + 3(1) = 1 + 3 + 1 = 5
        bool ok = (result.makespan == 5);
        printf("%s [Diamond DAG: makespan=%d (exp 5)]\n",
               ok ? "PASS" : "FAIL", result.makespan);
        if (ok) passed++;
    }

    // Test 5: Single worker -> sequential execution
    {
        total++;
        ThreadPool pool(1);
        pool.submit(0, 2);
        pool.submit(1, 3);
        pool.submit(2, 1);
        auto result = pool.run();

        // All tasks run sequentially: 2 + 3 + 1 = 6
        bool ok = (result.makespan == 6);
        bool util_ok = (fabs(result.utilization[0] - 1.0) < 0.01);
        ok = ok && util_ok;
        printf("%s [Single worker: makespan=%d (exp 6), utilization=%.2f (exp 1.00)]\n",
               ok ? "PASS" : "FAIL", result.makespan,
               result.utilization.empty() ? 0.0 : result.utilization[0]);
        if (ok) passed++;
    }

    // Test 6: Many small tasks with no deps -> good utilization
    {
        total++;
        ThreadPool pool(4);
        for (int i = 0; i < 20; i++) {
            pool.submit(i, 1);
        }
        auto result = pool.run();

        // 20 tasks of duration 1 across 4 workers -> makespan = 5
        // Each worker should be busy 5/5 = 100% utilization
        bool makespan_ok = (result.makespan == 5);
        double min_util = 1.0;
        for (auto u : result.utilization) min_util = min(min_util, u);
        bool util_ok = (min_util >= 0.8);  // allow some slack

        bool ok = makespan_ok && util_ok;
        printf("%s [20 small tasks, 4 workers: makespan=%d (exp 5), min_util=%.2f (exp >=0.8)]\n",
               ok ? "PASS" : "FAIL", result.makespan, min_util);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
