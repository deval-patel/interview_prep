/*
 * TPU Kernel Engineer Practice — Reference Solutions
 *
 * Complete, working implementations for all 18 practice questions.
 * Each solution matches the exact function signatures from the corresponding
 * practice file and is designed to pass all tests defined therein.
 *
 * Topics covered:
 *   Q01: Systolic Array Matrix Multiply
 *   Q02: Memory Hierarchy Simulator (HBM / SRAM)
 *   Q03: SIMD Vector Operations
 *   Q04: Tiled Matrix Multiplication
 *   Q05: Operator Fusion (ReLU + Bias + MatMul)
 *   Q06: BFloat16 Arithmetic
 *   Q07: FP8 Quantization and Dequantization
 *   Q08: All-Reduce Ring Algorithm
 *   Q09: Reduce-Scatter with Recursive Halving
 *   Q10: Roofline Model Analysis
 *   Q11: Bitwise Data Packing
 *   Q12: DMA Double Buffering
 *   Q13: Softmax Kernel with Numerical Stability
 *   Q14: Scaled Dot-Product Attention
 *   Q15: KV Cache Manager
 *   Q16: XLA-Style Tile Scheduler
 *   Q17: 2D Convolution with im2col
 *   Q18: VLIW Instruction Packing
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <cfloat>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>

using namespace std;


// ============================================================================
// Q01 -- Systolic Array Matrix Multiply
// ============================================================================

/*
 * Approach: Weight-stationary systolic array with K x N PE grid.
 * Weight B[k][n] lives in PE[k][n]. Activations from A flow left-to-right
 * with skewing by row. Partial sums accumulate top-to-bottom.
 * We use a double-buffer approach in step() to avoid read-after-write issues.
 */

struct PE {
    float weight;
    float acc;
    float in_left;
    float in_top;
    float out_right;
    float out_bottom;
    bool has_input;
    bool has_top;
};

void naive_matmul(const vector<vector<float>>& A,
                  const vector<vector<float>>& B,
                  vector<vector<float>>& C,
                  int M, int K, int N) {
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0f;
            for (int k = 0; k < K; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
}

struct SystolicArray {
    int K, N;
    vector<vector<PE>> grid;

    SystolicArray(int K, int N) : K(K), N(N), grid(K, vector<PE>(N)) {}

    void load_weights(const vector<vector<float>>& B) {
        for (int k = 0; k < K; k++)
            for (int n = 0; n < N; n++) {
                grid[k][n].weight = B[k][n];
                grid[k][n].acc = 0.0f;
                grid[k][n].in_left = 0.0f;
                grid[k][n].in_top = 0.0f;
                grid[k][n].out_right = 0.0f;
                grid[k][n].out_bottom = 0.0f;
                grid[k][n].has_input = false;
                grid[k][n].has_top = false;
            }
    }

    void step(const vector<float>& cycle_inputs, const vector<bool>& input_valid) {
        vector<vector<float>> new_out_right(K, vector<float>(N, 0.0f));
        vector<vector<float>> new_out_bottom(K, vector<float>(N, 0.0f));
        vector<vector<bool>>  new_has_input(K, vector<bool>(N, false));
        vector<vector<bool>>  new_has_top(K, vector<bool>(N, false));

        for (int k = 0; k < K; k++) {
            for (int n = 0; n < N; n++) {
                float left_val = 0.0f;
                bool left_valid = false;
                if (n == 0) {
                    left_val = cycle_inputs[k];
                    left_valid = input_valid[k];
                } else {
                    left_val = grid[k][n - 1].out_right;
                    left_valid = grid[k][n - 1].has_input;
                }

                float top_val = 0.0f;
                bool top_valid = false;
                if (k == 0) {
                    top_val = 0.0f;
                    top_valid = true;
                } else {
                    top_val = grid[k - 1][n].out_bottom;
                    top_valid = grid[k - 1][n].has_top;
                }

                new_has_input[k][n] = left_valid;
                new_has_top[k][n] = left_valid || top_valid;

                if (left_valid) {
                    new_out_bottom[k][n] = top_val + grid[k][n].weight * left_val;
                    new_out_right[k][n] = left_val;
                } else {
                    new_out_bottom[k][n] = top_val;
                    new_out_right[k][n] = 0.0f;
                }
            }
        }

        for (int k = 0; k < K; k++)
            for (int n = 0; n < N; n++) {
                grid[k][n].out_right = new_out_right[k][n];
                grid[k][n].out_bottom = new_out_bottom[k][n];
                grid[k][n].has_input = new_has_input[k][n];
                grid[k][n].has_top = new_has_top[k][n];
            }
    }

    float get_output(int col) {
        return grid[K - 1][col].out_bottom;
    }
};

/*
 * Total cycles = M + K + N - 2.
 * At cycle t, PE row k receives A[m][k] where m = t - k (skewed input).
 * Output for C[m][n] emerges from PE[K-1][n] at cycle m + K - 1 + n,
 * because data flows left-to-right with 1 cycle delay per column.
 */
void systolic_matmul(const vector<vector<float>>& A,
                     const vector<vector<float>>& B,
                     vector<vector<float>>& C,
                     int M, int K, int N) {
    SystolicArray sa(K, N);
    sa.load_weights(B);

    int total_cycles = M + K + N - 2;

    for (int t = 0; t < total_cycles; t++) {
        vector<float> inputs(K, 0.0f);
        vector<bool> valid(K, false);

        for (int k = 0; k < K; k++) {
            int m = t - k;
            if (m >= 0 && m < M) {
                inputs[k] = A[m][k];
                valid[k] = true;
            }
        }

        sa.step(inputs, valid);

        // Collect outputs: C[m][n] is ready at cycle m + K - 1 + n
        for (int n = 0; n < N; n++) {
            int m_out = t - (K - 1) - n;
            if (m_out >= 0 && m_out < M) {
                C[m_out][n] = sa.get_output(n);
            }
        }
    }
}


// ============================================================================
// Q02 -- Memory Hierarchy Simulator (HBM / SRAM)
// ============================================================================

/*
 * Approach: Simulate two-level memory. HBM has 100-cycle latency per tile
 * transfer, SRAM has 1-cycle latency per element. Tiled version loads bulk
 * tiles into SRAM, drastically reducing HBM accesses.
 */

struct MemorySystem {
    long long total_cycles;
    long long hbm_accesses;
    long long hbm_bytes;

    static const int HBM_LATENCY = 100;
    static const int SRAM_LATENCY = 1;
    static const int SRAM_CAPACITY = 1024;

    vector<float> sram;

    MemorySystem() : total_cycles(0), hbm_accesses(0), hbm_bytes(0),
                     sram(SRAM_CAPACITY, 0.0f) {}

    void reset() {
        total_cycles = 0;
        hbm_accesses = 0;
        hbm_bytes = 0;
    }

    void load_tile(const float* hbm_src, int sram_offset, int count) {
        memcpy(&sram[sram_offset], hbm_src, count * sizeof(float));
        total_cycles += HBM_LATENCY;
        hbm_accesses++;
        hbm_bytes += count * sizeof(float);
    }

    void store_tile(float* hbm_dst, int sram_offset, int count) {
        memcpy(hbm_dst, &sram[sram_offset], count * sizeof(float));
        total_cycles += HBM_LATENCY;
        hbm_accesses++;
        hbm_bytes += count * sizeof(float);
    }

    float sram_read(int offset) {
        total_cycles += SRAM_LATENCY;
        return sram[offset];
    }

    void sram_write(int offset, float val) {
        total_cycles += SRAM_LATENCY;
        sram[offset] = val;
    }
};

void vector_add_naive(MemorySystem& mem,
                      float* hbm_A, float* hbm_B, float* hbm_C,
                      int N) {
    for (int i = 0; i < N; i++) {
        mem.load_tile(&hbm_A[i], 0, 1);
        mem.load_tile(&hbm_B[i], 1, 1);
        float a = mem.sram_read(0);
        float b = mem.sram_read(1);
        mem.sram_write(2, a + b);
        mem.store_tile(&hbm_C[i], 2, 1);
    }
}

void vector_add_tiled(MemorySystem& mem,
                      float* hbm_A, float* hbm_B, float* hbm_C,
                      int N) {
    int tile_size = MemorySystem::SRAM_CAPACITY / 3;

    for (int base = 0; base < N; base += tile_size) {
        int count = min(tile_size, N - base);
        mem.load_tile(&hbm_A[base], 0, count);
        mem.load_tile(&hbm_B[base], tile_size, count);

        for (int i = 0; i < count; i++) {
            float a = mem.sram_read(i);
            float b = mem.sram_read(tile_size + i);
            mem.sram_write(2 * tile_size + i, a + b);
        }

        mem.store_tile(&hbm_C[base], 2 * tile_size, count);
    }
}

float compute_arithmetic_intensity(int N) {
    float flops = (float)N;
    float bytes = 3.0f * N * sizeof(float);
    return flops / bytes;
}


// ============================================================================
// Q03 -- SIMD Vector Operations
// ============================================================================

/*
 * Approach: Strip-mine loops to process VECTOR_WIDTH elements per iteration
 * using simulated SIMD intrinsics, with scalar fallback for tail elements.
 */

const int VECTOR_WIDTH = 8;

struct SimdReg { float data[8]; };

SimdReg simd_load(const float* ptr) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = ptr[i];
    return r;
}
void simd_store(float* ptr, SimdReg r) {
    for (int i = 0; i < VECTOR_WIDTH; i++) ptr[i] = r.data[i];
}
SimdReg simd_add(SimdReg a, SimdReg b) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = a.data[i] + b.data[i];
    return r;
}
SimdReg simd_mul(SimdReg a, SimdReg b) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = a.data[i] * b.data[i];
    return r;
}
SimdReg simd_max(SimdReg a, SimdReg b) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = max(a.data[i], b.data[i]);
    return r;
}
SimdReg simd_set1(float val) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = val;
    return r;
}
SimdReg simd_setzero() { return simd_set1(0.0f); }
float simd_hsum(SimdReg r) {
    float sum = 0;
    for (int i = 0; i < VECTOR_WIDTH; i++) sum += r.data[i];
    return sum;
}
SimdReg simd_fmadd(SimdReg a, SimdReg b, SimdReg c) {
    SimdReg r;
    for (int i = 0; i < VECTOR_WIDTH; i++) r.data[i] = a.data[i] * b.data[i] + c.data[i];
    return r;
}

