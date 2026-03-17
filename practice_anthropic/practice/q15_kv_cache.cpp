/*
 * Q15 — KV Cache Manager [Medium]
 *
 * Topic: ML Context — Transformer Architecture / KV Cache
 *
 * Problem:
 *   Implement a KV (Key-Value) cache for autoregressive LLM inference.
 *
 *   During generation, each new token needs to attend to all previous tokens.
 *   Without a KV cache, we'd recompute K and V projections for all past tokens
 *   at each step. The KV cache stores previously computed K and V vectors.
 *
 *   Implement:
 *   1. KVCache class with pre-allocated buffer for max_seq_len tokens
 *   2. append(): Add new K, V for the current token
 *   3. get_keys()/get_values(): Return all cached K/V up to current position
 *   4. Memory analysis for realistic model configurations
 *
 * Constraints:
 *   - Support multi-layer, multi-head caching
 *   - Pre-allocate for max_seq_len to avoid runtime allocation
 *   - Track memory usage
 *
 * Key Concepts:
 *   - KV cache structure and lifecycle
 *   - Memory bottleneck in LLM inference
 *   - Why KV cache size dominates inference memory for long sequences
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cstring>

using namespace std;

// ============ YOUR IMPLEMENTATION ============

struct KVCache {
    int num_layers;
    int num_heads;
    int head_dim;
    int max_seq_len;
    int current_pos;  // number of tokens currently cached

    // Storage: [layer][head][position][head_dim]
    // keys[layer][head] is a flat array of max_seq_len * head_dim floats
    vector<vector<vector<float>>> keys;    // [layer][head][pos * head_dim ..]
    vector<vector<vector<float>>> values;

    /*
     * Constructor: Pre-allocate KV cache for all layers and heads.
     */
    KVCache(int num_layers, int num_heads, int head_dim, int max_seq_len)
        : num_layers(num_layers), num_heads(num_heads), head_dim(head_dim),
          max_seq_len(max_seq_len), current_pos(0) {
        // TODO: Allocate keys and values arrays
        // keys[l][h] should have space for max_seq_len * head_dim floats
    }

    /*
     * append: Add K, V vectors for a new token across all layers and heads.
     *
     * new_keys[layer][head] = vector of head_dim floats
     * new_values[layer][head] = vector of head_dim floats
     *
     * Copy into position current_pos, then increment current_pos.
     */
    bool append(const vector<vector<vector<float>>>& new_keys,    // [layer][head][head_dim]
                const vector<vector<vector<float>>>& new_values) {
        // TODO: Implement
        // Return false if cache is full (current_pos >= max_seq_len)
        return false;
    }

    /*
     * get_keys: Return all cached keys for a given layer and head,
     * up to current_pos.
     *
     * Returns a pointer to the key data and sets 'length' to current_pos.
     */
    const float* get_keys(int layer, int head, int& length) {
        // TODO: Return pointer to keys[layer][head] data, set length = current_pos
        length = 0;
        return nullptr;
    }

    /*
     * get_values: Same as get_keys but for values.
     */
    const float* get_values(int layer, int head, int& length) {
        // TODO: Return pointer to values[layer][head] data, set length = current_pos
        length = 0;
        return nullptr;
    }

    /*
     * memory_bytes: Return total memory used by the KV cache in bytes.
     * = 2 (K+V) * num_layers * num_heads * max_seq_len * head_dim * sizeof(float)
     */
    long long memory_bytes() {
        // TODO: Implement
        return 0;
    }

    /*
     * reset: Clear the cache (reset position to 0).
     */
    void reset() {
        current_pos = 0;
    }
};

/*
 * compute_kv_cache_size_gb: Compute KV cache size in GB for a model config.
 *
 * Common model sizes:
 *   - 7B model:  32 layers, 32 heads, head_dim=128
 *   - 13B model: 40 layers, 40 heads, head_dim=128
 *   - 70B model: 80 layers, 64 heads, head_dim=128
 *
 * For a given max_seq_len, how much memory does the KV cache need?
 * Uses float16 (2 bytes per value) for realistic estimation.
 */
