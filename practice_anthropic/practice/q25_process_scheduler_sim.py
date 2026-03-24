"""
Q25 — Process Scheduler Simulation [Medium-Hard] (Q3-Style Problem Solving)

Topic: Pure Problem Solving — Simulation & State Machine

Problem:
  You are designing a scheduler for a simple multi-core processor.
  Processes arrive at different times with different durations and priorities.
  The scheduler must assign processes to cores following specific rules.

  Implement a ProcessScheduler that simulates execution step-by-step.

  Rules:
  1. There are C cores, numbered 0 to C-1.
  2. Processes arrive at specified timestamps with a duration and priority (1=highest).
  3. Each core runs one process at a time (non-preemptive within a quantum).
  4. Time is divided into quanta of size Q. At the end of each quantum,
     the scheduler can preempt a running process and reassign cores.
  5. At each scheduling decision point (start, or end of quantum):
     a. Collect all ready processes (arrived and not yet complete).
     b. Sort by priority (lower number = higher priority), break ties by arrival
        time (earlier first), then by process ID (lower first).
     c. Assign top-C processes to cores. If a process was already running on
        a core, prefer to keep it on the same core (avoid migration).
     d. Remaining ready processes wait in the queue.
  6. A process that gets preempted retains its remaining work.
  7. When a process finishes mid-quantum, the core becomes idle for the rest
     of that quantum (it doesn't pull a new process until the next decision point).

  Track:
  - Completion time for each process
  - Per-core utilization (busy cycles / total cycles)
  - Number of preemptions
  - Number of context switches (process changes on a core)
  - Average turnaround time (completion - arrival)
  - Average waiting time (turnaround - duration)

  Additionally implement:
  - priority_boost(process_id, new_priority): change a waiting process's priority
    (takes effect at next scheduling decision)
  - cancel(process_id): remove a process from the system (if not yet complete)

Constraints:
  - 1 <= C <= 8
  - 1 <= Q <= 100
  - 1 <= priority <= 10
  - Process IDs are unique non-negative integers
  - Processes that have already completed cannot be cancelled or boosted

Key Concepts:
  - Priority scheduling with preemption at quantum boundaries
  - Core affinity (prefer same core to reduce migration)
  - State machine: each process is NEW -> READY -> RUNNING -> (READY | DONE)
  - Simulation with discrete time steps
"""

from dataclasses import dataclass, field
from enum import Enum


class ProcessState(Enum):
    NEW = "NEW"
    READY = "READY"
    RUNNING = "RUNNING"
    DONE = "DONE"
    CANCELLED = "CANCELLED"


@dataclass
class Process:
    pid: int
    arrival_time: int
    duration: int          # total cycles needed
    priority: int          # 1 = highest
    remaining: int = 0     # cycles still needed (set to duration on init)
    state: ProcessState = ProcessState.NEW
    core_id: int = -1      # which core is it running on (-1 = none)
    completion_time: int = -1
    # Track for stats
    total_waited: int = 0  # cycles spent in READY state

    def __post_init__(self):
        self.remaining = self.duration


# ============ YOUR IMPLEMENTATION ============