void simd_vector_add(const float* A, const float* B, float* C, int N) {
    int i = 0;
    for (; i + VECTOR_WIDTH <= N; i += VECTOR_WIDTH) {
        simd_store(&C[i], simd_add(simd_load(&A[i]), simd_load(&B[i])));
    }
    for (; i < N; i++) C[i] = A[i] + B[i];
}

float simd_dot_product(const float* A, const float* B, int N) {
    SimdReg acc = simd_setzero();
    int i = 0;
    for (; i + VECTOR_WIDTH <= N; i += VECTOR_WIDTH) {
        acc = simd_add(acc, simd_mul(simd_load(&A[i]), simd_load(&B[i])));
    }
    float result = simd_hsum(acc);
    for (; i < N; i++) result += A[i] * B[i];
    return result;
}

void simd_relu(const float* in, float* out, int N) {
    SimdReg zero = simd_set1(0.0f);
    int i = 0;
    for (; i + VECTOR_WIDTH <= N; i += VECTOR_WIDTH) {
        simd_store(&out[i], simd_max(simd_load(&in[i]), zero));
    }
    for (; i < N; i++) out[i] = max(0.0f, in[i]);
}

void simd_fma(const float* A, const float* B, const float* C, float* out, int N) {
    int i = 0;
    for (; i + VECTOR_WIDTH <= N; i += VECTOR_WIDTH) {
        simd_store(&out[i], simd_fmadd(simd_load(&A[i]), simd_load(&B[i]), simd_load(&C[i])));
    }
    for (; i < N; i++) out[i] = A[i] * B[i] + C[i];
}


// ============================================================================
// Q04 -- Tiled Matrix Multiplication
// ============================================================================

/*
 * Approach: 6 nested loops (3 tile loops + 3 inner loops). Each tile of A and B
 * is loaded into SRAM-simulated buffers. HBM loads are counted per tile load,
 * not per element access within a tile.
 */

const int TILE_SIZE = 16;

struct MatmulStats {
    long long hbm_loads;
    long long hbm_stores;
    long long flops;
};

void naive_matmul_q04(const vector<vector<float>>& A,
                      const vector<vector<float>>& B,
                      vector<vector<float>>& C,
                      int M, int K, int N,
                      MatmulStats& stats) {
    stats = {0, 0, 0};
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0f;
            for (int k = 0; k < K; k++) {
                stats.hbm_loads += 2;
                C[i][j] += A[i][k] * B[k][j];
                stats.flops += 2;
            }
            stats.hbm_stores++;
        }
}

void tiled_matmul(const vector<vector<float>>& A,
                  const vector<vector<float>>& B,
                  vector<vector<float>>& C,
                  int M, int K, int N,
                  MatmulStats& stats) {
    stats = {0, 0, 0};
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            C[i][j] = 0.0f;

    float A_tile[TILE_SIZE][TILE_SIZE];
    float B_tile[TILE_SIZE][TILE_SIZE];
    float C_tile[TILE_SIZE][TILE_SIZE];

    for (int ti = 0; ti < M; ti += TILE_SIZE) {
        int ti_end = min(ti + TILE_SIZE, M);
        for (int tj = 0; tj < N; tj += TILE_SIZE) {
            int tj_end = min(tj + TILE_SIZE, N);
            memset(C_tile, 0, sizeof(C_tile));

            for (int tk = 0; tk < K; tk += TILE_SIZE) {
                int tk_end = min(tk + TILE_SIZE, K);

                for (int i = ti; i < ti_end; i++)
                    for (int k = tk; k < tk_end; k++) {
                        A_tile[i - ti][k - tk] = A[i][k];
                        stats.hbm_loads++;
                    }
                for (int k = tk; k < tk_end; k++)
                    for (int j = tj; j < tj_end; j++) {
                        B_tile[k - tk][j - tj] = B[k][j];
                        stats.hbm_loads++;
                    }

                for (int i = 0; i < ti_end - ti; i++)
                    for (int j = 0; j < tj_end - tj; j++)
                        for (int k = 0; k < tk_end - tk; k++) {
                            C_tile[i][j] += A_tile[i][k] * B_tile[k][j];
                            stats.flops += 2;
                        }
            }

            for (int i = ti; i < ti_end; i++)
                for (int j = tj; j < tj_end; j++) {
                    C[i][j] = C_tile[i - ti][j - tj];
                    stats.hbm_stores++;
                }
        }
    }
}


