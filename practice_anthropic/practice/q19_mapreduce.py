"""
Q19 — MapReduce Framework [Medium-Hard]

Topic: Parallel Programming — MapReduce

Problem:
  Implement a simplified single-machine MapReduce framework that can
  process data through three phases: Map, Shuffle, and Reduce.

  Implement:
  1. map_phase(input_data, mapper_fn) — applies mapper to each input,
     produces a flat list of (key, value) pairs
  2. shuffle_phase(mapped_data) — groups all values by key
  3. reduce_phase(shuffled_data, reducer_fn) — applies reducer to each
     key's value list, producing a single (key, result) per key
  4. mapreduce(input_data, mapper_fn, reducer_fn) — orchestrates all phases

  Then use the framework to implement:
  - Word count: count occurrences of each word in a list of strings
  - Inverted index: for each word, list which document IDs contain it
  - Mean computation: compute average of values grouped by key

Constraints:
  - Mapper takes one input and returns a list of (key, value) tuples
  - Reducer takes a key and a list of values and returns a single value
  - Shuffle must be stable (preserve insertion order of values per key)
  - Handle empty inputs gracefully

Key Concepts:
  - MapReduce paradigm (Google, 2004)
  - Functional decomposition of data processing
  - Key-value aggregation patterns
  - Foundation for distributed computing frameworks (Hadoop, Spark)
"""

from collections import OrderedDict


# ============ YOUR IMPLEMENTATION ============

def map_phase(input_data, mapper_fn):
    """
    Apply mapper_fn to each element of input_data.
    mapper_fn takes a single string and returns a list of (key, value) tuples.
    Return the flattened list of all (key, value) tuples produced.
    """
    ret = []
    # TODO: For each item in input_data:
    for item in input_data:
        #   1. Call mapper_fn(item)
        #   2. Extend the result list with all returned tuples
        ret.extend(mapper_fn(item))
    return ret


def shuffle_phase(mapped_data):
    """
    Group all values by key, preserving insertion order.
    Returns an OrderedDict from key -> list of values.
    """
    ord_dict = OrderedDict()
    # TODO: For each (key, value) in mapped_data:
    for key, val in mapped_data:
        #   Group values under their key, preserving order
        if key not in ord_dict:
            ord_dict[key] = []
        ord_dict[key].append(val)

    return ord_dict


def reduce_phase(shuffled_data, reducer_fn):
    """
    Apply reducer_fn to each key's list of values.
    reducer_fn takes (key, values) and returns a single result.
    Returns a dict from key -> reduced result.
    """
    result = {}

    # TODO: For each (key, values) in shuffled_data:
    for key, vals in shuffled_data.items():
        result[key] = reducer_fn(key, vals)

    return result


def mapreduce(input_data, mapper_fn, reducer_fn):
    """Orchestrate all three phases: map -> shuffle -> reduce."""
    # TODO: Chain map_phase -> shuffle_phase -> reduce_phase
    mapped_data = map_phase(input_data, mapper_fn)
    shuffled_data = shuffle_phase(mapped_data)
    return reduce_phase(shuffled_data, reducer_fn)


# --- Application-specific mappers and reducers ---

def word_count_mapper(text):
    """Split text into words, emit (word, 1) for each."""
    # TODO: return [(word, 1) for word in text.split()]
    return [(word, 1) for word in text.split()]


def word_count_reducer(key, values):
    """Sum up all the 1s for a given word."""
    # TODO: return sum(values)
    return sum(values)


def inverted_index_mapper(doc_input):
    """
    Input is "doc_id:text". Emit (word, doc_id) for each unique word.
    """
    # TODO: Parse "doc_id:text", split text into words
    #   For each unique word, emit (word, doc_id)
    # partition will result in [doc_id, ":", text]
    doc_id, _, text = doc_input.partition(":")
    return [(word, doc_id) for word in text.split()]


def inverted_index_reducer(key, values):
    """Return list of doc IDs (as-is)."""
    # TODO: return values (already a list of doc_ids)
    return values


