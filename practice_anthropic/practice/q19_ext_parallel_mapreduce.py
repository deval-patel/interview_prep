"""
Q19-EXT — Parallel MapReduce with Partitioning & Fault Tolerance [Hard]

Topic: Distributed Systems — Parallel MapReduce

Problem:
  Extend the basic MapReduce framework to handle real distributed-systems
  concerns: parallel execution, hash-based partitioning, shuffle cost
  tracking, and straggler mitigation.

  Implement:
  1. ParallelMapReduce(num_mappers, num_reducers) — configurable parallelism
  2. Hash partitioner — deterministic key-to-reducer assignment
  3. Shuffle phase that tracks bytes transferred between mappers and reducers
  4. Parallel map execution using concurrent.futures
  5. Straggler detection and speculative re-execution
  6. Combiner (local pre-aggregation) to reduce shuffle traffic

  The system should:
  - Split input data into chunks, one per mapper
  - Run mappers in parallel (use ThreadPoolExecutor for simulation)
  - Partition mapper output by hash(key) % num_reducers
  - Track shuffle cost: number of (key, value) pairs sent between partitions
  - Run reducers in parallel
  - Support an optional combiner that pre-aggregates mapper output locally
  - Detect stragglers: if a mapper takes >2x the median mapper time, re-run it

Constraints:
  - Partitioning must be deterministic (same key always goes to same reducer)
  - Combiner must be associative and commutative (like the reducer)
  - Track: total shuffle pairs, per-reducer load, mapper durations
  - Handle empty partitions gracefully

Key Concepts:
  - Hash partitioning (foundation of distributed shuffle)
  - Combiners reduce network traffic (like mini-reducers on map side)
  - Straggler mitigation via speculative execution
  - Work distribution and load balancing across reducers
"""

import time
import hashlib
from collections import OrderedDict
from concurrent.futures import ThreadPoolExecutor, as_completed


# ============ YOUR IMPLEMENTATION ============

class ShuffleStats:
    """Tracks shuffle phase metrics."""
    def __init__(self):
        self.total_pairs_shuffled = 0       # total (key, val) pairs crossing partitions
        self.per_reducer_pairs = {}         # reducer_id -> number of pairs received
        self.mapper_durations = []          # wall-clock time per mapper (seconds)
        self.stragglers_detected = 0        # number of mappers flagged as stragglers
        self.speculative_reruns = 0         # number of speculative re-executions


def hash_partition(key, num_reducers):
    """
    Deterministically assign a key to a reducer partition.
    Use MD5 hash of the string representation of the key, mod num_reducers.

    Args:
        key: any hashable key
        num_reducers: number of reducer partitions

    Returns:
        int: reducer partition index in [0, num_reducers)
    """
    # TODO: Hash the key deterministically and return partition index
    # Use hashlib.md5 on the str(key) encoded to bytes, convert digest to int
    pass


def split_input(input_data, num_mappers):
    """
    Split input_data into num_mappers roughly equal chunks.
    If len(input_data) is not evenly divisible, earlier chunks get one extra item.

    Args:
        input_data: list of input items
        num_mappers: number of mapper workers

    Returns:
        list of lists (one chunk per mapper)
    """
    # TODO: Split input_data into num_mappers chunks
    # Handle case where len(input_data) < num_mappers (some chunks will be empty)
    pass


def run_mapper(mapper_fn, chunk, mapper_id, slow_mappers=None):
    """
    Run a mapper function on a chunk of data. Simulates work with optional
    artificial slowdown for straggler testing.

    Args:
        mapper_fn: function that takes one input item and returns list of (key, value)
        chunk: list of input items for this mapper
        mapper_id: integer ID of this mapper
        slow_mappers: set of mapper_ids that should be artificially slowed (sleep 0.1s per item)

    Returns:
        tuple: (mapper_id, results_list, duration_seconds)
        where results_list is the flattened list of all (key, value) pairs
    """
    # TODO:
    # 1. Record start time
    # 2. For each item in chunk, call mapper_fn and collect results
    # 3. If mapper_id is in slow_mappers, add artificial delay
    # 4. Record end time
    # 5. Return (mapper_id, all_results, duration)
    pass


def apply_combiner(mapper_output, combiner_fn):
    """
    Apply a combiner (local pre-aggregation) to mapper output.
    Groups values by key and applies combiner_fn to each group,
    producing a reduced set of (key, value) pairs.

    This reduces shuffle traffic by aggregating locally before sending
    data to reducers.

    Args:
        mapper_output: list of (key, value) pairs from one mapper
        combiner_fn: function(key, list_of_values) -> single_value

    Returns:
        list of (key, combined_value) pairs
    """
    # TODO:
    # 1. Group mapper_output by key (preserve order)
    # 2. Apply combiner_fn to each group
    # 3. Return list of (key, combined_value)
    pass