// ============================================================================
// Q05 -- Operator Fusion (ReLU + Bias + MatMul)
// ============================================================================

/*
 * Approach: Unfused version does 3 separate passes (matmul, bias, relu),
 * each writing intermediates to HBM. Fused version computes everything
 * in a single pass — intermediates never touch HBM.
 */

struct FusionStats {
    long long hbm_reads;
    long long hbm_writes;
    long long total_hbm_bytes() { return (hbm_reads + hbm_writes) * (long long)sizeof(float); }
};

void unfused_matmul_bias_relu(const vector<vector<float>>& A,
                               const vector<vector<float>>& B,
                               const vector<float>& bias,
                               vector<vector<float>>& Y,
                               int M, int K, int N,
                               FusionStats& stats) {
    stats = {0, 0};

    // Step 1: T1 = A * B
    vector<vector<float>> T1(M, vector<float>(N, 0.0f));
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int k = 0; k < K; k++) {
                sum += A[i][k] * B[k][j];
                stats.hbm_reads += 2;
            }
            T1[i][j] = sum;
            stats.hbm_writes++;
        }

    // Step 2: T2 = T1 + bias
    vector<vector<float>> T2(M, vector<float>(N, 0.0f));
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++) {
            stats.hbm_reads += 2;
            T2[i][j] = T1[i][j] + bias[j];
            stats.hbm_writes++;
        }

    // Step 3: Y = ReLU(T2)
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++) {
            stats.hbm_reads++;
            Y[i][j] = max(0.0f, T2[i][j]);
            stats.hbm_writes++;
        }
}

void fused_matmul_bias_relu(const vector<vector<float>>& A,
                             const vector<vector<float>>& B,
                             const vector<float>& bias,
                             vector<vector<float>>& Y,
                             int M, int K, int N,
                             FusionStats& stats) {
    stats = {0, 0};
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++) {
            float sum = 0.0f;
            for (int k = 0; k < K; k++) {
                sum += A[i][k] * B[k][j];
                stats.hbm_reads += 2;
            }
            sum += bias[j];
            stats.hbm_reads++;
            Y[i][j] = max(0.0f, sum);
            stats.hbm_writes++;
        }
}


// ============================================================================
// Q06 -- BFloat16 Arithmetic
// ============================================================================

/*
 * Approach: BFloat16 is the upper 16 bits of IEEE 754 float32.
 * Conversion uses round-to-nearest-even on the truncated lower 16 bits.
 * Dot product accumulates in float32 (mixed-precision pattern).
 */

using bf16_t = uint16_t;

bf16_t float_to_bf16(float f) {
    uint32_t bits;
    memcpy(&bits, &f, sizeof(uint32_t));

    if (isnan(f)) {
        uint16_t result = (uint16_t)(bits >> 16);
        if ((result & 0x7F) == 0) result |= 0x0040;
        return result;
    }

    uint32_t round_bit = (bits >> 15) & 1;
    uint32_t sticky = bits & 0x7FFF;
    uint32_t upper = bits >> 16;

    if (round_bit && sticky) {
        upper++;
    } else if (round_bit && !sticky) {
        if (upper & 1) upper++;
    }

    return (bf16_t)(upper & 0xFFFF);
}

float bf16_to_float(bf16_t b) {
    uint32_t bits = ((uint32_t)b) << 16;
    float result;
    memcpy(&result, &bits, sizeof(float));
    return result;
}

bf16_t bf16_add(bf16_t a, bf16_t b) {
    return float_to_bf16(bf16_to_float(a) + bf16_to_float(b));
}

bf16_t bf16_mul(bf16_t a, bf16_t b) {
    return float_to_bf16(bf16_to_float(a) * bf16_to_float(b));
}

float bf16_dot_product(const bf16_t* a, const bf16_t* b, int N) {
    float acc = 0.0f;
    for (int i = 0; i < N; i++)
        acc += bf16_to_float(a[i]) * bf16_to_float(b[i]);
    return acc;
}


// ============================================================================
// Q07 -- FP8 Quantization and Dequantization
// ============================================================================

/*
 * Approach: Per-tensor scaling maps float range into FP8 range.
 * float_to_fp8 decomposes the scaled float into IEEE 754 components
 * and repacks into 8-bit format with rounding.
 * fp8_to_float reverses the process and multiplies by scale.
 */

using fp8_t = uint8_t;

struct FP8Format {
    int exp_bits;
    int man_bits;
    int exp_bias;
    float max_val;
    const char* name;
};

const FP8Format E4M3 = {4, 3, 7, 448.0f, "E4M3"};
const FP8Format E5M2 = {5, 2, 15, 57344.0f, "E5M2"};

float compute_scale(const float* data, int N, const FP8Format& fmt) {
    float max_abs = 0.0f;
    for (int i = 0; i < N; i++) {
        float absv = fabsf(data[i]);
        if (absv > max_abs) max_abs = absv;
    }
    if (max_abs == 0.0f) return 1.0f;
    return max_abs / fmt.max_val;
}

fp8_t float_to_fp8(float value, float scale, const FP8Format& fmt) {
    if (scale == 0.0f) return 0;
    float scaled = value / scale;
    if (scaled == 0.0f) return 0;

    uint8_t sign = 0;
    if (scaled < 0.0f) { sign = 1; scaled = -scaled; }
    if (scaled > fmt.max_val) scaled = fmt.max_val;

    uint32_t fbits;
    memcpy(&fbits, &scaled, sizeof(uint32_t));
    int f32_exp = (int)((fbits >> 23) & 0xFF) - 127;
    uint32_t f32_man = fbits & 0x7FFFFF;

    int fp8_exp = f32_exp + fmt.exp_bias;
    int max_exp = (1 << fmt.exp_bits) - 1;

    // E4M3FN: all-1s exponent is valid for normal values;
    //         NaN is only at (exp=max, man=all-1s)
    // E5M2:   all-1s exponent is reserved for inf/NaN (IEEE-like)
    bool nan_only_at_max_man = (fmt.man_bits >= 3);  // E4M3-style
    int max_normal_exp = nan_only_at_max_man ? max_exp : (max_exp - 1);
    int max_man_at_top = nan_only_at_max_man ? ((1 << fmt.man_bits) - 2)
                                             : ((1 << fmt.man_bits) - 1);

    if (fp8_exp <= 0) return (sign << 7);

    if (fp8_exp > max_normal_exp) {
        return (sign << 7) | (max_normal_exp << fmt.man_bits) | max_man_at_top;
    }

    int shift = 23 - fmt.man_bits;
    uint32_t fp8_man = f32_man >> shift;
    uint32_t round_bit = (f32_man >> (shift - 1)) & 1;
    uint32_t sticky_mask = (1u << (shift - 1)) - 1;
    uint32_t sticky = f32_man & sticky_mask;

    if (round_bit) {
        if (sticky || (fp8_man & 1)) {
            fp8_man++;
            // Check mantissa overflow
            int max_man_here = (nan_only_at_max_man && fp8_exp == max_exp)
                               ? max_man_at_top : ((1 << fmt.man_bits) - 1);
            if ((int)fp8_man > max_man_here) {
                fp8_man = 0;
                fp8_exp++;
                if (fp8_exp > max_normal_exp) {
                    return (sign << 7) | (max_normal_exp << fmt.man_bits) | max_man_at_top;
                }
            }
        }
    }

    // Final safety: avoid NaN encoding for E4M3
    if (nan_only_at_max_man && fp8_exp == max_exp && (int)fp8_man > max_man_at_top)
        fp8_man = max_man_at_top;

    return (sign << 7) | (fp8_exp << fmt.man_bits) | (fp8_man & ((1 << fmt.man_bits) - 1));
}

