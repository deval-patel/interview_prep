/*
 * Q10 — Roofline Model Analysis [Easy-Medium]
 *
 * Topic: Systems Programming — Performance Profiling
 *
 * Problem:
 *   Implement a roofline model analyzer that classifies kernels as
 *   memory-bound or compute-bound based on their arithmetic intensity.
 *
 *   The roofline model defines attainable performance as:
 *     perf = min(peak_compute, peak_bandwidth * arithmetic_intensity)
 *
 *   Where:
 *     - peak_compute: max FLOPS the hardware can deliver (e.g., 100 TFLOPS)
 *     - peak_bandwidth: max memory bandwidth (e.g., 1 TB/s)
 *     - arithmetic_intensity: FLOPs per byte of data moved (FLOPs/Byte)
 *     - ridge point: peak_compute / peak_bandwidth (the crossover AI)
 *
 * Constraints:
 *   - Model a TPU-like accelerator with given peak compute and bandwidth
 *   - Classify kernels: memory-bound (AI < ridge point) vs compute-bound
 *   - Compute utilization = attainable_perf / peak_compute
 *
 * Key Concepts:
 *   - Roofline model for performance analysis
 *   - Arithmetic intensity as the key metric
 *   - Memory-bound vs compute-bound classification
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cstring>

using namespace std;

// ============ YOUR IMPLEMENTATION ============

struct RooflineModel {
    double peak_compute_flops;    // peak FLOPS (e.g., 100e12 for 100 TFLOPS)
    double peak_bandwidth_bytes;  // peak bandwidth in bytes/sec (e.g., 1e12 for 1 TB/s)

    RooflineModel(double compute, double bandwidth)
        : peak_compute_flops(compute), peak_bandwidth_bytes(bandwidth) {}

    /*
     * ridge_point: The arithmetic intensity where the kernel transitions
     * from memory-bound to compute-bound.
     * ridge_point = peak_compute / peak_bandwidth (in FLOPs/Byte)
     */
    double ridge_point() {
        // TODO: Implement
        return 0.0;
    }

    /*
     * attainable_performance: Given a kernel's arithmetic intensity,
     * return the maximum achievable FLOPS.
     * = min(peak_compute, peak_bandwidth * arithmetic_intensity)
     */
    double attainable_performance(double arithmetic_intensity) {
        // TODO: Implement
        return 0.0;
    }

    /*
     * is_memory_bound: Returns true if the kernel is memory-bound
     * (arithmetic intensity < ridge point)
     */
    bool is_memory_bound(double arithmetic_intensity) {
        // TODO: Implement
        return false;
    }

    /*
     * utilization: What fraction of peak compute is achievable?
     * = attainable_performance / peak_compute
     */
    double utilization(double arithmetic_intensity) {
        // TODO: Implement
        return 0.0;
    }
};

/*
 * compute_arithmetic_intensity: Calculate FLOPs/Byte for common operations.
 *
 * Vector Add (C[i] = A[i] + B[i], length N):
 *   FLOPs = N, Bytes = 3*N*sizeof(float)  (read A, B; write C)
 *
 * Matrix Multiply (C[M][N] = A[M][K] * B[K][N]):
 *   FLOPs = 2*M*N*K (multiply + add per output element, K times)
 *   Bytes = (M*K + K*N + M*N) * sizeof(float)
 *
 * Reduction (sum of N elements):
 *   FLOPs = N-1, Bytes = N*sizeof(float) + sizeof(float)
 */
double ai_vector_add(int N) {
    // TODO: Return arithmetic intensity for vector add
    return 0.0;
}

double ai_matmul(int M, int K, int N) {
    // TODO: Return arithmetic intensity for matrix multiply
    return 0.0;
}

