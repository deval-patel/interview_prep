"""
Q22-EXT — Advanced Pipeline Parallelism: 1F1B, Memory & Interleaving [Hard]

Topic: Distributed Systems — Pipeline Parallelism

Problem:
  Extend the basic GPipe pipeline scheduler to support the 1F1B (one forward,
  one backward) schedule, memory usage tracking, and interleaved stages.

  Implement:
  1. GPipeScheduler — baseline: all forwards, then all backwards
  2. OneF1BScheduler — PipeDream-style 1F1B schedule (lower peak memory)
  3. Memory tracking — count how many activations each stage holds at any time
  4. InterleavedScheduler — each device handles multiple virtual stages

  Background:
  In training, each microbatch does a forward pass then a backward pass.
  The backward pass for microbatch m at stage s can only start after:
    - The forward pass for m at stage s is done
    - The backward pass for m at stage s+1 is done (gradients flow backwards)

  GPipe: run ALL forwards first, then ALL backwards.
    Simple but high peak memory (all activations stored simultaneously).

  1F1B: after the pipeline fills, alternate 1 forward + 1 backward per stage.
    Lower peak memory because activations are freed earlier.

  Memory model:
    - A forward pass at stage s for microbatch m allocates 1 activation unit
    - A backward pass at stage s for microbatch m frees 1 activation unit
    - Peak memory per stage = max activations held at any point

Constraints:
  - Forward latencies and backward latencies may differ per stage
  - backward_latency[s] is typically ~2x forward_latency[s]
  - Forward order: stage 0 -> 1 -> ... -> S-1
  - Backward order: stage S-1 -> S-2 -> ... -> 0
  - Memory tracked per stage, peak memory is the max across all stages

Key Concepts:
  - GPipe vs PipeDream/1F1B tradeoffs
  - Memory-compute tradeoff in pipeline schedules
  - Bubble ratio with forward+backward passes
  - Interleaved pipelines reduce bubble at cost of more communication
"""


# ============ YOUR IMPLEMENTATION ============

class GPipeScheduler:
    """
    GPipe schedule: all forwards first, then all backwards.

    Timeline:
      Forward phase:
        fwd_start[s][m] = max(fwd_end[s][m-1], fwd_end[s-1][m])
        fwd_end[s][m] = fwd_start[s][m] + fwd_latency[s]

      Backward phase (starts after ALL forwards complete):
        Backward flows in reverse: stage S-1 first, then S-2, ..., 0
        Microbatches also go in reverse order for backward.
        bwd_start[s][m] = max(bwd_end[s][m+1], bwd_end[s+1][m])
          where m+1 means the next microbatch to be processed backward at this stage
          (backward processes microbatches in reverse: M-1, M-2, ..., 0)
        bwd_end[s][m] = bwd_start[s][m] + bwd_latency[s]

      Actually, for GPipe backward ordering:
        The last stage processes backward first. For the backward pass,
        stage S-1 processes microbatch 0 first (the first one that completed forward).
        Then microbatch 0 flows backward through stages S-2, S-1, etc.

        bwd_start[S-1][0] = max of (all forwards done)
        bwd_start[s][m] = max(bwd_end[s][m-1], bwd_end[s+1][m])
        bwd_end[s][m] = bwd_start[s][m] + bwd_latency[s]
    """

    def __init__(self, num_stages, num_microbatches, fwd_latency, bwd_latency):
        self.S = num_stages
        self.M = num_microbatches
        self.fwd_latency = fwd_latency
        self.bwd_latency = bwd_latency

        self.fwd_start = [[0] * num_microbatches for _ in range(num_stages)]
        self.fwd_end = [[0] * num_microbatches for _ in range(num_stages)]
        self.bwd_start = [[0] * num_microbatches for _ in range(num_stages)]
        self.bwd_end = [[0] * num_microbatches for _ in range(num_stages)]

    def simulate(self):
        """
        Fill fwd_start, fwd_end, bwd_start, bwd_end.

        Forward pass: standard pipeline fill (same as Q22).
        Backward pass: starts after all forwards complete.
          Process microbatches 0, 1, ..., M-1 at stages S-1, S-2, ..., 0.
          bwd_start[s][m] depends on:
            - bwd_end[s][m-1] (previous microbatch at same stage)
            - bwd_end[s+1][m] (same microbatch at next stage — gradients arrive)
        """
        # TODO: Fill all four timing arrays
        pass

    def get_stats(self):
        """
        Returns dict with:
          total_time: when the last backward pass completes
          bubble_ratio: idle / (S * total_time)
          peak_memory: max activations held by any single stage at any point
          per_stage_peak_memory: list of peak memory per stage
        """
        # TODO:
        # total_time = max of all bwd_end values
        # busy_time per stage = sum of fwd_latency * M + bwd_latency * M
        # peak_memory: at any point in time, stage s holds one activation unit
        #   for each microbatch that has completed forward but not yet completed backward
        #   The peak for GPipe is M (all microbatches forwarded before any backward)
        return {
            "total_time": 0,
            "bubble_ratio": 0.0,
            "peak_memory": 0,
            "per_stage_peak_memory": [0] * self.S,
        }