float fp8_to_float(fp8_t value, float scale, const FP8Format& fmt) {
    uint8_t sign = (value >> 7) & 1;
    int max_exp = (1 << fmt.exp_bits) - 1;
    int exp_val = (value >> fmt.man_bits) & ((1 << fmt.exp_bits) - 1);
    int man_val = value & ((1 << fmt.man_bits) - 1);

    if (exp_val == 0 && man_val == 0) return sign ? -0.0f : 0.0f;

    // NaN detection: E4M3 = (max_exp, max_man), E5M2 = (max_exp, any)
    bool nan_only_at_max_man = (fmt.man_bits >= 3);
    if (nan_only_at_max_man) {
        if (exp_val == max_exp && man_val == ((1 << fmt.man_bits) - 1)) return 0.0f;
    } else {
        if (exp_val == max_exp) return 0.0f;
    }

    float mantissa;
    int unbiased_exp;

    if (exp_val == 0) {
        mantissa = (float)man_val / (float)(1 << fmt.man_bits);
        unbiased_exp = 1 - fmt.exp_bias;
    } else {
        mantissa = 1.0f + (float)man_val / (float)(1 << fmt.man_bits);
        unbiased_exp = exp_val - fmt.exp_bias;
    }

    float result = ldexpf(mantissa, unbiased_exp);
    if (sign) result = -result;
    return result * scale;
}

void quantize_tensor(const float* input, fp8_t* output, int N,
                     float scale, const FP8Format& fmt) {
    for (int i = 0; i < N; i++) output[i] = float_to_fp8(input[i], scale, fmt);
}

void dequantize_tensor(const fp8_t* input, float* output, int N,
                       float scale, const FP8Format& fmt) {
    for (int i = 0; i < N; i++) output[i] = fp8_to_float(input[i], scale, fmt);
}

float compute_quantization_error(const float* original, const float* reconstructed, int N) {
    float mse = 0.0f;
    for (int i = 0; i < N; i++) {
        float diff = original[i] - reconstructed[i];
        mse += diff * diff;
    }
    return mse / (float)N;
}


// ============================================================================
// Q08 -- All-Reduce Ring Algorithm
// ============================================================================

/*
 * Approach: Two phases of N-1 steps each. Phase 1 (reduce-scatter) sends
 * chunks around the ring, accumulating sums. Phase 2 (all-gather) broadcasts
 * fully-reduced chunks. Uses snapshot buffers to avoid race conditions.
 */

struct AllReduceStats {
    long long total_bytes_transferred;
    int num_steps;
};

AllReduceStats ring_allreduce(vector<vector<float>>& node_data, int num_nodes) {
    AllReduceStats stats = {0, 0};
    int V = (int)node_data[0].size();
    int chunk_size = V / num_nodes;

    // Phase 1: Reduce-Scatter
    for (int s = 0; s < num_nodes - 1; s++) {
        vector<vector<float>> send_bufs(num_nodes, vector<float>(chunk_size));
        for (int i = 0; i < num_nodes; i++) {
            int send_chunk = ((i - s) % num_nodes + num_nodes) % num_nodes;
            int offset = send_chunk * chunk_size;
            for (int e = 0; e < chunk_size; e++)
                send_bufs[i][e] = node_data[i][offset + e];
        }
        for (int i = 0; i < num_nodes; i++) {
            int recv_from = (i - 1 + num_nodes) % num_nodes;
            int recv_chunk = ((i - s - 1) % num_nodes + num_nodes) % num_nodes;
            int offset = recv_chunk * chunk_size;
            for (int e = 0; e < chunk_size; e++)
                node_data[i][offset + e] += send_bufs[recv_from][e];
        }
        stats.total_bytes_transferred += (long long)num_nodes * chunk_size * sizeof(float);
        stats.num_steps++;
    }

    // Phase 2: All-Gather
    for (int s = 0; s < num_nodes - 1; s++) {
        vector<vector<float>> send_bufs(num_nodes, vector<float>(chunk_size));
        for (int i = 0; i < num_nodes; i++) {
            int send_chunk = ((i - s + 1) % num_nodes + num_nodes) % num_nodes;
            int offset = send_chunk * chunk_size;
            for (int e = 0; e < chunk_size; e++)
                send_bufs[i][e] = node_data[i][offset + e];
        }
        for (int i = 0; i < num_nodes; i++) {
            int recv_from = (i - 1 + num_nodes) % num_nodes;
            int recv_chunk = ((i - s) % num_nodes + num_nodes) % num_nodes;
            int offset = recv_chunk * chunk_size;
            for (int e = 0; e < chunk_size; e++)
                node_data[i][offset + e] = send_bufs[recv_from][e];
        }
        stats.total_bytes_transferred += (long long)num_nodes * chunk_size * sizeof(float);
        stats.num_steps++;
    }

    return stats;
}


// ============================================================================
// Q09 -- Reduce-Scatter with Recursive Halving
// ============================================================================

/*
 * Approach: log2(N) steps. Each step pairs nodes separated by a halving stride.
 * Lower node keeps the reduced lower half, upper node keeps upper half.
 * After all steps each node has one fully-reduced chunk.
 */

struct ReduceScatterStats {
    long long total_bytes_transferred;
    int num_steps;
};

ReduceScatterStats recursive_halving_reduce_scatter(
    vector<vector<float>>& node_data, int num_nodes) {
    ReduceScatterStats stats = {0, 0};
    int V = (int)node_data[0].size();
    int num_steps_total = (int)(log2(num_nodes) + 0.5);
    int active_size = V;

    for (int step = 0; step < num_steps_total; step++) {
        int half = active_size / 2;
        int stride = num_nodes >> (step + 1);
        int group_size = num_nodes >> step;

        vector<vector<float>> snapshot(num_nodes);
        for (int i = 0; i < num_nodes; i++)
            snapshot[i] = node_data[i];

        for (int group_start = 0; group_start < num_nodes; group_start += group_size) {
            for (int j = 0; j < stride; j++) {
                int node_lo = group_start + j;
                int node_hi = group_start + j + stride;

                for (int e = 0; e < half; e++)
                    node_data[node_lo][e] = snapshot[node_lo][e] + snapshot[node_hi][e];
                for (int e = 0; e < half; e++)
                    node_data[node_hi][e] = snapshot[node_lo][half + e] + snapshot[node_hi][half + e];

                stats.total_bytes_transferred += 2LL * half * sizeof(float);
            }
        }

        active_size = half;
        stats.num_steps++;
    }

    return stats;
}