class ProcessScheduler:
    def __init__(self, num_cores: int, quantum: int):
        """
        Args:
            num_cores: number of CPU cores
            quantum: scheduling quantum size (cycles)
        """
        self.num_cores = num_cores
        self.quantum = quantum
        self.processes = {}          # pid -> Process
        self.core_assignment = {}    # core_id -> pid or None
        self.current_time = 0
        self.total_preemptions = 0
        self.total_context_switches = 0
        self.events = []             # list of scheduling events for debugging

        # TODO: Initialize core assignments
        pass

    def add_process(self, pid: int, arrival_time: int, duration: int, priority: int):
        """
        Register a process. It becomes READY at arrival_time.

        Args:
            pid: unique process ID
            arrival_time: when the process arrives (>= 0)
            duration: total cycles of work needed (> 0)
            priority: priority level (1 = highest)
        """
        # TODO: Create and store the process
        pass

    def priority_boost(self, pid: int, new_priority: int):
        """
        Change a process's priority. Only works if process is READY or NEW.
        Raises ValueError if process is DONE, CANCELLED, or RUNNING.
        """
        # TODO
        pass

    def cancel(self, pid: int):
        """
        Cancel a process. Works if process is NEW, READY, or RUNNING.
        If RUNNING, the core becomes idle immediately.
        Raises ValueError if process is already DONE or CANCELLED.
        """
        # TODO
        pass

    def _get_ready_processes(self):
        """
        Return list of processes that are READY or RUNNING at current_time,
        sorted by (priority, arrival_time, pid).
        Also include NEW processes whose arrival_time <= current_time.
        """
        # TODO: Collect eligible processes and sort them
        pass

    def _schedule(self):
        """
        Make scheduling decisions at current decision point.

        1. Transition NEW processes to READY if arrived.
        2. Get sorted list of ready/running processes.
        3. Top num_cores get assigned to cores.
           - Prefer keeping a process on its current core (affinity).
        4. The rest are preempted/wait.
        5. Track preemptions and context switches.

        A preemption occurs when a RUNNING process is moved to READY.
        A context switch occurs when a core's assigned process changes
        (including from None to a process, or process to a different process).
        """
        # TODO: Implement scheduling logic
        pass

    def _run_quantum(self):
        """
        Simulate one quantum of execution.

        For each core with an assigned process:
          - Run for min(quantum, process.remaining) cycles
          - Decrease remaining
          - If remaining == 0, mark as DONE with completion_time

        For READY processes not on any core:
          - Increase total_waited by quantum (or by actual time if less)

        Advance current_time by quantum.
        """
        # TODO: Simulate execution for one quantum
        pass

    def run(self):
        """
        Run the simulation until all processes are DONE or CANCELLED.

        Loop:
          1. _schedule() — make assignments
          2. _run_quantum() — execute one quantum
          3. Repeat until no processes are NEW, READY, or RUNNING

        Returns:
          dict with keys:
            completion_times: dict of pid -> completion_time (for DONE processes)
            avg_turnaround: average (completion_time - arrival_time)
            avg_waiting: average (turnaround - duration)
            core_utilization: list of (busy_cycles / total_cycles) per core
            total_preemptions: int
            total_context_switches: int
            total_time: final current_time
        """
        # TODO: Run simulation loop and compute stats
        return {
            "completion_times": {},
            "avg_turnaround": 0.0,
            "avg_waiting": 0.0,
            "core_utilization": [0.0] * self.num_cores,
            "total_preemptions": 0,
            "total_context_switches": 0,
            "total_time": 0,
        }


# ============ TEST FRAMEWORK ============

def _run_test(name, fn):
    """Run a single test, catching exceptions from unimplemented stubs."""
    try:
        return fn()
    except Exception as e:
        print(f"FAIL [{name}] — {type(e).__name__}: {e}")
        return False


passed = 0
total = 0