double compute_kv_cache_size_gb(int num_layers, int num_heads, int head_dim,
                                 int max_seq_len, int bytes_per_value = 2) {
    // TODO: Return size in GB
    // = 2 * num_layers * num_heads * head_dim * max_seq_len * bytes_per_value / (1024^3)
    return 0.0;
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q15: KV Cache Manager ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Basic append and retrieve
    {
        total++;
        KVCache cache(2, 2, 4, 10);  // 2 layers, 2 heads, head_dim=4, max_seq=10

        // Create K, V for one token
        vector<vector<vector<float>>> k(2, vector<vector<float>>(2, vector<float>(4)));
        vector<vector<vector<float>>> v(2, vector<vector<float>>(2, vector<float>(4)));

        // Fill with known values
        for (int l = 0; l < 2; l++)
            for (int h = 0; h < 2; h++)
                for (int d = 0; d < 4; d++) {
                    k[l][h][d] = (float)(l * 100 + h * 10 + d);
                    v[l][h][d] = (float)(l * 100 + h * 10 + d + 1000);
                }

        bool appended = cache.append(k, v);
        int len;
        const float* keys = cache.get_keys(0, 0, len);

        bool ok = appended && len == 1 && keys != nullptr;
        if (ok) {
            for (int d = 0; d < 4; d++)
                if (fabs(keys[d] - k[0][0][d]) > 1e-5) ok = false;
        }
        printf("%s [Basic append and retrieve]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 2: Multiple tokens
    {
        total++;
        KVCache cache(1, 1, 4, 10);
        bool ok = true;

        for (int t = 0; t < 5; t++) {
            vector<vector<vector<float>>> k(1, vector<vector<float>>(1, vector<float>(4)));
            vector<vector<vector<float>>> v(1, vector<vector<float>>(1, vector<float>(4)));
            for (int d = 0; d < 4; d++) {
                k[0][0][d] = (float)(t * 10 + d);
                v[0][0][d] = (float)(t * 10 + d + 100);
            }
            cache.append(k, v);
        }

        int len;
        const float* keys = cache.get_keys(0, 0, len);
        ok = (len == 5);

        // Verify token 2's key data starts at offset 2 * head_dim
        if (ok && keys) {
            for (int d = 0; d < 4; d++)
                if (fabs(keys[2 * 4 + d] - (float)(20 + d)) > 1e-5) ok = false;
        }

        printf("%s [5 tokens cached, correct retrieval]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 3: Cache full
    {
        total++;
        KVCache cache(1, 1, 2, 3);  // max 3 tokens
        vector<vector<vector<float>>> kv(1, vector<vector<float>>(1, vector<float>(2, 1.0f)));

        bool ok = cache.append(kv, kv);  // token 0
        ok = ok && cache.append(kv, kv); // token 1
        ok = ok && cache.append(kv, kv); // token 2
        ok = ok && !cache.append(kv, kv); // should fail — full

        printf("%s [Cache full returns false]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 4: Memory calculation
    {
        total++;
        KVCache cache(2, 4, 8, 100);
        long long expected = 2LL * 2 * 4 * 100 * 8 * sizeof(float);
        long long actual = cache.memory_bytes();
        bool ok = (actual == expected);
        printf("%s [Memory: %lld bytes, expected %lld]\n",
               ok ? "PASS" : "FAIL", actual, expected);
        if (ok) passed++;
    }

    // Test 5: Realistic model KV cache size (70B model)
    {
        total++;
        // 70B: 80 layers, 64 heads (8 KV heads with GQA, but for simplicity use 64), head_dim=128
        // max_seq_len = 8192, fp16 (2 bytes)
        double gb = compute_kv_cache_size_gb(80, 64, 128, 8192, 2);
        // 2 * 80 * 64 * 128 * 8192 * 2 bytes = ~21.47 GB
        bool ok = (gb > 15.0 && gb < 25.0);  // reasonable range
        printf("%s [70B model KV cache = %.2f GB]\n", ok ? "PASS" : "FAIL", gb);
        if (ok) passed++;
    }

    // Test 6: Reset
    {
        total++;
        KVCache cache(1, 1, 2, 5);
        vector<vector<vector<float>>> kv(1, vector<vector<float>>(1, vector<float>(2, 1.0f)));
        cache.append(kv, kv);
        cache.append(kv, kv);
        cache.reset();

        int len;
        cache.get_keys(0, 0, len);
        bool ok = (len == 0 && cache.current_pos == 0);

        // Should be able to append again
        ok = ok && cache.append(kv, kv);
        cache.get_keys(0, 0, len);
        ok = ok && (len == 1);

        printf("%s [Reset and reuse]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
