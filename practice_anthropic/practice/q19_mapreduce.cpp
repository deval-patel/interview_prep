/*
 * Q19 — MapReduce Framework [Medium-Hard]
 *
 * Topic: Parallel Programming — MapReduce
 *
 * Problem:
 *   Implement a simplified single-machine MapReduce framework that can
 *   process data through three phases: Map, Shuffle, and Reduce.
 *
 *   Implement:
 *   1. map_phase(input_data, mapper_fn) — applies mapper to each input,
 *      produces a flat list of (key, value) pairs
 *   2. shuffle_phase(mapped_data) — groups all values by key
 *   3. reduce_phase(shuffled_data, reducer_fn) — applies reducer to each
 *      key's value list, producing a single (key, result) per key
 *   4. mapreduce(input_data, mapper_fn, reducer_fn) — orchestrates all phases
 *
 *   Then use the framework to implement:
 *   - Word count: count occurrences of each word in a list of strings
 *   - Inverted index: for each word, list which document IDs contain it
 *   - Mean computation: compute average of values grouped by key
 *
 * Constraints:
 *   - Mapper takes one input and returns a vector of (key, value) pairs
 *   - Reducer takes a key and a vector of values and returns a single value
 *   - Shuffle must be stable (preserve insertion order of values per key)
 *   - Handle empty inputs gracefully
 *
 * Key Concepts:
 *   - MapReduce paradigm (Google, 2004)
 *   - Functional decomposition of data processing
 *   - Key-value aggregation patterns
 *   - Foundation for distributed computing frameworks (Hadoop, Spark)
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

// A key-value pair used throughout the MapReduce pipeline
struct KVPair {
    string key;
    string value;
};

// ============ YOUR IMPLEMENTATION ============

/*
 * map_phase: Apply mapper_fn to each element of input_data.
 * mapper_fn takes a single string input and returns a vector of KVPairs.
 * Return the flattened list of all KVPairs produced.
 */
vector<KVPair> map_phase(const vector<string>& input_data,
                         function<vector<KVPair>(const string&)> mapper_fn) {
    vector<KVPair> result;

    // TODO: For each input in input_data:
    //   1. Call mapper_fn(input)
    //   2. Append all returned KVPairs to result

    return result;
}

/*
 * shuffle_phase: Group all values by key.
 * Returns a map from key -> list of values (in the order they appeared).
 */
map<string, vector<string>> shuffle_phase(const vector<KVPair>& mapped_data) {
    map<string, vector<string>> grouped;

    // TODO: For each KVPair in mapped_data:
    //   Group values under their key, preserving order

    return grouped;
}

/*
 * reduce_phase: Apply reducer_fn to each key's list of values.
 * reducer_fn takes (key, values) and returns a single string result.
 * Returns a map from key -> reduced result.
 */
map<string, string> reduce_phase(const map<string, vector<string>>& shuffled_data,
                                 function<string(const string&, const vector<string>&)> reducer_fn) {
    map<string, string> result;

    // TODO: For each (key, values) in shuffled_data:
    //   result[key] = reducer_fn(key, values)

    return result;
}

/*
 * mapreduce: Orchestrate all three phases.
 */
map<string, string> mapreduce(const vector<string>& input_data,
                              function<vector<KVPair>(const string&)> mapper_fn,
                              function<string(const string&, const vector<string>&)> reducer_fn) {
    // TODO: Chain map_phase -> shuffle_phase -> reduce_phase
    map<string, string> result;
    return result;
}

// --- Application-specific mappers and reducers ---

/*
 * word_count_mapper: Split input string into words, emit (word, "1") for each.
 */
vector<KVPair> word_count_mapper(const string& input) {
    vector<KVPair> pairs;

    // TODO: Split input by whitespace
    //   For each word, emit KVPair{word, "1"}

    return pairs;
}

/*
 * word_count_reducer: Sum up all the "1"s for a given word.
 * Return the count as a string.
 */
string word_count_reducer(const string& key, const vector<string>& values) {
    // TODO: Return to_string(values.size()) or sum of integer values
    return "0";
}

/*
 * inverted_index_mapper: Input is "doc_id:text". Emit (word, doc_id) for
 * each unique word in the text.
 */
vector<KVPair> inverted_index_mapper(const string& input) {
    vector<KVPair> pairs;

    // TODO: Parse "doc_id:text" format
    //   Split text into words
    //   For each unique word, emit KVPair{word, doc_id}

    return pairs;
}

/*
 * inverted_index_reducer: Concatenate doc IDs into a comma-separated list.
 */
string inverted_index_reducer(const string& key, const vector<string>& values) {
    // TODO: Join values with ","
    return "";
}

/*
 * mean_mapper: Input is "group:value". Emit (group, value).
 */