class OneF1BScheduler:
    """
    1F1B (One Forward, One Backward) schedule — PipeDream-Flush style.

    Phases:
      1. Warmup: fill the pipeline with forwards only.
         Stage s does (S - s) forward passes before starting any backward.
         This is also called the "ramp up" phase.

      2. Steady state: alternate 1 forward + 1 backward per step per stage.
         Each stage does one forward then one backward, keeping the number
         of in-flight microbatches roughly constant.

      3. Cooldown: drain remaining backwards after all forwards are done.

    Scheduling rules:
      Forward:
        fwd_start[s][m] = max(fwd_end[s][m-1], fwd_end[s-1][m])
        (same as GPipe)

      Backward:
        bwd_start[s][m] = max(bwd_end[s][m-1], bwd_end[s+1][m], fwd_end[s][m])
        The backward for microbatch m at stage s must also wait for the
        forward of m at stage s to finish (need activations).

      1F1B ordering at stage s:
        During warmup, do (S - 1 - s) forwards.
        Then alternate: 1 backward (oldest in-flight mb), 1 forward (next mb).
        During cooldown, only backwards.

      For simplicity, you can implement by scheduling events chronologically:
        - Maintain per-stage queues of which microbatch to forward/backward next
        - A stage can do a backward for mb m only after fwd_end[s][m] and bwd_end[s+1][m]
        - A stage does backward before forward when both are ready (in steady state)
    """

    def __init__(self, num_stages, num_microbatches, fwd_latency, bwd_latency):
        self.S = num_stages
        self.M = num_microbatches
        self.fwd_latency = fwd_latency
        self.bwd_latency = bwd_latency

        self.fwd_start = [[0] * num_microbatches for _ in range(num_stages)]
        self.fwd_end = [[0] * num_microbatches for _ in range(num_stages)]
        self.bwd_start = [[0] * num_microbatches for _ in range(num_stages)]
        self.bwd_end = [[0] * num_microbatches for _ in range(num_stages)]

    def simulate(self):
        """
        Simulate the 1F1B schedule.

        A practical approach:
        1. First, schedule all forwards just like GPipe (fill fwd_start/fwd_end).
        2. Then schedule backwards with ordering constraints:
           - Stage s processes backwards for microbatches in order 0, 1, ..., M-1
           - bwd_start[s][m] = max(
               fwd_end[s][m],           # need activations from own forward
               bwd_end[s+1][m],         # need gradients from downstream (if s < S-1)
               bwd_end[s][m-1],         # previous backward at same stage (if m > 0)
               fwd_end[s][warmup_m]     # in 1F1B, backward is interleaved with forward
             )

        The key difference from GPipe: backward for microbatch m at stage s
        starts as soon as possible (doesn't wait for all forwards to finish).

        For stage s, the number of warmup forwards = S - 1 - s
        After warmup_count forwards, stage s alternates: backward, forward, backward, forward...
        This means bwd for mb 0 at stage s starts after fwd for mb (warmup_count - 1) ends.

        Simplified scheduling (recommended):
          For each stage s, build a sequence of operations:
            ops = [F]*warmup_count + [B, F]*num_steady + [B]*cooldown_count
          where warmup_count = S - 1 - s (clamped to M)
                num_steady = min(M - warmup_count, M)  (pairs of B+F)
                cooldown_count adjusts to ensure all M forwards and M backwards happen

          Then simulate this operation sequence per stage, respecting dependencies.
        """
        # TODO: Implement 1F1B scheduling
        pass

    def get_stats(self):
        """
        Same interface as GPipeScheduler.get_stats().

        For peak_memory: count activations held per stage.
          Forward of microbatch m at stage s: +1 activation
          Backward of microbatch m at stage s: -1 activation
          Peak should be ~S (bounded by pipeline depth), much less than GPipe's M.
        """
        # TODO
        return {
            "total_time": 0,
            "bubble_ratio": 0.0,
            "peak_memory": 0,
            "per_stage_peak_memory": [0] * self.S,
        }


