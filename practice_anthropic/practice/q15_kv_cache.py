"""
Q15 -- KV Cache Manager [Medium]

Topic: ML Context -- Transformer Architecture / KV Cache

Problem:
  Implement a KV (Key-Value) cache for autoregressive LLM inference.

  During generation, each new token needs to attend to all previous tokens.
  Without a KV cache, we'd recompute K and V projections for all past tokens
  at each step. The KV cache stores previously computed K and V vectors.

  Implement:
  1. KVCache class with pre-allocated buffer for max_seq_len tokens
  2. append(): Add new K, V for the current token
  3. get_keys()/get_values(): Return all cached K/V up to current position
  4. Memory analysis for realistic model configurations

Constraints:
  - Support multi-layer, multi-head caching
  - Pre-allocate for max_seq_len to avoid runtime allocation
  - Track memory usage

Key Concepts:
  - KV cache structure and lifecycle
  - Memory bottleneck in LLM inference
  - Why KV cache size dominates inference memory for long sequences
"""


# ============ YOUR IMPLEMENTATION ============

class KVCache:
    def __init__(self, num_layers, num_heads, head_dim, max_seq_len):
        """
        Constructor: Pre-allocate KV cache for all layers and heads.

        Storage layout: keys[layer][head] is a flat list of max_seq_len * head_dim floats.

        Args:
            num_layers: int
            num_heads: int
            head_dim: int
            max_seq_len: int
        """
        self.num_layers = num_layers
        self.num_heads = num_heads
        self.head_dim = head_dim
        self.max_seq_len = max_seq_len
        self.current_pos = 0  # number of tokens currently cached

        # TODO: Allocate keys and values arrays
        # keys[l][h] should have space for max_seq_len * head_dim floats
        self.keys = None    # [layer][head][pos * head_dim ..]
        self.values = None

    def append(self, new_keys, new_values):
        """
        append: Add K, V vectors for a new token across all layers and heads.

        new_keys[layer][head] = list of head_dim floats
        new_values[layer][head] = list of head_dim floats

        Copy into position current_pos, then increment current_pos.

        Args:
            new_keys: list[list[list[float]]], shape [num_layers][num_heads][head_dim]
            new_values: list[list[list[float]]], shape [num_layers][num_heads][head_dim]

        Returns:
            bool: False if cache is full (current_pos >= max_seq_len)
        """
        # TODO: Implement
        # Return False if cache is full (current_pos >= max_seq_len)
        return False

    def get_keys(self, layer, head):
        """
        get_keys: Return all cached keys for a given layer and head,
        up to current_pos.

        Returns:
            tuple: (data, length) where data is the flat list of key floats
                   and length is current_pos
        """
        # TODO: Return keys[layer][head] data, length = current_pos
        return None, 0

    def get_values(self, layer, head):
        """
        get_values: Same as get_keys but for values.

        Returns:
            tuple: (data, length) where data is the flat list of value floats
                   and length is current_pos
        """
        # TODO: Return values[layer][head] data, length = current_pos
        return None, 0

    def memory_bytes(self):
        """
        memory_bytes: Return total memory used by the KV cache in bytes.
        = 2 (K+V) * num_layers * num_heads * max_seq_len * head_dim * sizeof(float)
        (Assume 4 bytes per float)
        """
        # TODO: Implement
        return 0

    def reset(self):
        """reset: Clear the cache (reset position to 0)."""
        self.current_pos = 0