ReduceScatterStats ring_reduce_scatter(
    vector<vector<float>>& node_data, int num_nodes) {
    ReduceScatterStats stats = {0, 0};
    int V = (int)node_data[0].size();
    int chunk_size = V / num_nodes;

    for (int s = 0; s < num_nodes - 1; s++) {
        vector<vector<float>> send_bufs(num_nodes, vector<float>(chunk_size));
        for (int i = 0; i < num_nodes; i++) {
            int send_chunk = ((i - s) % num_nodes + num_nodes) % num_nodes;
            int offset = send_chunk * chunk_size;
            for (int e = 0; e < chunk_size; e++)
                send_bufs[i][e] = node_data[i][offset + e];
        }
        for (int i = 0; i < num_nodes; i++) {
            int recv_from = (i - 1 + num_nodes) % num_nodes;
            int recv_chunk = ((i - s - 1) % num_nodes + num_nodes) % num_nodes;
            int offset = recv_chunk * chunk_size;
            for (int e = 0; e < chunk_size; e++)
                node_data[i][offset + e] += send_bufs[recv_from][e];
        }
        stats.total_bytes_transferred += (long long)num_nodes * chunk_size * sizeof(float);
        stats.num_steps++;
    }

    // Move each node's fully-reduced chunk to front for output consistency
    for (int i = 0; i < num_nodes; i++) {
        int offset = i * chunk_size;
        vector<float> chunk(chunk_size);
        for (int e = 0; e < chunk_size; e++)
            chunk[e] = node_data[i][offset + e];
        for (int e = 0; e < chunk_size; e++)
            node_data[i][e] = chunk[e];
    }

    return stats;
}


// ============================================================================
// Q10 -- Roofline Model Analysis
// ============================================================================

/*
 * Approach: Roofline model classifies kernels based on arithmetic intensity (AI).
 * If AI < ridge_point (peak_compute / peak_bandwidth), the kernel is memory-bound.
 * Otherwise it's compute-bound.
 */

struct RooflineModel {
    double peak_compute_flops;
    double peak_bandwidth_bytes;

    RooflineModel(double compute, double bandwidth)
        : peak_compute_flops(compute), peak_bandwidth_bytes(bandwidth) {}

    double ridge_point() {
        return peak_compute_flops / peak_bandwidth_bytes;
    }

    double attainable_performance(double arithmetic_intensity) {
        return min(peak_compute_flops, peak_bandwidth_bytes * arithmetic_intensity);
    }

    bool is_memory_bound(double arithmetic_intensity) {
        return arithmetic_intensity < ridge_point();
    }

    double utilization(double arithmetic_intensity) {
        return attainable_performance(arithmetic_intensity) / peak_compute_flops;
    }
};

double ai_vector_add(int N) {
    return (double)N / (3.0 * N * sizeof(float));
}

double ai_matmul(int M, int K, int N) {
    double flops = 2.0 * M * N * K;
    double bytes = ((double)M * K + (double)K * N + (double)M * N) * sizeof(float);
    return flops / bytes;
}

double ai_reduction(int N) {
    return (double)(N - 1) / (((double)N + 1.0) * sizeof(float));
}


// ============================================================================
// Q11 -- Bitwise Data Packing
// ============================================================================

/*
 * Approach: Process bits MSB-first within each field. Each bit is placed at
 * buffer[bit_position/8] at bit offset (7 - bit_position%8), giving MSB-first
 * ordering within bytes.
 */

struct BitPacker {
    vector<uint8_t> buffer;
    int bit_position;

    BitPacker() : bit_position(0) { buffer.resize(256, 0); }

    void pack(uint32_t value, int num_bits) {
        for (int i = num_bits - 1; i >= 0; i--) {
            int bit = (value >> i) & 1;
            int byte_idx = bit_position / 8;
            int bit_offset = bit_position % 8;
            if (bit)
                buffer[byte_idx] |= (1 << (7 - bit_offset));
            else
                buffer[byte_idx] &= ~(1 << (7 - bit_offset));
            bit_position++;
        }
    }

    void reset_read() { bit_position = 0; }

    uint32_t unpack(int num_bits) {
        uint32_t result = 0;
        for (int i = 0; i < num_bits; i++) {
            int byte_idx = bit_position / 8;
            int bit_offset = bit_position % 8;
            int bit = (buffer[byte_idx] >> (7 - bit_offset)) & 1;
            result = (result << 1) | bit;
            bit_position++;
        }
        return result;
    }

    int get_total_bytes() { return (bit_position + 7) / 8; }
};


// ============================================================================
// Q12 -- DMA Double Buffering
// ============================================================================

/*
 * Approach: Single buffer = sequential load/compute/store per tile.
 * Double buffer overlaps load(N+1) with compute(N) with store(N-1),
 * taking max(load, compute, store) per overlapped iteration.
 * For 1 tile, no overlap is possible so it equals single buffer.
 */

const int LOAD_LATENCY = 100;
const int COMPUTE_LATENCY = 80;
const int STORE_LATENCY = 100;

struct DMAStats {
    long long total_cycles;
    long long compute_cycles;
    long long idle_cycles;
    int num_tiles;
};

DMAStats single_buffer_pipeline(int num_tiles) {
    DMAStats stats = {0, 0, 0, num_tiles};
    stats.total_cycles = (long long)num_tiles * (LOAD_LATENCY + COMPUTE_LATENCY + STORE_LATENCY);
    stats.compute_cycles = (long long)num_tiles * COMPUTE_LATENCY;
    stats.idle_cycles = stats.total_cycles - stats.compute_cycles;
    return stats;
}

DMAStats double_buffer_pipeline(int num_tiles) {
    DMAStats stats = {0, 0, 0, num_tiles};
    stats.compute_cycles = (long long)num_tiles * COMPUTE_LATENCY;

    if (num_tiles == 1) {
        stats.total_cycles = LOAD_LATENCY + COMPUTE_LATENCY + STORE_LATENCY;
    } else {
        long long overlap = max({LOAD_LATENCY, COMPUTE_LATENCY, STORE_LATENCY});
        stats.total_cycles = LOAD_LATENCY
                           + (long long)(num_tiles - 1) * overlap
                           + COMPUTE_LATENCY + STORE_LATENCY;
    }
    stats.idle_cycles = stats.total_cycles - stats.compute_cycles;
    return stats;
}

float compute_speedup(int num_tiles) {
    auto s = single_buffer_pipeline(num_tiles);
    auto d = double_buffer_pipeline(num_tiles);
    return (float)s.total_cycles / (float)d.total_cycles;
}

float compute_utilization(DMAStats stats) {
    return (float)stats.compute_cycles / (float)stats.total_cycles;
}


// ============================================================================
// Q13 -- Softmax Kernel with Numerical Stability
// ============================================================================

/*
 * Approach: Four variants with increasing sophistication.
 * Naive: direct exp (overflows). Stable: subtract max first.
 * Online: single pass with running max/sum correction.
 * Tiled: per-tile local max/sum combined with correction factors.
 */