def shuffle_and_partition(all_mapper_outputs, num_reducers):
    """
    Partition all mapper outputs by key hash into reducer buckets.
    Track shuffle statistics.

    Args:
        all_mapper_outputs: list of (mapper_id, list of (key, value) pairs)
        num_reducers: number of reducer partitions

    Returns:
        tuple: (reducer_buckets, stats)
        - reducer_buckets: dict of reducer_id -> OrderedDict of key -> [values]
        - stats partial: ShuffleStats with total_pairs_shuffled and per_reducer_pairs filled
    """
    # TODO:
    # 1. For each mapper's output, for each (key, value):
    #    a. Determine target reducer via hash_partition(key, num_reducers)
    #    b. Add value to the appropriate reducer bucket under that key
    #    c. Increment shuffle counters
    # 2. Return the reducer buckets and stats
    pass


def run_reducer(reducer_fn, reducer_id, key_values):
    """
    Run reducer on all key-value groups assigned to this reducer.

    Args:
        reducer_fn: function(key, list_of_values) -> result
        reducer_id: integer ID of this reducer
        key_values: OrderedDict of key -> [values]

    Returns:
        tuple: (reducer_id, dict of key -> result)
    """
    # TODO: Apply reducer_fn to each key's value list
    pass


class ParallelMapReduce:
    """
    Full parallel MapReduce framework with partitioning, combiners,
    and straggler detection.
    """

    def __init__(self, num_mappers=2, num_reducers=2):
        self.num_mappers = num_mappers
        self.num_reducers = num_reducers
        self.stats = ShuffleStats()

    def execute(self, input_data, mapper_fn, reducer_fn,
                combiner_fn=None, slow_mappers=None):
        """
        Run the full MapReduce pipeline:
          1. Split input across mappers
          2. Run mappers in parallel (ThreadPoolExecutor)
          3. Detect stragglers (duration > 2x median)
          4. Optionally apply combiner to each mapper's output
          5. Shuffle & partition by key hash
          6. Run reducers in parallel
          7. Merge reducer outputs

        Args:
            input_data: list of input items
            mapper_fn: function(item) -> list of (key, value)
            reducer_fn: function(key, [values]) -> result
            combiner_fn: optional function(key, [values]) -> value (pre-aggregation)
            slow_mappers: set of mapper_ids to artificially slow down (for testing)

        Returns:
            dict of key -> reduced result
        """
        self.stats = ShuffleStats()

        # TODO:
        # Step 1: Split input
        # Step 2: Run mappers in parallel using ThreadPoolExecutor
        # Step 3: Collect results and mapper durations
        # Step 4: Detect stragglers (duration > 2x median of all mapper durations)
        #         Record in stats.stragglers_detected
        # Step 5: Apply combiner if provided
        # Step 6: Shuffle and partition
        # Step 7: Run reducers in parallel using ThreadPoolExecutor
        # Step 8: Merge all reducer results into one dict
        # Step 9: Return merged results

        return {}


# --- Application: Parallel Word Count with Combiner ---

def word_count_mapper(text):
    """Emit (word, 1) for each word."""
    return [(word, 1) for word in text.split()]


def word_count_reducer(key, values):
    """Sum counts."""
    return sum(values)


def word_count_combiner(key, values):
    """Local sum (same as reducer — addition is associative)."""
    return sum(values)


# --- Application: Top-K Words ---

def topk_reducer(key, values):
    """Sum counts (used with a post-processing top-k step)."""
    return sum(values)


