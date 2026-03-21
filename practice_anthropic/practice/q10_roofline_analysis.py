"""
Q10 -- Roofline Model Analysis [Easy-Medium]

Topic: Systems Programming -- Performance Profiling

Problem:
  Implement a roofline model analyzer that classifies kernels as
  memory-bound or compute-bound based on their arithmetic intensity.

  The roofline model defines attainable performance as:
    perf = min(peak_compute, peak_bandwidth * arithmetic_intensity)

  Where:
    - peak_compute: max FLOPS the hardware can deliver (e.g., 100 TFLOPS)
    - peak_bandwidth: max memory bandwidth (e.g., 1 TB/s)
    - arithmetic_intensity: FLOPs per byte of data moved (FLOPs/Byte)
    - ridge point: peak_compute / peak_bandwidth (the crossover AI)

Constraints:
  - Model a TPU-like accelerator with given peak compute and bandwidth
  - Classify kernels: memory-bound (AI < ridge point) vs compute-bound
  - Compute utilization = attainable_perf / peak_compute

Key Concepts:
  - Roofline model for performance analysis
  - Arithmetic intensity as the key metric
  - Memory-bound vs compute-bound classification
"""


# ============ YOUR IMPLEMENTATION ============

class RooflineModel:
    def __init__(self, peak_compute_flops, peak_bandwidth_bytes):
        """
        Args:
            peak_compute_flops: peak FLOPS (e.g., 100e12 for 100 TFLOPS)
            peak_bandwidth_bytes: peak bandwidth in bytes/sec (e.g., 1e12 for 1 TB/s)
        """
        self.peak_compute_flops = peak_compute_flops
        self.peak_bandwidth_bytes = peak_bandwidth_bytes

    def ridge_point(self):
        """
        The arithmetic intensity where the kernel transitions
        from memory-bound to compute-bound.
        ridge_point = peak_compute / peak_bandwidth (in FLOPs/Byte)
        """
        # TODO: Implement
        return 0.0

    def attainable_performance(self, arithmetic_intensity):
        """
        Given a kernel's arithmetic intensity, return the maximum
        achievable FLOPS.
        = min(peak_compute, peak_bandwidth * arithmetic_intensity)
        """
        # TODO: Implement
        return 0.0

    def is_memory_bound(self, arithmetic_intensity):
        """
        Returns True if the kernel is memory-bound
        (arithmetic intensity < ridge point)
        """
        # TODO: Implement
        return False

    def utilization(self, arithmetic_intensity):
        """
        What fraction of peak compute is achievable?
        = attainable_performance / peak_compute
        """
        # TODO: Implement
        return 0.0


def ai_vector_add(n):
    """
    Compute arithmetic intensity for vector add: C[i] = A[i] + B[i], length N.
    FLOPs = N, Bytes = 3*N*4 (read A, B; write C, each float is 4 bytes)

    Args:
        n: vector length

    Returns:
        float arithmetic intensity (FLOPs/Byte)
    """
    # TODO: Return arithmetic intensity for vector add
    return 0.0


def ai_matmul(M, K, N):
    """
    Compute arithmetic intensity for matrix multiply: C[M][N] = A[M][K] * B[K][N].
    FLOPs = 2*M*N*K (multiply + add per output element, K times)
    Bytes = (M*K + K*N + M*N) * 4 (sizeof float)

    Args:
        M, K, N: matrix dimensions

    Returns:
        float arithmetic intensity (FLOPs/Byte)
    """
    # TODO: Return arithmetic intensity for matrix multiply
    return 0.0


def ai_reduction(n):
    """
    Compute arithmetic intensity for reduction (sum of N elements).
    FLOPs = N-1, Bytes = N*4 + 4 (read N floats, write 1 float)

    Args:
        n: number of elements

    Returns:
        float arithmetic intensity (FLOPs/Byte)
    """
    # TODO: Return arithmetic intensity for reduction
    return 0.0


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q10: Roofline Model Analysis ===\n")
    passed = 0
    total = 0

    # TPU-like specs: 100 TFLOPS compute, 1 TB/s bandwidth
    tpu = RooflineModel(100e12, 1e12)

    # Test 1: Ridge point
    total += 1
    rp = tpu.ridge_point()
    expected = 100.0  # 100e12 / 1e12 = 100 FLOPs/Byte
    ok = abs(rp - expected) < 1e-6
    print(f"{'PASS' if ok else 'FAIL'} [Ridge point = {rp:.1f} FLOPs/Byte, expected 100.0]")
    if ok:
        passed += 1

    # Test 2: Memory-bound kernel (vector add)
    total += 1
    ai = ai_vector_add(1024)
    mem_bound = tpu.is_memory_bound(ai)
    # Vector add AI = 1 / 12 ~ 0.083, way below ridge point of 100
    ok = mem_bound and ai < 1.0
    print(f"{'PASS' if ok else 'FAIL'} [Vector add is memory-bound, AI={ai:.4f}]")
    if ok:
        passed += 1

    # Test 3: Compute-bound kernel (large matmul)
    total += 1
    ai = ai_matmul(4096, 4096, 4096)
    comp_bound = not tpu.is_memory_bound(ai)
    # Large matmul AI ~ 2*4096 / 3*4 ~ 682.7, above ridge point
    ok = comp_bound and ai > 100.0
    print(f"{'PASS' if ok else 'FAIL'} [Large matmul is compute-bound, AI={ai:.1f}]")
    if ok:
        passed += 1

    # Test 4: Attainable performance for memory-bound kernel
    total += 1
    ai = 10.0  # below ridge point of 100
    perf = tpu.attainable_performance(ai)
    expected = 1e12 * 10.0  # bandwidth * AI = 10 TFLOPS
    ok = abs(perf - expected) / expected < 1e-6
    print(f"{'PASS' if ok else 'FAIL'} [AI=10: attainable = {perf / 1e12:.1f} TFLOPS]")
    if ok:
        passed += 1

    # Test 5: Attainable performance for compute-bound kernel
    total += 1
    ai = 200.0  # above ridge point of 100
    perf = tpu.attainable_performance(ai)
    expected = 100e12  # capped at peak compute
    ok = abs(perf - expected) / expected < 1e-6
    print(f"{'PASS' if ok else 'FAIL'} [AI=200: attainable = {perf / 1e12:.1f} TFLOPS (capped at peak)]")
    if ok:
        passed += 1

    # Test 6: Utilization
    total += 1
    util_low = tpu.utilization(10.0)    # 10/100 = 10%
    util_high = tpu.utilization(200.0)   # capped at 100%
    ok = abs(util_low - 0.1) < 1e-6 and abs(util_high - 1.0) < 1e-6
    print(f"{'PASS' if ok else 'FAIL'} [Utilization: AI=10 -> {util_low * 100:.0f}%, AI=200 -> {util_high * 100:.0f}%]")
    if ok:
        passed += 1

    # Test 7: Reduction is memory-bound
    total += 1
    ai = ai_reduction(1024)
    ok = tpu.is_memory_bound(ai)
    print(f"{'PASS' if ok else 'FAIL'} [Reduction is memory-bound, AI={ai:.4f}]")
    if ok:
        passed += 1

    # Test 8: Small matmul can be memory-bound
    total += 1
    ai = ai_matmul(2, 2, 2)
    # 2*2*2*2 = 16 FLOPs, (4+4+4)*4 = 48 bytes, AI = 0.33
    ok = tpu.is_memory_bound(ai)
    print(f"{'PASS' if ok else 'FAIL'} [2x2 matmul is memory-bound, AI={ai:.4f}]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