void naive_softmax(const float* input, float* output, int N) {
    float sum = 0.0f;
    for (int i = 0; i < N; i++) {
        output[i] = expf(input[i]);
        sum += output[i];
    }
    for (int i = 0; i < N; i++) output[i] /= sum;
}

void stable_softmax(const float* input, float* output, int N) {
    float max_val = input[0];
    for (int i = 1; i < N; i++) max_val = max(max_val, input[i]);

    float sum = 0.0f;
    for (int i = 0; i < N; i++) {
        output[i] = expf(input[i] - max_val);
        sum += output[i];
    }
    for (int i = 0; i < N; i++) output[i] /= sum;
}

void online_softmax(const float* input, float* output, int N) {
    float max_val = -FLT_MAX;
    float sum_val = 0.0f;

    for (int i = 0; i < N; i++) {
        if (input[i] > max_val) {
            sum_val = sum_val * expf(max_val - input[i]) + 1.0f;
            max_val = input[i];
        } else {
            sum_val += expf(input[i] - max_val);
        }
    }

    for (int i = 0; i < N; i++)
        output[i] = expf(input[i] - max_val) / sum_val;
}

void tiled_softmax(const float* input, float* output, int N, int tile_size) {
    int num_tiles = (N + tile_size - 1) / tile_size;
    vector<float> tile_max(num_tiles), tile_sum(num_tiles);

    for (int t = 0; t < num_tiles; t++) {
        int start = t * tile_size;
        int end = min(start + tile_size, N);

        float local_max = input[start];
        for (int i = start + 1; i < end; i++) local_max = max(local_max, input[i]);
        tile_max[t] = local_max;

        float local_sum = 0.0f;
        for (int i = start; i < end; i++) local_sum += expf(input[i] - local_max);
        tile_sum[t] = local_sum;
    }

    float global_max = tile_max[0];
    for (int t = 1; t < num_tiles; t++) global_max = max(global_max, tile_max[t]);

    float global_sum = 0.0f;
    for (int t = 0; t < num_tiles; t++)
        global_sum += tile_sum[t] * expf(tile_max[t] - global_max);

    for (int i = 0; i < N; i++)
        output[i] = expf(input[i] - global_max) / global_sum;
}


// ============================================================================
// Q14 -- Scaled Dot-Product Attention
// ============================================================================

/*
 * Approach: Naive materializes full NxN attention matrix. Tiled processes
 * in blocks using online softmax correction — rescales accumulated output
 * when a new block's local max exceeds the running max. Peak memory drops
 * from O(N^2) to O(block_size^2).
 */

struct AttentionStats {
    long long peak_memory_bytes;
    long long flops;
};

void row_softmax(vector<float>& row, int len) {
    float max_val = -FLT_MAX;
    for (int i = 0; i < len; i++) max_val = max(max_val, row[i]);
    float sum = 0;
    for (int i = 0; i < len; i++) { row[i] = expf(row[i] - max_val); sum += row[i]; }
    for (int i = 0; i < len; i++) row[i] /= sum;
}

void naive_attention(const vector<vector<float>>& Q,
                     const vector<vector<float>>& K,
                     const vector<vector<float>>& V,
                     vector<vector<float>>& O,
                     int seq_len, int d_model,
                     bool causal,
                     AttentionStats& stats) {
    float scale = 1.0f / sqrtf((float)d_model);
    vector<vector<float>> S(seq_len, vector<float>(seq_len, 0.0f));
    stats.peak_memory_bytes = (long long)seq_len * seq_len * sizeof(float);

    for (int i = 0; i < seq_len; i++)
        for (int j = 0; j < seq_len; j++) {
            float dot = 0.0f;
            for (int k = 0; k < d_model; k++) dot += Q[i][k] * K[j][k];
            S[i][j] = dot * scale;
        }

    if (causal)
        for (int i = 0; i < seq_len; i++)
            for (int j = i + 1; j < seq_len; j++)
                S[i][j] = -FLT_MAX;

    for (int i = 0; i < seq_len; i++) row_softmax(S[i], seq_len);

    for (int i = 0; i < seq_len; i++)
        for (int j = 0; j < d_model; j++) {
            float val = 0.0f;
            for (int k = 0; k < seq_len; k++) val += S[i][k] * V[k][j];
            O[i][j] = val;
        }

    stats.flops = 4LL * seq_len * seq_len * d_model;
}

void tiled_attention(const vector<vector<float>>& Q,
                     const vector<vector<float>>& K,
                     const vector<vector<float>>& V,
                     vector<vector<float>>& O,
                     int seq_len, int d_model,
                     bool causal,
                     int block_size,
                     AttentionStats& stats) {
    float scale = 1.0f / sqrtf((float)d_model);
    int num_blocks = (seq_len + block_size - 1) / block_size;
    stats.peak_memory_bytes = (long long)block_size * block_size * sizeof(float);

    for (int i = 0; i < seq_len; i++)
        for (int j = 0; j < d_model; j++) O[i][j] = 0.0f;

    vector<float> row_max_v(seq_len, -FLT_MAX);
    vector<float> row_sum_v(seq_len, 0.0f);

    for (int bq = 0; bq < num_blocks; bq++) {
        int q_start = bq * block_size;
        int q_end = min(q_start + block_size, seq_len);

        for (int bkv = 0; bkv < num_blocks; bkv++) {
            int kv_start = bkv * block_size;
            int kv_end = min(kv_start + block_size, seq_len);
            int bq_size = q_end - q_start;
            int bkv_size = kv_end - kv_start;

            vector<vector<float>> S_block(bq_size, vector<float>(bkv_size, 0.0f));
            for (int qi = 0; qi < bq_size; qi++)
                for (int ki = 0; ki < bkv_size; ki++) {
                    float dot = 0.0f;
                    for (int d = 0; d < d_model; d++)
                        dot += Q[q_start + qi][d] * K[kv_start + ki][d];
                    S_block[qi][ki] = dot * scale;
                }

            if (causal)
                for (int qi = 0; qi < bq_size; qi++)
                    for (int ki = 0; ki < bkv_size; ki++)
                        if ((kv_start + ki) > (q_start + qi))
                            S_block[qi][ki] = -FLT_MAX;

            for (int qi = 0; qi < bq_size; qi++) {
                int gqi = q_start + qi;

                float local_max = -FLT_MAX;
                for (int ki = 0; ki < bkv_size; ki++)
                    local_max = max(local_max, S_block[qi][ki]);

                float prev_max = row_max_v[gqi];
                float new_max = max(prev_max, local_max);
                float correction = expf(prev_max - new_max);

                float local_sum = 0.0f;
                vector<float> p_row(bkv_size);
                for (int ki = 0; ki < bkv_size; ki++) {
                    p_row[ki] = expf(S_block[qi][ki] - new_max);
                    local_sum += p_row[ki];
                }

                float new_sum = row_sum_v[gqi] * correction + local_sum;

                for (int d = 0; d < d_model; d++) {
                    O[gqi][d] *= correction;
                    for (int ki = 0; ki < bkv_size; ki++)
                        O[gqi][d] += p_row[ki] * V[kv_start + ki][d];
                }

                row_max_v[gqi] = new_max;
                row_sum_v[gqi] = new_sum;
            }
        }
    }

    for (int i = 0; i < seq_len; i++)
        for (int d = 0; d < d_model; d++)
            O[i][d] /= row_sum_v[i];

    stats.flops = 4LL * seq_len * seq_len * d_model;
}