def compute_kv_cache_size_gb(num_layers, num_heads, head_dim, max_seq_len, bytes_per_value=2):
    """
    compute_kv_cache_size_gb: Compute KV cache size in GB for a model config.

    Common model sizes:
      - 7B model:  32 layers, 32 heads, head_dim=128
      - 13B model: 40 layers, 40 heads, head_dim=128
      - 70B model: 80 layers, 64 heads, head_dim=128

    For a given max_seq_len, how much memory does the KV cache need?
    Uses float16 (2 bytes per value) for realistic estimation.

    Returns:
        float: size in GB
        = 2 * num_layers * num_heads * head_dim * max_seq_len * bytes_per_value / (1024^3)
    """
    # TODO: Return size in GB
    return 0.0


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q15: KV Cache Manager ===\n")
    passed = 0
    total = 0

    # Test 1: Basic append and retrieve
    total += 1
    cache = KVCache(2, 2, 4, 10)  # 2 layers, 2 heads, head_dim=4, max_seq=10

    # Create K, V for one token
    k = [[[float(l * 100 + h * 10 + d) for d in range(4)] for h in range(2)] for l in range(2)]
    v = [[[float(l * 100 + h * 10 + d + 1000) for d in range(4)] for h in range(2)] for l in range(2)]

    appended = cache.append(k, v)
    keys_data, length = cache.get_keys(0, 0)

    ok = appended and length == 1 and keys_data is not None
    if ok:
        for d in range(4):
            if abs(keys_data[d] - k[0][0][d]) > 1e-5:
                ok = False
    print(f"{'PASS' if ok else 'FAIL'} [Basic append and retrieve]")
    if ok:
        passed += 1

    # Test 2: Multiple tokens
    total += 1
    cache = KVCache(1, 1, 4, 10)
    ok = True

    for t in range(5):
        k = [[[float(t * 10 + d) for d in range(4)]]]
        v = [[[float(t * 10 + d + 100) for d in range(4)]]]
        cache.append(k, v)

    keys_data, length = cache.get_keys(0, 0)
    ok = (length == 5)

    # Verify token 2's key data starts at offset 2 * head_dim
    if ok and keys_data is not None:
        for d in range(4):
            if abs(keys_data[2 * 4 + d] - float(20 + d)) > 1e-5:
                ok = False

    print(f"{'PASS' if ok else 'FAIL'} [5 tokens cached, correct retrieval]")
    if ok:
        passed += 1

    # Test 3: Cache full
    total += 1
    cache = KVCache(1, 1, 2, 3)  # max 3 tokens
    kv = [[[1.0, 1.0]]]

    ok = cache.append(kv, kv)       # token 0
    ok = ok and cache.append(kv, kv)  # token 1
    ok = ok and cache.append(kv, kv)  # token 2
    ok = ok and not cache.append(kv, kv)  # should fail -- full

    print(f"{'PASS' if ok else 'FAIL'} [Cache full returns False]")
    if ok:
        passed += 1

    # Test 4: Memory calculation
    total += 1
    cache = KVCache(2, 4, 8, 100)
    expected = 2 * 2 * 4 * 100 * 8 * 4  # 4 bytes per float
    actual = cache.memory_bytes()
    ok = (actual == expected)
    print(f"{'PASS' if ok else 'FAIL'} [Memory: {actual} bytes, expected {expected}]")
    if ok:
        passed += 1

    # Test 5: Realistic model KV cache size (70B model)
    total += 1
    # 70B: 80 layers, 64 heads, head_dim=128, max_seq_len=8192, fp16 (2 bytes)
    gb = compute_kv_cache_size_gb(80, 64, 128, 8192, 2)
    # 2 * 80 * 64 * 128 * 8192 * 2 bytes = ~21.47 GB
    ok = (15.0 < gb < 25.0)
    print(f"{'PASS' if ok else 'FAIL'} [70B model KV cache = {gb:.2f} GB]")
    if ok:
        passed += 1

    # Test 6: Reset
    total += 1
    cache = KVCache(1, 1, 2, 5)
    kv = [[[1.0, 1.0]]]
    cache.append(kv, kv)
    cache.append(kv, kv)
    cache.reset()

    _, length = cache.get_keys(0, 0)
    ok = (length == 0 and cache.current_pos == 0)

    # Should be able to append again
    ok = ok and cache.append(kv, kv)
    _, length = cache.get_keys(0, 0)
    ok = ok and (length == 1)

    print(f"{'PASS' if ok else 'FAIL'} [Reset and reuse]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
