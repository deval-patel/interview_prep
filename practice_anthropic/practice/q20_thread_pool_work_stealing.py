"""
Q20 — Thread Pool with Work Stealing [Medium-Hard]

Topic: Parallel Programming — Concurrency

Problem:
  Simulate a thread pool with work stealing. No actual threads are needed —
  simulate the scheduling algorithm step by step.

  Implement:
  1. ThreadPool(num_workers) — create pool with N worker queues
  2. submit(task_id, duration, dependencies) — add task with dependencies
  3. run() — simulate execution with work stealing, return schedule

  Work stealing policy:
    When a worker's local queue is empty and it is idle, it steals a task
    from the worker with the longest queue. If multiple workers have the
    same longest queue length, steal from the lowest-indexed one.

  Dependency handling:
    A task can only begin execution once ALL of its dependencies have
    completed. Tasks with unmet dependencies remain in a pending set.

  Task assignment:
    Tasks without dependencies are distributed round-robin to workers
    upon submission (worker = task_index % num_workers).

  Track:
    - Total makespan (time when the last task finishes)
    - Per-worker utilization (busy_time / makespan)
    - Number of steals that occurred
    - Schedule: list of (task_id, worker_id, start_time, end_time)

Constraints:
  - Task IDs are unique non-negative integers
  - Duration is a positive integer (time units)
  - Dependencies reference previously submitted task IDs
  - A stolen task is removed from the victim's queue

Key Concepts:
  - Work stealing for load balancing
  - Dependency-aware task scheduling
  - Critical path analysis
  - Worker utilization metrics
"""

from collections import deque


class Task:
    def __init__(self, task_id, duration, dependencies=None):
        self.id = task_id
        self.duration = duration
        self.dependencies = dependencies or []


class ScheduleEntry:
    def __init__(self, task_id, worker_id, start_time, end_time):
        self.task_id = task_id
        self.worker_id = worker_id
        self.start_time = start_time
        self.end_time = end_time


class RunResult:
    def __init__(self):
        self.makespan = 0
        self.utilization = []       # per-worker utilization
        self.num_steals = 0
        self.schedule = []          # list of ScheduleEntry


class ThreadPool:
    def __init__(self, num_workers):
        self.num_workers = num_workers
        # TODO: Initialize data structures:
        #   - Per-worker task queues (deque)
        #   - Pending tasks (dependencies not yet met)
        #   - Completed task set
        #   - Round-robin counter for initial assignment

    def submit(self, task_id, duration, dependencies=None):
        """
        Add a task to the pool.
        If the task has no unmet dependencies, assign round-robin to a worker queue.
        Otherwise, add to pending set.
        """
        # TODO: Create Task and either enqueue or defer based on dependencies
        pass

    def run(self):
        """
        Simulate execution step by step.

        Each time step:
          1. Advance running tasks. If a task completes:
             - Mark completed
             - Check pending: enqueue any task whose deps are all met
          2. For each idle worker:
             a. If local queue non-empty, dequeue and start
             b. Else steal from the worker with the longest queue
          3. If all done, stop.

        Return RunResult.
        """
        result = RunResult()
        result.utilization = [0.0] * self.num_workers

        # TODO: Implement simulation loop
        #
        # current_time = 0
        # worker_busy_time = [0] * self.num_workers
        #
        # while tasks_remain:
        #     advance running tasks, check completions
        #     release newly-ready from pending
        #     assign/steal for idle workers
        #     current_time += 1
        #
        # result.makespan = current_time
        # result.utilization = [bt / makespan for bt in worker_busy_time]

        return result


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q20: Thread Pool with Work Stealing ===\n")
    passed = 0
    total = 0

    # Test 1: Independent tasks distributed evenly
    total += 1
    pool = ThreadPool(2)
    pool.submit(0, 3)
    pool.submit(1, 3)
    pool.submit(2, 3)
    pool.submit(3, 3)
    result = pool.run()
    ok = result.makespan == 6
    print(f"{'PASS' if ok else 'FAIL'} [4 independent tasks, 2 workers: "
          f"makespan={result.makespan} (exp 6)]")
    if ok:
        passed += 1

    # Test 2: Dependencies respected
    total += 1
    pool = ThreadPool(2)
    pool.submit(0, 2)
    pool.submit(1, 2, [0])
    pool.submit(2, 2, [1])
    result = pool.run()
    ok = result.makespan == 6
    end_times = {e.task_id: e.end_time for e in result.schedule}
    start_times = {e.task_id: e.start_time for e in result.schedule}
    deps_ok = True
    if 1 in start_times and 0 in end_times:
        deps_ok = deps_ok and start_times[1] >= end_times[0]
    if 2 in start_times and 1 in end_times:
        deps_ok = deps_ok and start_times[2] >= end_times[1]
    ok = ok and deps_ok
    print(f"{'PASS' if ok else 'FAIL'} [Dependency chain 0->1->2: "
          f"makespan={result.makespan} (exp 6), deps respected={deps_ok}]")
    if ok:
        passed += 1

    # Test 3: Work stealing with imbalanced load
    total += 1
    pool = ThreadPool(2)
    pool.submit(0, 1)
    pool.submit(1, 1)
    pool.submit(2, 1)
    pool.submit(3, 1)
    pool.submit(4, 1)  # worker 0 gets 3, worker 1 gets 2
    result = pool.run()
    ok = result.num_steals > 0
    print(f"{'PASS' if ok else 'FAIL'} [Imbalanced load: "
          f"steals={result.num_steals} (exp >0), makespan={result.makespan}]")
    if ok:
        passed += 1

    # Test 4: Makespan matches critical path (diamond DAG)
    total += 1
    pool = ThreadPool(2)
    pool.submit(0, 1)
    pool.submit(1, 3, [0])
    pool.submit(2, 2, [0])
    pool.submit(3, 1, [1, 2])
    result = pool.run()
    ok = result.makespan == 5
    print(f"{'PASS' if ok else 'FAIL'} [Diamond DAG: makespan={result.makespan} (exp 5)]")
    if ok:
        passed += 1

    # Test 5: Single worker -> sequential
    total += 1
    pool = ThreadPool(1)
    pool.submit(0, 2)
    pool.submit(1, 3)
    pool.submit(2, 1)
    result = pool.run()
    ok = result.makespan == 6
    util_ok = len(result.utilization) > 0 and abs(result.utilization[0] - 1.0) < 0.01
    ok = ok and util_ok
    util_val = result.utilization[0] if result.utilization else 0.0
    print(f"{'PASS' if ok else 'FAIL'} [Single worker: "
          f"makespan={result.makespan} (exp 6), utilization={util_val:.2f} (exp 1.00)]")
    if ok:
        passed += 1

    # Test 6: Many small tasks -> good utilization
    total += 1
    pool = ThreadPool(4)
    for i in range(20):
        pool.submit(i, 1)
    result = pool.run()
    makespan_ok = result.makespan == 5
    min_util = min(result.utilization) if result.utilization else 0.0
    util_ok = min_util >= 0.8
    ok = makespan_ok and util_ok
    print(f"{'PASS' if ok else 'FAIL'} [20 small tasks, 4 workers: "
          f"makespan={result.makespan} (exp 5), min_util={min_util:.2f} (exp >=0.8)]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