if __name__ == "__main__":
    print("=== Q25: Process Scheduler Simulation ===\n")

    def _tests():
        global passed, total

        total += 1
        def t1():
            sched = ProcessScheduler(num_cores=1, quantum=10)
            sched.add_process(0, arrival_time=0, duration=25, priority=1)
            result = sched.run()
            ok = (result["completion_times"][0] == 25 and result["total_preemptions"] == 0)
            print(f"{'PASS' if ok else 'FAIL'} [Single process: completes at "
                  f"{result['completion_times'].get(0)}(exp 25), "
                  f"preemptions={result['total_preemptions']}(exp 0)]")
            return ok
        if _run_test("Single process", t1): passed += 1

        total += 1
        def t2():
            sched = ProcessScheduler(num_cores=1, quantum=5)
            sched.add_process(0, arrival_time=0, duration=10, priority=5)
            sched.add_process(1, arrival_time=0, duration=10, priority=1)
            result = sched.run()
            ok = (result["completion_times"][1] == 10 and result["completion_times"][0] == 20)
            print(f"{'PASS' if ok else 'FAIL'} [Priority: p1@{result['completion_times'].get(1)}(exp 10), "
                  f"p0@{result['completion_times'].get(0)}(exp 20)]")
            return ok
        if _run_test("Priority ordering", t2): passed += 1

        total += 1
        def t3():
            sched = ProcessScheduler(num_cores=2, quantum=10)
            sched.add_process(0, arrival_time=0, duration=20, priority=1)
            sched.add_process(1, arrival_time=0, duration=20, priority=2)
            result = sched.run()
            ok = (result["completion_times"][0] == 20 and result["completion_times"][1] == 20)
            print(f"{'PASS' if ok else 'FAIL'} [Parallel: both at 20, got "
                  f"p0={result['completion_times'].get(0)}, p1={result['completion_times'].get(1)}]")
            return ok
        if _run_test("Parallel exec", t3): passed += 1

        total += 1
        def t4():
            sched = ProcessScheduler(num_cores=1, quantum=5)
            sched.add_process(0, arrival_time=0, duration=20, priority=5)
            sched.add_process(1, arrival_time=3, duration=10, priority=1)
            result = sched.run()
            ok = (result["completion_times"][1] == 15 and
                  result["completion_times"][0] == 30 and
                  result["total_preemptions"] >= 1)
            print(f"{'PASS' if ok else 'FAIL'} [Preemption: p1@{result['completion_times'].get(1)}(exp 15), "
                  f"p0@{result['completion_times'].get(0)}(exp 30)]")
            return ok
        if _run_test("Preemption", t4): passed += 1

        total += 1
        def t5():
            sched = ProcessScheduler(num_cores=2, quantum=10)
            sched.add_process(0, arrival_time=0, duration=30, priority=1)
            result = sched.run()
            ok = (result["core_utilization"][0] > 0.9 and result["core_utilization"][1] < 0.01)
            print(f"{'PASS' if ok else 'FAIL'} [Utilization: core0={result['core_utilization'][0]:.2f}, "
                  f"core1={result['core_utilization'][1]:.2f}]")
            return ok
        if _run_test("Core utilization", t5): passed += 1

        total += 1
        def t6():
            sched = ProcessScheduler(num_cores=1, quantum=5)
            sched.add_process(0, arrival_time=0, duration=10, priority=5)
            sched.add_process(1, arrival_time=0, duration=10, priority=3)
            sched.priority_boost(0, 1)
            result = sched.run()
            ok = result["completion_times"][0] < result["completion_times"][1]
            print(f"{'PASS' if ok else 'FAIL'} [Priority boost: p0@{result['completion_times'].get(0)} "
                  f"< p1@{result['completion_times'].get(1)}]")
            return ok
        if _run_test("Priority boost", t6): passed += 1

        total += 1
        def t7():
            sched = ProcessScheduler(num_cores=1, quantum=5)
            sched.add_process(0, arrival_time=0, duration=10, priority=1)
            sched.add_process(1, arrival_time=0, duration=10, priority=2)
            sched.cancel(1)
            result = sched.run()
            ok = (0 in result["completion_times"] and 1 not in result["completion_times"])
            print(f"{'PASS' if ok else 'FAIL'} [Cancel: p0={0 in result['completion_times']}, "
                  f"p1 absent={1 not in result['completion_times']}]")
            return ok
        if _run_test("Cancel", t7): passed += 1

        total += 1
        def t8():
            sched = ProcessScheduler(num_cores=1, quantum=10)
            sched.add_process(0, arrival_time=0, duration=10, priority=1)
            sched.add_process(1, arrival_time=0, duration=10, priority=2)
            result = sched.run()
            ok = (abs(result["avg_turnaround"] - 15.0) < 0.1 and
                  abs(result["avg_waiting"] - 5.0) < 0.1)
            print(f"{'PASS' if ok else 'FAIL'} [Avg turnaround={result['avg_turnaround']:.1f}(exp 15), "
                  f"waiting={result['avg_waiting']:.1f}(exp 5)]")
            return ok
        if _run_test("Avg times", t8): passed += 1

        total += 1
        def t9():
            sched = ProcessScheduler(num_cores=1, quantum=10)
            sched.add_process(2, arrival_time=0, duration=10, priority=1)
            sched.add_process(0, arrival_time=0, duration=10, priority=1)
            sched.add_process(1, arrival_time=0, duration=10, priority=1)
            result = sched.run()
            ok = (result["completion_times"][0] < result["completion_times"][1] <
                  result["completion_times"][2])
            print(f"{'PASS' if ok else 'FAIL'} [Tie-break: p0@{result['completion_times'].get(0)}, "
                  f"p1@{result['completion_times'].get(1)}, p2@{result['completion_times'].get(2)}]")
            return ok
        if _run_test("Tie-breaking", t9): passed += 1

        total += 1
        def t10():
            sched = ProcessScheduler(num_cores=1, quantum=5)
            sched.add_process(0, arrival_time=0, duration=15, priority=3)
            sched.add_process(1, arrival_time=7, duration=5, priority=1)
            sched.add_process(2, arrival_time=12, duration=5, priority=2)
            result = sched.run()
            ok = (result["completion_times"][1] == 15 and
                  result["completion_times"][2] == 20 and
                  result["completion_times"][0] == 25)
            print(f"{'PASS' if ok else 'FAIL'} [Staggered: p1@{result['completion_times'].get(1)}(exp 15), "
                  f"p2@{result['completion_times'].get(2)}(exp 20), "
                  f"p0@{result['completion_times'].get(0)}(exp 25)]")
            return ok
        if _run_test("Staggered arrivals", t10): passed += 1

    _tests()
    print(f"\nPassed {passed}/{total} tests")