def mean_mapper(entry):
    """Input is "group:value". Emit (group, float(value))."""
    # TODO: Parse "group:value" and emit [(group, float(value))]
    group, _, value = entry.partition(":")
    return [(group, float(value))]


def mean_reducer(key, values):
    """Compute arithmetic mean of all values."""
    # TODO: return sum(values) / len(values)
    return sum(values) / len(values)


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q19: MapReduce Framework ===\n")
    passed = 0
    total = 0

    # Test 1: Word count on sample text
    total += 1
    docs = [
        "the cat sat on the mat",
        "the dog sat on the log",
        "the cat and the dog",
    ]
    result = mapreduce(docs, word_count_mapper, word_count_reducer)
    ok = (result.get("the") == 6 and result.get("cat") == 2 and
          result.get("sat") == 2 and result.get("dog") == 2 and
          result.get("on") == 2 and result.get("mat") == 1 and
          result.get("log") == 1 and result.get("and") == 1)
    print(f"{'PASS' if ok else 'FAIL'} [Word count: the={result.get('the')}(exp 6), "
          f"cat={result.get('cat')}(exp 2), mat={result.get('mat')}(exp 1)]")
    if ok:
        passed += 1

    # Test 2: Inverted index
    total += 1
    docs = ["0:hello world", "1:hello there", "2:world peace"]
    result = mapreduce(docs, inverted_index_mapper, inverted_index_reducer)
    ok = (sorted(result.get("hello", [])) == ["0", "1"] and
          sorted(result.get("world", [])) == ["0", "2"] and
          result.get("there") == ["1"] and
          result.get("peace") == ["2"])
    print(f"{'PASS' if ok else 'FAIL'} [Inverted index: hello={result.get('hello')}(exp ['0','1']), "
          f"world={result.get('world')}(exp ['0','2'])]")
    if ok:
        passed += 1

    # Test 3: Mean computation
    total += 1
    data = ["A:10", "A:20", "A:30", "B:5", "B:15", "C:100"]
    result = mapreduce(data, mean_mapper, mean_reducer)
    ok = (abs(result.get("A", 0) - 20.0) < 0.01 and
          abs(result.get("B", 0) - 10.0) < 0.01 and
          abs(result.get("C", 0) - 100.0) < 0.01)
    print(f"{'PASS' if ok else 'FAIL'} [Mean: A={result.get('A')}(exp 20.0), "
          f"B={result.get('B')}(exp 10.0), C={result.get('C')}(exp 100.0)]")
    if ok:
        passed += 1

    # Test 4: Empty input handling
    total += 1
    result = mapreduce([], word_count_mapper, word_count_reducer)
    ok = len(result) == 0
    print(f"{'PASS' if ok else 'FAIL'} [Empty input returns empty result (size={len(result)})]")
    if ok:
        passed += 1

    # Test 5: Large-ish input (100+ items)
    total += 1
    data = ["apple banana"] * 120 + ["cherry"]
    result = mapreduce(data, word_count_mapper, word_count_reducer)
    ok = (result.get("apple") == 120 and result.get("banana") == 120 and
          result.get("cherry") == 1)
    print(f"{'PASS' if ok else 'FAIL'} [Large input (121 items): "
          f"apple={result.get('apple')}(exp 120), cherry={result.get('cherry')}(exp 1)]")
    if ok:
        passed += 1

    # Test 6: Custom mapper/reducer — character frequency
    total += 1
    char_mapper = lambda s: [(c, 1) for c in s if c != " "]
    sum_reducer = lambda key, vals: sum(vals)
    data = ["aab", "bcc"]
    result = mapreduce(data, char_mapper, sum_reducer)
    ok = (result.get("a") == 2 and result.get("b") == 2 and result.get("c") == 2)
    print(f"{'PASS' if ok else 'FAIL'} [Custom char-frequency: "
          f"a={result.get('a')}(exp 2), b={result.get('b')}(exp 2), c={result.get('c')}(exp 2)]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