def get_topk(results, k):
    """
    Given a dict of word -> count, return the top-k words by count.
    Ties broken alphabetically.

    Returns: list of (word, count) sorted by count descending, then word ascending.
    """
    # TODO: Sort results and return top k
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
    print("=== Q19-EXT: Parallel MapReduce ===\n")

    def _tests():
        global passed, total

        # Test 1: Hash partitioning is deterministic
        total += 1
        def t1():
            p1 = hash_partition("hello", 4)
            p2 = hash_partition("hello", 4)
            p3 = hash_partition("world", 4)
            ok = (p1 == p2 and 0 <= p1 < 4 and 0 <= p3 < 4)
            print(f"{'PASS' if ok else 'FAIL'} [Hash partition deterministic: "
                  f"hello->{p1} (twice), world->{p3}]")
            return ok
        if _run_test("Hash partition", t1): passed += 1

        # Test 2: Input splitting
        total += 1
        def t2():
            chunks = split_input(list(range(10)), 3)
            ok = (len(chunks) == 3 and
                  sum(len(c) for c in chunks) == 10 and
                  all(abs(len(chunks[i]) - len(chunks[j])) <= 1
                      for i in range(3) for j in range(3)))
            print(f"{'PASS' if ok else 'FAIL'} [Split 10 items into 3 chunks: "
                  f"sizes={[len(c) for c in chunks]}]")
            return ok
        if _run_test("Input splitting", t2): passed += 1

        # Test 3: Split with more mappers than data
        total += 1
        def t3():
            chunks = split_input([1, 2], 5)
            ok = (len(chunks) == 5 and sum(len(c) for c in chunks) == 2)
            print(f"{'PASS' if ok else 'FAIL'} [Split 2 items into 5 chunks: "
                  f"sizes={[len(c) for c in chunks]}]")
            return ok
        if _run_test("Split overflow", t3): passed += 1

        # Test 4: Basic parallel word count
        total += 1
        def t4():
            pmr = ParallelMapReduce(num_mappers=2, num_reducers=2)
            docs = ["the cat sat on the mat", "the dog sat on the log", "the cat and the dog"]
            result = pmr.execute(docs, word_count_mapper, word_count_reducer)
            ok = (result.get("the") == 6 and result.get("cat") == 2 and
                  result.get("dog") == 2 and result.get("mat") == 1)
            print(f"{'PASS' if ok else 'FAIL'} [Parallel word count: the={result.get('the')}(exp 6), "
                  f"cat={result.get('cat')}(exp 2)]")
            return ok
        if _run_test("Parallel word count", t4): passed += 1

        # Test 5: Combiner reduces shuffle traffic
        total += 1
        def t5():
            pmr_no = ParallelMapReduce(num_mappers=2, num_reducers=2)
            pmr_yes = ParallelMapReduce(num_mappers=2, num_reducers=2)
            big_docs = ["apple banana apple banana cherry"] * 50
            res_no = pmr_no.execute(big_docs, word_count_mapper, word_count_reducer)
            res_yes = pmr_yes.execute(big_docs, word_count_mapper, word_count_reducer,
                                       combiner_fn=word_count_combiner)
            ok = (res_no == res_yes and
                  pmr_yes.stats.total_pairs_shuffled < pmr_no.stats.total_pairs_shuffled)
            print(f"{'PASS' if ok else 'FAIL'} [Combiner reduces shuffle: "
                  f"without={pmr_no.stats.total_pairs_shuffled}, "
                  f"with={pmr_yes.stats.total_pairs_shuffled}]")
            return ok
        if _run_test("Combiner", t5): passed += 1

        # Test 6: Partition correctness
        total += 1
        def t6():
            pmr = ParallelMapReduce(num_mappers=1, num_reducers=3)
            result = pmr.execute(["a a a b b c"], word_count_mapper, word_count_reducer)
            ok = (result.get("a") == 3 and result.get("b") == 2 and result.get("c") == 1)
            print(f"{'PASS' if ok else 'FAIL'} [Partition correctness: "
                  f"a={result.get('a')}(exp 3), b={result.get('b')}(exp 2)]")
            return ok
        if _run_test("Partition", t6): passed += 1

        # Test 7: Straggler detection
        total += 1
        def t7():
            pmr = ParallelMapReduce(num_mappers=4, num_reducers=2)
            docs = ["word " * 10] * 20
            result = pmr.execute(docs, word_count_mapper, word_count_reducer, slow_mappers={2})
            ok = (pmr.stats.stragglers_detected >= 1 and result.get("word") == 200)
            print(f"{'PASS' if ok else 'FAIL'} [Straggler detection: "
                  f"detected={pmr.stats.stragglers_detected}, word={result.get('word')}(exp 200)]")
            return ok
        if _run_test("Straggler", t7): passed += 1

        # Test 8: Empty input
        total += 1
        def t8():
            pmr = ParallelMapReduce(num_mappers=2, num_reducers=2)
            result = pmr.execute([], word_count_mapper, word_count_reducer)
            ok = len(result) == 0
            print(f"{'PASS' if ok else 'FAIL'} [Empty input: size={len(result)}]")
            return ok
        if _run_test("Empty input", t8): passed += 1

        # Test 9: Per-reducer load tracking
        total += 1
        def t9():
            pmr = ParallelMapReduce(num_mappers=2, num_reducers=4)
            docs = ["alpha beta gamma delta"] * 10
            result = pmr.execute(docs, word_count_mapper, word_count_reducer)
            total_tracked = sum(pmr.stats.per_reducer_pairs.values())
            ok = (total_tracked == pmr.stats.total_pairs_shuffled and
                  len(pmr.stats.per_reducer_pairs) <= 4 and
                  result.get("alpha") == 10)
            print(f"{'PASS' if ok else 'FAIL'} [Per-reducer load: "
                  f"pairs={pmr.stats.per_reducer_pairs}, total={total_tracked}]")
            return ok
        if _run_test("Reducer load", t9): passed += 1

        # Test 10: Top-K post-processing
        total += 1
        def t10():
            pmr = ParallelMapReduce(num_mappers=2, num_reducers=2)
            docs = ["the the the cat cat dog bird bird bird bird"]
            result = pmr.execute(docs, word_count_mapper, topk_reducer)
            top2 = get_topk(result, 2)
            ok = (len(top2) == 2 and top2[0] == ("bird", 4) and top2[1] == ("the", 3))
            print(f"{'PASS' if ok else 'FAIL'} [Top-K: top2={top2}, "
                  f"expected=[('bird',4), ('the',3)]]")
            return ok
        if _run_test("Top-K", t10): passed += 1

    _tests()
    print(f"\nPassed {passed}/{total} tests")