// ============================================================================
// Q15 -- KV Cache Manager
// ============================================================================

/*
 * Approach: Pre-allocate flat arrays keys[layer][head] of size max_seq*head_dim.
 * append() copies new K,V at current_pos offset. get_keys/values returns
 * pointer to the flat array with length = current_pos.
 */

struct KVCache {
    int num_layers;
    int num_heads;
    int head_dim;
    int max_seq_len;
    int current_pos;

    vector<vector<vector<float>>> keys;
    vector<vector<vector<float>>> values;

    KVCache(int num_layers, int num_heads, int head_dim, int max_seq_len)
        : num_layers(num_layers), num_heads(num_heads), head_dim(head_dim),
          max_seq_len(max_seq_len), current_pos(0) {
        keys.resize(num_layers);
        values.resize(num_layers);
        for (int l = 0; l < num_layers; l++) {
            keys[l].resize(num_heads);
            values[l].resize(num_heads);
            for (int h = 0; h < num_heads; h++) {
                keys[l][h].resize(max_seq_len * head_dim, 0.0f);
                values[l][h].resize(max_seq_len * head_dim, 0.0f);
            }
        }
    }

    bool append(const vector<vector<vector<float>>>& new_keys,
                const vector<vector<vector<float>>>& new_values) {
        if (current_pos >= max_seq_len) return false;
        for (int l = 0; l < num_layers; l++)
            for (int h = 0; h < num_heads; h++) {
                int offset = current_pos * head_dim;
                for (int d = 0; d < head_dim; d++) {
                    keys[l][h][offset + d] = new_keys[l][h][d];
                    values[l][h][offset + d] = new_values[l][h][d];
                }
            }
        current_pos++;
        return true;
    }

    const float* get_keys(int layer, int head, int& length) {
        length = current_pos;
        return keys[layer][head].data();
    }

    const float* get_values(int layer, int head, int& length) {
        length = current_pos;
        return values[layer][head].data();
    }

    long long memory_bytes() {
        return 2LL * num_layers * num_heads * max_seq_len * head_dim * (long long)sizeof(float);
    }

    void reset() { current_pos = 0; }
};

double compute_kv_cache_size_gb(int num_layers, int num_heads, int head_dim,
                                 int max_seq_len, int bytes_per_value = 2) {
    double total_bytes = 2.0 * num_layers * num_heads * head_dim
                       * max_seq_len * bytes_per_value;
    return total_bytes / (1024.0 * 1024.0 * 1024.0);
}


// ============================================================================
// Q16 -- XLA-Style Tile Scheduler
// ============================================================================

/*
 * Approach: Kahn's algorithm for topological sort. Greedy sequential scheduler
 * processes ops in topo order, checking SRAM constraints. Tiles are live from
 * production until all consumers complete; tiles with no consumers are freed
 * immediately.
 */

struct TileOp {
    int id;
    const char* name;
    int output_size;
    int compute_cycles;
    vector<int> deps;
    vector<int> consumers;
};

struct ScheduleEntry {
    int op_id;
    int start_cycle;
    int end_cycle;
};

struct TileScheduler {
    vector<TileOp> ops;
    int sram_capacity;

    TileScheduler(int sram_cap) : sram_capacity(sram_cap) {}

    int add_op(const char* name, int output_size, int compute_cycles,
               vector<int> deps = {}) {
        int id = (int)ops.size();
        TileOp op;
        op.id = id;
        op.name = name;
        op.output_size = output_size;
        op.compute_cycles = compute_cycles;
        op.deps = deps;
        ops.push_back(op);
        for (int dep_id : deps) ops[dep_id].consumers.push_back(id);
        return id;
    }

    vector<int> topological_sort() {
        int n = (int)ops.size();
        vector<int> in_degree(n, 0);
        for (int i = 0; i < n; i++) in_degree[i] = (int)ops[i].deps.size();

        queue<int> q;
        for (int i = 0; i < n; i++)
            if (in_degree[i] == 0) q.push(i);

        vector<int> order;
        while (!q.empty()) {
            int node = q.front(); q.pop();
            order.push_back(node);
            for (int consumer : ops[node].consumers)
                if (--in_degree[consumer] == 0) q.push(consumer);
        }
        return order;
    }

    vector<ScheduleEntry> schedule_greedy() {
        vector<int> order = topological_sort();
        int n = (int)ops.size();
        vector<ScheduleEntry> schedule;
        vector<int> end_times(n, 0);
        int current_cycle = 0;

        for (int op_id : order) {
            int earliest = 0;
            for (int dep : ops[op_id].deps)
                earliest = max(earliest, end_times[dep]);

            int start = max(earliest, current_cycle);

            auto compute_sram_at = [&](int cycle) -> int {
                int sram_used = 0;
                for (auto& entry : schedule) {
                    int eid = entry.op_id;
                    if (entry.end_cycle <= cycle) {
                        if (ops[eid].consumers.empty()) continue;
                        bool all_done = true;
                        for (int c : ops[eid].consumers) {
                            bool consumer_scheduled = false;
                            for (auto& se : schedule) {
                                if (se.op_id == c) {
                                    consumer_scheduled = true;
                                    if (se.end_cycle > cycle) all_done = false;
                                    break;
                                }
                            }
                            if (!consumer_scheduled) all_done = false;
                            if (!all_done) break;
                        }
                        if (!all_done) sram_used += ops[eid].output_size;
                    }
                }
                return sram_used;
            };

            int sram_needed = ops[op_id].output_size;
            while (compute_sram_at(start + ops[op_id].compute_cycles) + sram_needed > sram_capacity) {
                start++;
                if (start > 1000000) break;
            }

            int end = start + ops[op_id].compute_cycles;
            end_times[op_id] = end;
            schedule.push_back({op_id, start, end});
            current_cycle = end;
        }

        return schedule;
    }

    int compute_total_cycles(const vector<ScheduleEntry>& schedule) {
        int max_end = 0;
        for (auto& e : schedule) max_end = max(max_end, e.end_cycle);
        return max_end;
    }

    int peak_sram_usage(const vector<ScheduleEntry>& schedule) {
        if (schedule.empty()) return 0;
        int n = (int)ops.size();
        vector<int> op_end(n, 0);
        for (auto& e : schedule) op_end[e.op_id] = e.end_cycle;

        set<int> time_points;
        for (auto& e : schedule) time_points.insert(e.end_cycle);

        int peak = 0;
        for (int t : time_points) {
            int sram_used = 0;
            for (auto& e : schedule) {
                int eid = e.op_id;
                int tile_born = e.end_cycle;
                if (tile_born > t) continue;
                if (ops[eid].consumers.empty()) continue;
                int tile_dies = 0;
                for (int c : ops[eid].consumers) tile_dies = max(tile_dies, op_end[c]);
                if (t >= tile_born && t < tile_dies) sram_used += ops[eid].output_size;
            }
            peak = max(peak, sram_used);
        }
        return peak;
    }
};


// ============================================================================
// Q17 -- 2D Convolution with im2col
// ============================================================================

