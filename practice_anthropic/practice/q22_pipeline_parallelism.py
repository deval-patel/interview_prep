"""
Q22 — Pipeline Parallelism Simulator [Medium-Hard]

Topic: Parallel Programming — Pipeline Parallelism

Problem:
  Simulate pipeline parallelism for model training across multiple stages
  (GPipe-style). In pipeline parallelism, a model is split into stages
  across devices. Microbatches flow through the pipeline, overlapping
  computation across stages.

  Implement:
  1. PipelineScheduler(num_stages, num_microbatches) — setup pipeline
  2. Each stage processes one microbatch at a time, takes stage_latencies[i] cycles
  3. Naive (sequential) vs pipelined execution
  4. Compute: bubble ratio, throughput, speedup vs naive

  In pipelined execution:
  - Stage i can start microbatch m+1 only after stage i finishes microbatch m
  - Stage i+1 can start microbatch m only after stage i finishes microbatch m
  - The "bubble" is idle time where stages sit empty at pipeline start/end

Constraints:
  - num_stages >= 1, num_microbatches >= 1
  - stage_latencies[i] > 0 for all i
  - Track per-stage busy and idle time

Key Concepts:
  - Pipeline parallelism (GPipe, PipeDream)
  - Bubble overhead = (num_stages - 1) / (num_stages - 1 + num_microbatches)
  - Throughput approaches 1/max(stage_latency) as microbatches -> infinity
  - Speedup over naive execution
"""


# ============ YOUR IMPLEMENTATION ============

def naive_execution(stage_latencies: list[int], num_microbatches: int) -> int:
    """
    No pipelining. Each microbatch runs through all stages
    sequentially before the next microbatch begins.

    Total time = num_microbatches * sum(stage_latencies)
    """
    # TODO: Compute total execution time without any pipelining
    return 0