def compute_peak_memory(fwd_end, bwd_end, num_stages, num_microbatches):
    """
    Helper: compute per-stage peak memory from forward/backward end times.

    At any time t, stage s holds activations for all microbatches m where
    fwd_end[s][m] <= t and bwd_end[s][m] > t (or backward hasn't started).

    An efficient approach: for each stage, create a list of events:
      (fwd_end[s][m], +1) and (bwd_end[s][m], -1) for each m.
    Sort by time, sweep to find peak.

    Args:
        fwd_end: 2D array [stage][microbatch] of forward completion times
        bwd_end: 2D array [stage][microbatch] of backward completion times
        num_stages: S
        num_microbatches: M

    Returns:
        tuple: (global_peak, list of per-stage peaks)
    """
    # TODO: Sweep events to find peak memory per stage
    pass


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
    print("=== Q22-EXT: Advanced Pipeline Parallelism ===\n")

    def _tests():
        global passed, total

        total += 1
        def t1():
            gs = GPipeScheduler(3, 5, [10, 10, 10], [20, 20, 20])
            gs.simulate()
            stats = gs.get_stats()
            expected_total = (3 + 5 - 1) * 10 + (3 + 5 - 1) * 20
            ok = stats["total_time"] == expected_total
            print(f"{'PASS' if ok else 'FAIL'} [GPipe total time: {stats['total_time']}(exp {expected_total})]")
            return ok
        if _run_test("GPipe total time", t1): passed += 1

        total += 1
        def t2():
            gs = GPipeScheduler(4, 8, [10, 10, 10, 10], [20, 20, 20, 20])
            gs.simulate()
            stats = gs.get_stats()
            ok = stats["peak_memory"] == 8
            print(f"{'PASS' if ok else 'FAIL'} [GPipe peak memory: {stats['peak_memory']}(exp 8=M)]")
            return ok
        if _run_test("GPipe peak memory", t2): passed += 1

        total += 1
        def t3():
            gs = GPipeScheduler(3, 4, [10, 10, 10], [10, 10, 10])
            gs.simulate()
            stats = gs.get_stats()
            ok = 0.0 < stats["bubble_ratio"] < 1.0
            print(f"{'PASS' if ok else 'FAIL'} [GPipe bubble ratio: {stats['bubble_ratio']:.3f} (0 < x < 1)]")
            return ok
        if _run_test("GPipe bubble", t3): passed += 1

        total += 1
        def t4():
            ofb = OneF1BScheduler(3, 6, [10, 10, 10], [20, 20, 20])
            ofb.simulate()
            stats = ofb.get_stats()
            ok = stats["total_time"] > 0
            all_fwd = all(ofb.fwd_end[s][m] > 0 for s in range(3) for m in range(6))
            all_bwd = all(ofb.bwd_end[s][m] > 0 for s in range(3) for m in range(6))
            ok = ok and all_fwd and all_bwd
            print(f"{'PASS' if ok else 'FAIL'} [1F1B: all ops scheduled, total_time={stats['total_time']}]")
            return ok
        if _run_test("1F1B correctness", t4): passed += 1

        total += 1
        def t5():
            gs = GPipeScheduler(4, 8, [10, 10, 10, 10], [20, 20, 20, 20])
            gs.simulate()
            gs_stats = gs.get_stats()
            ofb = OneF1BScheduler(4, 8, [10, 10, 10, 10], [20, 20, 20, 20])
            ofb.simulate()
            ofb_stats = ofb.get_stats()
            ok = ofb_stats["peak_memory"] < gs_stats["peak_memory"]
            print(f"{'PASS' if ok else 'FAIL'} [1F1B peak mem({ofb_stats['peak_memory']}) "
                  f"< GPipe({gs_stats['peak_memory']})]")
            return ok
        if _run_test("1F1B vs GPipe memory", t5): passed += 1

        total += 1
        def t6():
            ofb = OneF1BScheduler(3, 5, [10, 15, 10], [20, 25, 20])
            ofb.simulate()
            ok = True
            for s in range(3):
                for m in range(5):
                    if m > 0 and ofb.fwd_start[s][m] < ofb.fwd_end[s][m - 1]: ok = False
                    if s > 0 and ofb.fwd_start[s][m] < ofb.fwd_end[s - 1][m]: ok = False
                    if ofb.bwd_start[s][m] < ofb.fwd_end[s][m]: ok = False
                    if s < 2 and ofb.bwd_start[s][m] < ofb.bwd_end[s + 1][m]: ok = False
                    if m > 0 and ofb.bwd_start[s][m] < ofb.bwd_end[s][m - 1]: ok = False
            print(f"{'PASS' if ok else 'FAIL'} [1F1B all dependency constraints satisfied]")
            return ok
        if _run_test("1F1B deps", t6): passed += 1

        total += 1
        def t7():
            ofb = OneF1BScheduler(3, 5, [10, 15, 10], [20, 25, 20])
            ofb.simulate()
            ok = True
            for s in range(3):
                intervals = []
                for m in range(5):
                    intervals.append((ofb.fwd_start[s][m], ofb.fwd_end[s][m]))
                    intervals.append((ofb.bwd_start[s][m], ofb.bwd_end[s][m]))
                intervals.sort()
                for i in range(1, len(intervals)):
                    if intervals[i][0] < intervals[i - 1][1]:
                        ok = False
                        break
            print(f"{'PASS' if ok else 'FAIL'} [1F1B no overlapping ops within a stage]")
            return ok
        if _run_test("1F1B no overlap", t7): passed += 1

        total += 1
        def t8():
            fwd_end = [[10, 20, 30], [20, 30, 40]]
            bwd_end = [[80, 90, 100], [60, 70, 80]]
            peak, per_stage = compute_peak_memory(fwd_end, bwd_end, 2, 3)
            ok = per_stage[0] == 3
            print(f"{'PASS' if ok else 'FAIL'} [Peak memory helper: stage0 peak={per_stage[0]}(exp 3)]")
            return ok
        if _run_test("Peak memory helper", t8): passed += 1

        total += 1
        def t9():
            gs = GPipeScheduler(3, 4, [5, 10, 5], [10, 20, 10])
            gs.simulate()
            stats = gs.get_stats()
            expected = (5 + 10 + 5) + (4 - 1) * 10 + (10 + 20 + 10) + (4 - 1) * 20
            ok = stats["total_time"] == expected
            print(f"{'PASS' if ok else 'FAIL'} [GPipe unequal: total={stats['total_time']}(exp {expected})]")
            return ok
        if _run_test("GPipe unequal", t9): passed += 1

        total += 1
        def t10():
            configs = [
                (4, 8, [10, 10, 10, 10], [20, 20, 20, 20]),
                (3, 6, [5, 15, 10], [10, 30, 20]),
                (2, 10, [10, 10], [10, 10]),
            ]
            ok = True
            for S, M, fwd, bwd in configs:
                gs = GPipeScheduler(S, M, fwd, bwd)
                gs.simulate()
                ofb = OneF1BScheduler(S, M, fwd, bwd)
                ofb.simulate()
                if ofb.get_stats()["total_time"] > gs.get_stats()["total_time"]:
                    ok = False
                    break
            print(f"{'PASS' if ok else 'FAIL'} [1F1B total_time <= GPipe for all configs]")
            return ok
        if _run_test("1F1B vs GPipe time", t10): passed += 1

    _tests()
    print(f"\nPassed {passed}/{total} tests")