/*
 * Approach: im2col extracts input patches as rows of a matrix.
 * Convolution becomes matrix-vector multiply: output = col_matrix * kernel_flat.
 * This maps perfectly to systolic arrays / GEMM units on TPUs.
 */

int compute_output_dim(int input_dim, int kernel_dim, int stride, int padding) {
    return (input_dim + 2 * padding - kernel_dim) / stride + 1;
}

void naive_conv2d(const vector<vector<float>>& input,
                  const vector<vector<float>>& kernel,
                  vector<vector<float>>& output,
                  int H, int W, int KH, int KW,
                  int stride, int padding) {
    int OH = compute_output_dim(H, KH, stride, padding);
    int OW = compute_output_dim(W, KW, stride, padding);
    for (int oh = 0; oh < OH; oh++)
        for (int ow = 0; ow < OW; ow++) {
            float sum = 0.0f;
            for (int kh = 0; kh < KH; kh++)
                for (int kw = 0; kw < KW; kw++) {
                    int ih = oh * stride + kh - padding;
                    int iw = ow * stride + kw - padding;
                    if (ih >= 0 && ih < H && iw >= 0 && iw < W)
                        sum += input[ih][iw] * kernel[kh][kw];
                }
            output[oh][ow] = sum;
        }
}

void im2col(const vector<vector<float>>& input,
            int H, int W, int KH, int KW,
            int stride, int padding,
            vector<vector<float>>& col_matrix,
            int OH, int OW) {
    for (int oh = 0; oh < OH; oh++)
        for (int ow = 0; ow < OW; ow++) {
            int row = oh * OW + ow;
            for (int kh = 0; kh < KH; kh++)
                for (int kw = 0; kw < KW; kw++) {
                    int ih = oh * stride + kh - padding;
                    int iw = ow * stride + kw - padding;
                    int col = kh * KW + kw;
                    col_matrix[row][col] = (ih >= 0 && ih < H && iw >= 0 && iw < W)
                                           ? input[ih][iw] : 0.0f;
                }
        }
}

void im2col_conv2d(const vector<vector<float>>& input,
                   const vector<vector<float>>& kernel,
                   vector<vector<float>>& output,
                   int H, int W, int KH, int KW,
                   int stride, int padding) {
    int OH = compute_output_dim(H, KH, stride, padding);
    int OW = compute_output_dim(W, KW, stride, padding);
    int patch_size = KH * KW;

    vector<vector<float>> col_matrix(OH * OW, vector<float>(patch_size, 0.0f));
    im2col(input, H, W, KH, KW, stride, padding, col_matrix, OH, OW);

    vector<float> kernel_flat(patch_size);
    for (int kh = 0; kh < KH; kh++)
        for (int kw = 0; kw < KW; kw++)
            kernel_flat[kh * KW + kw] = kernel[kh][kw];

    for (int i = 0; i < OH * OW; i++) {
        float dot = 0.0f;
        for (int j = 0; j < patch_size; j++) dot += col_matrix[i][j] * kernel_flat[j];
        output[i / OW][i % OW] = dot;
    }
}


// ============================================================================
// Q18 -- VLIW Instruction Packing
// ============================================================================

/*
 * Approach: Detect RAW/WAW/WAR hazards via register set intersection.
 * can_pack_together checks unit availability + hazards + dep satisfaction.
 * Greedy scheduler iterates ready ops, packing as many as possible per word.
 */

enum FuncUnit { ALU = 0, MUL = 1, MEM = 2, BRANCH = 3, NUM_UNITS = 4 };

const char* unit_names[] = {"ALU", "MUL", "MEM", "BR "};

struct Operation {
    int id;
    const char* name;
    FuncUnit unit;
    vector<int> reads;
    vector<int> writes;
    vector<int> deps;
};

struct VLIWWord {
    int ops[4];
    VLIWWord() { ops[0] = ops[1] = ops[2] = ops[3] = -1; }
};

struct VLIWScheduler {
    vector<Operation> operations;

    int add_op(const char* name, FuncUnit unit,
               vector<int> reads, vector<int> writes,
               vector<int> deps = {}) {
        int id = (int)operations.size();
        operations.push_back({id, name, unit, reads, writes, deps});
        return id;
    }

    bool has_data_hazard(int op_a_id, int op_b_id) {
        const Operation& a = operations[op_a_id];
        const Operation& b = operations[op_b_id];

        for (int w : a.writes) for (int r : b.reads) if (w == r) return true;   // RAW
        for (int wa : a.writes) for (int wb : b.writes) if (wa == wb) return true; // WAW
        for (int r : a.reads) for (int w : b.writes) if (r == w) return true;   // WAR
        return false;
    }

    bool can_pack_together(const VLIWWord& word, int op_id,
                           const set<int>& scheduled_ops) {
        const Operation& op = operations[op_id];
        if (word.ops[op.unit] >= 0) return false;

        for (int u = 0; u < NUM_UNITS; u++)
            if (word.ops[u] >= 0)
                if (has_data_hazard(word.ops[u], op_id) || has_data_hazard(op_id, word.ops[u]))
                    return false;

        for (int dep : op.deps)
            if (scheduled_ops.find(dep) == scheduled_ops.end()) return false;

        return true;
    }

    vector<VLIWWord> schedule() {
        int n = (int)operations.size();
        if (n == 0) return {};

        set<int> scheduled_ops;
        vector<bool> is_scheduled(n, false);
        vector<VLIWWord> words;

        while ((int)scheduled_ops.size() < n) {
            VLIWWord word;
            vector<int> ready;
            for (int i = 0; i < n; i++) {
                if (is_scheduled[i]) continue;
                bool ok = true;
                for (int dep : operations[i].deps) if (!is_scheduled[dep]) { ok = false; break; }
                if (ok) ready.push_back(i);
            }

            for (int op_id : ready)
                if (can_pack_together(word, op_id, scheduled_ops))
                    word.ops[operations[op_id].unit] = op_id;

            for (int u = 0; u < NUM_UNITS; u++)
                if (word.ops[u] >= 0) {
                    scheduled_ops.insert(word.ops[u]);
                    is_scheduled[word.ops[u]] = true;
                }

            words.push_back(word);
        }
        return words;
    }

    float compute_ilp(const vector<VLIWWord>& words) {
        if (words.empty()) return 0.0f;
        return (float)operations.size() / (float)words.size();
    }

    int count_nops(const vector<VLIWWord>& words) {
        int count = 0;
        for (auto& w : words) for (int u = 0; u < NUM_UNITS; u++) if (w.ops[u] == -1) count++;
        return count;
    }

    void print_schedule(const vector<VLIWWord>& words) {
        printf("  VLIW Schedule:\n");
        printf("  %4s | %8s | %8s | %8s | %8s\n", "Word", "ALU", "MUL", "MEM", "BRANCH");
        printf("  %s\n", "-----+----------+----------+----------+---------");
        for (int i = 0; i < (int)words.size(); i++) {
            printf("  %4d |", i);
            for (int u = 0; u < NUM_UNITS; u++) {
                if (words[i].ops[u] >= 0) printf(" %8s |", operations[words[i].ops[u]].name);
                else printf(" %8s |", "NOP");
            }
            printf("\n");
        }
    }
};