double ai_reduction(int N) {
    // TODO: Return arithmetic intensity for reduction
    return 0.0;
}

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q10: Roofline Model Analysis ===\n\n");
    int passed = 0, total = 0;

    // TPU-like specs: 100 TFLOPS compute, 1 TB/s bandwidth
    RooflineModel tpu(100e12, 1e12);

    // Test 1: Ridge point
    {
        total++;
        double rp = tpu.ridge_point();
        double expected = 100.0;  // 100e12 / 1e12 = 100 FLOPs/Byte
        bool ok = fabs(rp - expected) < 1e-6;
        printf("%s [Ridge point = %.1f FLOPs/Byte, expected 100.0]\n",
               ok ? "PASS" : "FAIL", rp);
        if (ok) passed++;
    }

    // Test 2: Memory-bound kernel (vector add)
    {
        total++;
        double ai = ai_vector_add(1024);
        bool mem_bound = tpu.is_memory_bound(ai);
        // Vector add AI = 1 / 12 ≈ 0.083, way below ridge point of 100
        bool ok = mem_bound && ai < 1.0;
        printf("%s [Vector add is memory-bound, AI=%.4f]\n", ok ? "PASS" : "FAIL", ai);
        if (ok) passed++;
    }

    // Test 3: Compute-bound kernel (large matmul)
    {
        total++;
        double ai = ai_matmul(4096, 4096, 4096);
        bool comp_bound = !tpu.is_memory_bound(ai);
        // Large matmul AI ≈ 2*4096 / 3*4 ≈ 682.7, above ridge point
        bool ok = comp_bound && ai > 100.0;
        printf("%s [Large matmul is compute-bound, AI=%.1f]\n", ok ? "PASS" : "FAIL", ai);
        if (ok) passed++;
    }

    // Test 4: Attainable performance for memory-bound kernel
    {
        total++;
        double ai = 10.0;  // below ridge point of 100
        double perf = tpu.attainable_performance(ai);
        double expected = 1e12 * 10.0;  // bandwidth * AI = 10 TFLOPS
        bool ok = fabs(perf - expected) / expected < 1e-6;
        printf("%s [AI=10: attainable = %.1f TFLOPS]\n",
               ok ? "PASS" : "FAIL", perf / 1e12);
        if (ok) passed++;
    }

    // Test 5: Attainable performance for compute-bound kernel
    {
        total++;
        double ai = 200.0;  // above ridge point of 100
        double perf = tpu.attainable_performance(ai);
        double expected = 100e12;  // capped at peak compute
        bool ok = fabs(perf - expected) / expected < 1e-6;
        printf("%s [AI=200: attainable = %.1f TFLOPS (capped at peak)]\n",
               ok ? "PASS" : "FAIL", perf / 1e12);
        if (ok) passed++;
    }

    // Test 6: Utilization
    {
        total++;
        double util_low = tpu.utilization(10.0);    // 10/100 = 10%
        double util_high = tpu.utilization(200.0);   // capped at 100%
        bool ok = fabs(util_low - 0.1) < 1e-6 && fabs(util_high - 1.0) < 1e-6;
        printf("%s [Utilization: AI=10 -> %.0f%%, AI=200 -> %.0f%%]\n",
               ok ? "PASS" : "FAIL", util_low * 100, util_high * 100);
        if (ok) passed++;
    }

    // Test 7: Reduction is memory-bound
    {
        total++;
        double ai = ai_reduction(1024);
        bool ok = tpu.is_memory_bound(ai);
        printf("%s [Reduction is memory-bound, AI=%.4f]\n", ok ? "PASS" : "FAIL", ai);
        if (ok) passed++;
    }

    // Test 8: Small matmul can be memory-bound
    {
        total++;
        double ai = ai_matmul(2, 2, 2);
        // 2*2*2*2 = 16 FLOPs, (4+4+4)*4 = 48 bytes, AI = 0.33
        bool ok = tpu.is_memory_bound(ai);
        printf("%s [2x2 matmul is memory-bound, AI=%.4f]\n", ok ? "PASS" : "FAIL", ai);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