class PipelineScheduler:
    """
    Simulates GPipe-style pipeline parallelism.

    For pipelined execution, track when each (stage, microbatch) pair starts
    and finishes. The start time for (stage s, microbatch m) is:
      start[s][m] = max(finish[s][m-1], finish[s-1][m])
      finish[s][m] = start[s][m] + stage_latencies[s]

    Bubble ratio = total_idle_time / (num_stages * total_time)
    where total_idle_time = num_stages * total_time - sum(all busy times)
    """

    def __init__(self, num_stages: int, num_microbatches: int,
                 stage_latencies: list[int]):
        self.num_stages = num_stages
        self.num_microbatches = num_microbatches
        self.stage_latencies = stage_latencies
        # start_time[s][m] = cycle when stage s begins processing microbatch m
        self.start_time = [[0] * num_microbatches for _ in range(num_stages)]
        # finish_time[s][m] = cycle when stage s finishes processing microbatch m
        self.finish_time = [[0] * num_microbatches for _ in range(num_stages)]

    def simulate(self):
        """
        Fill in start_time and finish_time for all (stage, microbatch) pairs.

        Constraints:
          start_time[0][0] = 0
          start_time[s][0] = finish_time[s-1][0]  (first microbatch flows through)
          start_time[0][m] = finish_time[0][m-1]  (stage 0 processes sequentially)
          start_time[s][m] = max(finish_time[s][m-1], finish_time[s-1][m])
          finish_time[s][m] = start_time[s][m] + stage_latencies[s]
        """
        # TODO: Fill start_time and finish_time arrays
        pass

    def get_stats(self) -> dict:
        """
        Compute pipeline statistics after simulation.

        Returns dict with:
          total_time: finish_time[num_stages-1][num_microbatches-1]
          bubble_ratio: sum(idle_times) / (num_stages * total_time)
          throughput: num_microbatches / total_time
          stage_busy_time: list of per-stage busy cycles
          stage_idle_time: list of per-stage idle cycles
        """
        # TODO: Compute all stats from start_time/finish_time
        return {
            "total_time": 0,
            "bubble_ratio": 0.0,
            "throughput": 0.0,
            "stage_busy_time": [0] * self.num_stages,
            "stage_idle_time": [0] * self.num_stages,
        }


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q22: Pipeline Parallelism Simulator ===\n")
    passed = 0
    total = 0

    # Test 1: Naive total time = num_microbatches * sum(stage_latencies)
    total += 1
    latencies = [10, 20, 15]
    naive = naive_execution(latencies, 4)
    expected = 4 * (10 + 20 + 15)  # 180
    if naive == expected:
        print("PASS [Naive total time = mb * sum(latencies)]")
        passed += 1
    else:
        print(f"FAIL [Naive total time = mb * sum(latencies)] expected {expected}, got {naive}")

    # Test 2: Pipelined is faster than naive for >1 microbatch
    total += 1
    latencies = [10, 10, 10]
    naive = naive_execution(latencies, 4)
    ps = PipelineScheduler(3, 4, latencies)
    ps.simulate()
    stats = ps.get_stats()
    if 0 < stats["total_time"] < naive:
        print(f"PASS [Pipelined faster than naive (pipe={stats['total_time']} < naive={naive})]")
        passed += 1
    else:
        print(f"FAIL [Pipelined faster than naive (pipe={stats['total_time']}, naive={naive})]")

    # Test 3: Bubble ratio decreases with more microbatches
    total += 1
    latencies = [10, 10, 10]
    ps4 = PipelineScheduler(3, 4, latencies)
    ps4.simulate()
    stats4 = ps4.get_stats()
    ps16 = PipelineScheduler(3, 16, latencies)
    ps16.simulate()
    stats16 = ps16.get_stats()
    if stats4["bubble_ratio"] > 0.0 and stats16["bubble_ratio"] < stats4["bubble_ratio"]:
        print(f"PASS [Bubble ratio decreases: 4mb={stats4['bubble_ratio']:.3f} > 16mb={stats16['bubble_ratio']:.3f}]")
        passed += 1
    else:
        print(f"FAIL [Bubble ratio decreases: 4mb={stats4['bubble_ratio']:.3f}, 16mb={stats16['bubble_ratio']:.3f}]")

    # Test 4: Equal stage latencies -> optimal pipeline (known formula)
    # With equal latencies L, total_time = (S + M - 1) * L
    total += 1
    latencies = [10, 10, 10]
    ps = PipelineScheduler(3, 5, latencies)
    ps.simulate()
    stats = ps.get_stats()
    expected = (3 + 5 - 1) * 10  # 70
    if stats["total_time"] == expected:
        print(f"PASS [Equal latencies: total={stats['total_time']}, expected={expected}]")
        passed += 1
    else:
        print(f"FAIL [Equal latencies: total={stats['total_time']}, expected={expected}]")

    # Test 5: Unequal stages -> bottleneck is slowest stage
    # total_time = sum(latencies) + (M-1) * max(latencies)
    total += 1
    latencies = [5, 20, 10]
    ps = PipelineScheduler(3, 6, latencies)
    ps.simulate()
    stats = ps.get_stats()
    expected = (5 + 20 + 10) + (6 - 1) * 20  # 35 + 100 = 135
    if stats["total_time"] == expected:
        print(f"PASS [Unequal stages bottleneck: total={stats['total_time']}, expected={expected}]")
        passed += 1
    else:
        print(f"FAIL [Unequal stages bottleneck: total={stats['total_time']}, expected={expected}]")

    # Test 6: Throughput approaches 1/max(stage_latency) with many microbatches
    total += 1
    latencies = [5, 20, 10]
    ps = PipelineScheduler(3, 100, latencies)
    ps.simulate()
    stats = ps.get_stats()
    ideal_throughput = 1.0 / 20.0
    if abs(stats["throughput"] - ideal_throughput) < 0.005:
        print(f"PASS [Throughput -> 1/max(lat): actual={stats['throughput']:.4f}, ideal={ideal_throughput:.4f}]")
        passed += 1
    else:
        print(f"FAIL [Throughput -> 1/max(lat): actual={stats['throughput']:.4f}, ideal={ideal_throughput:.4f}]")

    print(f"\nPassed {passed}/{total} tests")