vector<KVPair> mean_mapper(const string& input) {
    vector<KVPair> pairs;

    // TODO: Parse "group:value" and emit KVPair{group, value}

    return pairs;
}

/*
 * mean_reducer: Compute the arithmetic mean of all values for a key.
 * Return the mean as a string with 2 decimal places.
 */
string mean_reducer(const string& key, const vector<string>& values) {
    // TODO: Parse each value as a double, compute mean
    //   Return formatted string (e.g., "3.50")
    return "0.00";
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q19: MapReduce Framework ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Word count on sample text
    {
        total++;
        vector<string> docs = {
            "the cat sat on the mat",
            "the dog sat on the log",
            "the cat and the dog"
        };
        auto result = mapreduce(docs, word_count_mapper, word_count_reducer);

        bool ok = (result["the"] == "5") &&
                  (result["cat"] == "2") &&
                  (result["sat"] == "2") &&
                  (result["dog"] == "2") &&
                  (result["on"] == "2") &&
                  (result["mat"] == "1") &&
                  (result["log"] == "1") &&
                  (result["and"] == "1");

        printf("%s [Word count: 'the'=%s(exp 5), 'cat'=%s(exp 2), 'mat'=%s(exp 1)]\n",
               ok ? "PASS" : "FAIL",
               result["the"].c_str(), result["cat"].c_str(), result["mat"].c_str());
        if (ok) passed++;
    }

    // Test 2: Inverted index
    {
        total++;
        vector<string> docs = {
            "0:hello world",
            "1:hello there",
            "2:world peace"
        };
        auto result = mapreduce(docs, inverted_index_mapper, inverted_index_reducer);

        bool ok = (result["hello"] == "0,1") &&
                  (result["world"] == "0,2") &&
                  (result["there"] == "1") &&
                  (result["peace"] == "2");

        printf("%s [Inverted index: 'hello'=%s(exp 0,1), 'world'=%s(exp 0,2)]\n",
               ok ? "PASS" : "FAIL",
               result["hello"].c_str(), result["world"].c_str());
        if (ok) passed++;
    }

    // Test 3: Mean computation
    {
        total++;
        vector<string> data = {
            "A:10", "A:20", "A:30",
            "B:5", "B:15",
            "C:100"
        };
        auto result = mapreduce(data, mean_mapper, mean_reducer);

        bool ok = (result["A"] == "20.00") &&
                  (result["B"] == "10.00") &&
                  (result["C"] == "100.00");

        printf("%s [Mean: A=%s(exp 20.00), B=%s(exp 10.00), C=%s(exp 100.00)]\n",
               ok ? "PASS" : "FAIL",
               result["A"].c_str(), result["B"].c_str(), result["C"].c_str());
        if (ok) passed++;
    }

    // Test 4: Empty input handling
    {
        total++;
        vector<string> empty_data;
        auto result = mapreduce(empty_data, word_count_mapper, word_count_reducer);
        bool ok = result.empty();

        printf("%s [Empty input returns empty result (size=%zu)]\n",
               ok ? "PASS" : "FAIL", result.size());
        if (ok) passed++;
    }

    // Test 5: Large-ish input (100+ items)
    {
        total++;
        vector<string> data;
        for (int i = 0; i < 120; i++) {
            data.push_back("apple banana");
        }
        data.push_back("cherry");

        auto result = mapreduce(data, word_count_mapper, word_count_reducer);

        bool ok = (result["apple"] == "120") &&
                  (result["banana"] == "120") &&
                  (result["cherry"] == "1");

        printf("%s [Large input (121 items): apple=%s(exp 120), cherry=%s(exp 1)]\n",
               ok ? "PASS" : "FAIL",
               result["apple"].c_str(), result["cherry"].c_str());
        if (ok) passed++;
    }

    // Test 6: Custom mapper/reducer — character frequency
    {
        total++;
        auto char_mapper = [](const string& input) -> vector<KVPair> {
            vector<KVPair> pairs;
            for (char c : input) {
                if (c != ' ') {
                    pairs.push_back({string(1, c), "1"});
                }
            }
            return pairs;
        };
        auto sum_reducer = [](const string& key, const vector<string>& values) -> string {
            return to_string(values.size());
        };

        vector<string> data = {"aab", "bcc"};
        auto result = mapreduce(data, char_mapper, sum_reducer);

        bool ok = (result["a"] == "2") &&
                  (result["b"] == "2") &&
                  (result["c"] == "2");

        printf("%s [Custom char-frequency: a=%s(exp 2), b=%s(exp 2), c=%s(exp 2)]\n",
               ok ? "PASS" : "FAIL",
               result["a"].c_str(), result["b"].c_str(), result["c"].c_str());
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
