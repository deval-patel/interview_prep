"""
TPU Kernel Engineer Practice — Reference Solutions (Python)

Complete, working implementations for all 24 practice questions.
Each solution matches the exact function signatures from the corresponding
practice file and is designed to pass all tests defined therein.

Topics covered:
  Q01: Systolic Array Matrix Multiply
  Q02: Memory Hierarchy Simulator (HBM / SRAM)
  Q03: SIMD Vector Operations
  Q04: Tiled Matrix Multiplication
  Q05: Operator Fusion (ReLU + Bias + MatMul)
  Q06: BFloat16 Arithmetic
  Q07: FP8 Quantization and Dequantization
  Q08: All-Reduce Ring Algorithm
  Q09: Reduce-Scatter with Recursive Halving
  Q10: Roofline Model Analysis
  Q11: Bitwise Data Packing
  Q12: DMA Double Buffering
  Q13: Softmax Kernel with Numerical Stability
  Q14: Scaled Dot-Product Attention
  Q15: KV Cache Manager
  Q16: XLA-Style Tile Scheduler
  Q17: 2D Convolution with im2col
  Q18: VLIW Instruction Packing
  Q19: MapReduce Framework
  Q20: Thread Pool with Work Stealing
  Q21: Async Network Message Router
  Q22: Pipeline Parallelism Simulator
  Q23: Memory Pool Allocator
  Q24: Gradient Accumulation & Mixed Precision Training
"""

import struct
import math
import heapq
import random
from collections import OrderedDict, deque


# ============================================================================
# Q01 -- Systolic Array Matrix Multiply
# ============================================================================

class PE:
    def __init__(self):
        self.weight = 0.0
        self.acc = 0.0
        self.in_left = 0.0
        self.in_top = 0.0
        self.out_right = 0.0
        self.out_bottom = 0.0
        self.has_input = False
        self.has_top = False


def naive_matmul(A, B, M, K, N, stats=None):
    C = [[0.0] * N for _ in range(M)]
    for i in range(M):
        for j in range(N):
            for k in range(K):
                if stats:
                    stats.hbm_loads += 2
                C[i][j] += A[i][k] * B[k][j]
                if stats:
                    stats.flops += 2
            if stats:
                stats.hbm_stores += 1
    return C


class SystolicArray:
    def __init__(self, K, N):
        self.K = K
        self.N = N
        self.grid = [[PE() for _ in range(N)] for _ in range(K)]

    def load_weights(self, B):
        for k in range(self.K):
            for n in range(self.N):
                self.grid[k][n].weight = B[k][n]
                self.grid[k][n].acc = 0.0
                self.grid[k][n].in_left = 0.0
                self.grid[k][n].in_top = 0.0
                self.grid[k][n].out_right = 0.0
                self.grid[k][n].out_bottom = 0.0
                self.grid[k][n].has_input = False
                self.grid[k][n].has_top = False

    def step(self, cycle_inputs, input_valid):
        # Double-buffer approach
        new_out_right = [[0.0] * self.N for _ in range(self.K)]
        new_out_bottom = [[0.0] * self.N for _ in range(self.K)]
        new_has_input = [[False] * self.N for _ in range(self.K)]
        new_has_top = [[False] * self.N for _ in range(self.K)]

        for k in range(self.K):
            for n in range(self.N):
                if n == 0:
                    left_val = cycle_inputs[k]
                    left_valid = input_valid[k]
                else:
                    left_val = self.grid[k][n - 1].out_right
                    left_valid = self.grid[k][n - 1].has_input

                if k == 0:
                    top_val = 0.0
                    top_valid = True
                else:
                    top_val = self.grid[k - 1][n].out_bottom
                    top_valid = self.grid[k - 1][n].has_top

                new_has_input[k][n] = left_valid
                new_has_top[k][n] = left_valid or top_valid

                if left_valid:
                    new_out_bottom[k][n] = top_val + self.grid[k][n].weight * left_val
                    new_out_right[k][n] = left_val
                else:
                    new_out_bottom[k][n] = top_val
                    new_out_right[k][n] = 0.0

        for k in range(self.K):
            for n in range(self.N):
                self.grid[k][n].out_right = new_out_right[k][n]
                self.grid[k][n].out_bottom = new_out_bottom[k][n]
                self.grid[k][n].has_input = new_has_input[k][n]
                self.grid[k][n].has_top = new_has_top[k][n]

    def get_output(self, col):
        return self.grid[self.K - 1][col].out_bottom


def systolic_matmul(A, B, M, K, N):
    C = [[0.0] * N for _ in range(M)]
    sa = SystolicArray(K, N)
    sa.load_weights(B)

    total_cycles = M + K + N - 2

    for t in range(total_cycles):
        inputs = [0.0] * K
        valid = [False] * K

        for k in range(K):
            m = t - k
            if 0 <= m < M:
                inputs[k] = A[m][k]
                valid[k] = True

        sa.step(inputs, valid)

        for n in range(N):
            m_out = t - (K - 1) - n
            if 0 <= m_out < M:
                C[m_out][n] = sa.get_output(n)

    return C


# ============================================================================
# Q02 -- Memory Hierarchy Simulator (HBM / SRAM)
# ============================================================================

class MemorySystem:
    HBM_LATENCY = 100
    SRAM_LATENCY = 1
    SRAM_CAPACITY = 1024

    def __init__(self):
        self.total_cycles = 0
        self.hbm_accesses = 0
        self.hbm_bytes = 0
        self.sram = [0.0] * self.SRAM_CAPACITY

    def reset(self):
        self.total_cycles = 0
        self.hbm_accesses = 0
        self.hbm_bytes = 0

    def load_tile(self, hbm_src, src_offset, sram_offset, count):
        for i in range(count):
            self.sram[sram_offset + i] = hbm_src[src_offset + i]
        self.total_cycles += self.HBM_LATENCY
        self.hbm_accesses += 1
        self.hbm_bytes += count * 4

    def store_tile(self, hbm_dst, dst_offset, sram_offset, count):
        for i in range(count):
            hbm_dst[dst_offset + i] = self.sram[sram_offset + i]
        self.total_cycles += self.HBM_LATENCY
        self.hbm_accesses += 1
        self.hbm_bytes += count * 4

    def sram_read(self, offset):
        self.total_cycles += self.SRAM_LATENCY
        return self.sram[offset]

    def sram_write(self, offset, val):
        self.total_cycles += self.SRAM_LATENCY
        self.sram[offset] = val


def vector_add_naive(mem, hbm_A, hbm_B, hbm_C, N):
    for i in range(N):
        mem.load_tile(hbm_A, i, 0, 1)
        mem.load_tile(hbm_B, i, 1, 1)
        a = mem.sram_read(0)
        b = mem.sram_read(1)
        mem.sram_write(2, a + b)
        mem.store_tile(hbm_C, i, 2, 1)


def vector_add_tiled(mem, hbm_A, hbm_B, hbm_C, N):
    tile_size = MemorySystem.SRAM_CAPACITY // 3

    for base in range(0, N, tile_size):
        count = min(tile_size, N - base)
        mem.load_tile(hbm_A, base, 0, count)
        mem.load_tile(hbm_B, base, tile_size, count)

        for i in range(count):
            a = mem.sram_read(i)
            b = mem.sram_read(tile_size + i)
            mem.sram_write(2 * tile_size + i, a + b)

        mem.store_tile(hbm_C, base, 2 * tile_size, count)


def compute_arithmetic_intensity(N):
    flops = float(N)
    bytes_transferred = 3.0 * N * 4  # sizeof(float) = 4
    return flops / bytes_transferred


# ============================================================================
# Q03 -- SIMD Vector Operations
# ============================================================================

VECTOR_WIDTH = 8


class SimdReg:
    def __init__(self):
        self.data = [0.0] * VECTOR_WIDTH


def simd_load(ptr, offset):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = ptr[offset + i]
    return r

def simd_store(ptr, offset, r):
    for i in range(VECTOR_WIDTH):
        ptr[offset + i] = r.data[i]

def simd_add(a, b):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = a.data[i] + b.data[i]
    return r

def simd_mul(a, b):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = a.data[i] * b.data[i]
    return r

def simd_max(a, b):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = max(a.data[i], b.data[i])
    return r

def simd_set1(val):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = val
    return r

def simd_setzero():
    return simd_set1(0.0)

def simd_hsum(r):
    return sum(r.data)

def simd_fmadd(a, b, c):
    r = SimdReg()
    for i in range(VECTOR_WIDTH):
        r.data[i] = a.data[i] * b.data[i] + c.data[i]
    return r


def simd_vector_add(A, B, C, N):
    i = 0
    while i + VECTOR_WIDTH <= N:
        simd_store(C, i, simd_add(simd_load(A, i), simd_load(B, i)))
        i += VECTOR_WIDTH
    while i < N:
        C[i] = A[i] + B[i]
        i += 1


def simd_dot_product(A, B, N):
    acc = simd_setzero()
    i = 0
    while i + VECTOR_WIDTH <= N:
        acc = simd_add(acc, simd_mul(simd_load(A, i), simd_load(B, i)))
        i += VECTOR_WIDTH
    result = simd_hsum(acc)
    while i < N:
        result += A[i] * B[i]
        i += 1
    return result


def simd_relu(inp, out, N):
    zero = simd_set1(0.0)
    i = 0
    while i + VECTOR_WIDTH <= N:
        simd_store(out, i, simd_max(simd_load(inp, i), zero))
        i += VECTOR_WIDTH
    while i < N:
        out[i] = max(0.0, inp[i])
        i += 1


def simd_fma(A, B, C, out, N):
    i = 0
    while i + VECTOR_WIDTH <= N:
        simd_store(out, i, simd_fmadd(simd_load(A, i), simd_load(B, i), simd_load(C, i)))
        i += VECTOR_WIDTH
    while i < N:
        out[i] = A[i] * B[i] + C[i]
        i += 1


# ============================================================================
# Q04 -- Tiled Matrix Multiplication
# ============================================================================

TILE_SIZE = 16


class MatmulStats:
    def __init__(self):
        self.hbm_loads = 0
        self.hbm_stores = 0
        self.flops = 0


def naive_matmul_q04(A, B, M, K, N, stats):
    C = [[0.0] * N for _ in range(M)]
    stats.hbm_loads = 0
    stats.hbm_stores = 0
    stats.flops = 0
    for i in range(M):
        for j in range(N):
            for k in range(K):
                stats.hbm_loads += 2
                C[i][j] += A[i][k] * B[k][j]
                stats.flops += 2
            stats.hbm_stores += 1
    return C


def tiled_matmul(A, B, M, K, N, stats):
    C = [[0.0] * N for _ in range(M)]
    stats.hbm_loads = 0
    stats.hbm_stores = 0
    stats.flops = 0

    for ti in range(0, M, TILE_SIZE):
        ti_end = min(ti + TILE_SIZE, M)
        for tj in range(0, N, TILE_SIZE):
            tj_end = min(tj + TILE_SIZE, N)
            C_tile = [[0.0] * TILE_SIZE for _ in range(TILE_SIZE)]

            for tk in range(0, K, TILE_SIZE):
                tk_end = min(tk + TILE_SIZE, K)

                # Load A tile
                A_tile = [[0.0] * TILE_SIZE for _ in range(TILE_SIZE)]
                for i in range(ti, ti_end):
                    for k in range(tk, tk_end):
                        A_tile[i - ti][k - tk] = A[i][k]
                        stats.hbm_loads += 1

                # Load B tile
                B_tile = [[0.0] * TILE_SIZE for _ in range(TILE_SIZE)]
                for k in range(tk, tk_end):
                    for j in range(tj, tj_end):
                        B_tile[k - tk][j - tj] = B[k][j]
                        stats.hbm_loads += 1

                # Compute
                for i in range(ti_end - ti):
                    for j in range(tj_end - tj):
                        for k in range(tk_end - tk):
                            C_tile[i][j] += A_tile[i][k] * B_tile[k][j]
                            stats.flops += 2

            # Store C tile
            for i in range(ti, ti_end):
                for j in range(tj, tj_end):
                    C[i][j] = C_tile[i - ti][j - tj]
                    stats.hbm_stores += 1

    return C


# ============================================================================
# Q05 -- Operator Fusion (ReLU + Bias + MatMul)
# ============================================================================

class FusionStats:
    def __init__(self):
        self.hbm_reads = 0
        self.hbm_writes = 0

    def total_hbm_bytes(self):
        return (self.hbm_reads + self.hbm_writes) * 4


def unfused_matmul_bias_relu(A, B, bias, M, K, N, stats):
    stats.hbm_reads = 0
    stats.hbm_writes = 0

    # Step 1: T1 = A * B
    T1 = [[0.0] * N for _ in range(M)]
    for i in range(M):
        for j in range(N):
            s = 0.0
            for k in range(K):
                s += A[i][k] * B[k][j]
                stats.hbm_reads += 2
            T1[i][j] = s
            stats.hbm_writes += 1

    # Step 2: T2 = T1 + bias
    T2 = [[0.0] * N for _ in range(M)]
    for i in range(M):
        for j in range(N):
            stats.hbm_reads += 2
            T2[i][j] = T1[i][j] + bias[j]
            stats.hbm_writes += 1

    # Step 3: Y = ReLU(T2)
    Y = [[0.0] * N for _ in range(M)]
    for i in range(M):
        for j in range(N):
            stats.hbm_reads += 1
            Y[i][j] = max(0.0, T2[i][j])
            stats.hbm_writes += 1

    return Y


def fused_matmul_bias_relu(A, B, bias, M, K, N, stats):
    stats.hbm_reads = 0
    stats.hbm_writes = 0
    Y = [[0.0] * N for _ in range(M)]
    for i in range(M):
        for j in range(N):
            s = 0.0
            for k in range(K):
                s += A[i][k] * B[k][j]
                stats.hbm_reads += 2
            s += bias[j]
            stats.hbm_reads += 1
            Y[i][j] = max(0.0, s)
            stats.hbm_writes += 1
    return Y


# ============================================================================
# Q06 -- BFloat16 Arithmetic
# ============================================================================

def float_to_bf16(f):
    bits = struct.unpack('>I', struct.pack('>f', f))[0]

    if math.isnan(f):
        result = (bits >> 16) & 0xFFFF
        if (result & 0x7F) == 0:
            result |= 0x0040
        return result

    round_bit = (bits >> 15) & 1
    sticky = bits & 0x7FFF
    upper = (bits >> 16) & 0xFFFF

    if round_bit and sticky:
        upper += 1
    elif round_bit and not sticky:
        if upper & 1:
            upper += 1

    return upper & 0xFFFF


def bf16_to_float(b):
    bits = (b & 0xFFFF) << 16
    return struct.unpack('>f', struct.pack('>I', bits))[0]


def bf16_add(a, b):
    return float_to_bf16(bf16_to_float(a) + bf16_to_float(b))


def bf16_mul(a, b):
    return float_to_bf16(bf16_to_float(a) * bf16_to_float(b))


def bf16_dot_product(a, b, N):
    acc = 0.0
    for i in range(N):
        acc += bf16_to_float(a[i]) * bf16_to_float(b[i])
    return acc


# ============================================================================
# Q07 -- FP8 Quantization and Dequantization
# ============================================================================

class FP8Format:
    def __init__(self, exp_bits, man_bits, exp_bias, max_val, name):
        self.exp_bits = exp_bits
        self.man_bits = man_bits
        self.exp_bias = exp_bias
        self.max_val = max_val
        self.name = name


E4M3 = FP8Format(4, 3, 7, 448.0, "E4M3")
E5M2 = FP8Format(5, 2, 15, 57344.0, "E5M2")


def compute_scale(data, fmt):
    max_abs = 0.0
    for v in data:
        a = abs(v)
        if a > max_abs:
            max_abs = a
    if max_abs == 0.0:
        return 1.0
    return max_abs / fmt.max_val


def float_to_fp8(value, scale, fmt):
    if scale == 0.0:
        return 0
    scaled = value / scale
    if scaled == 0.0:
        return 0

    sign = 0
    if scaled < 0.0:
        sign = 1
        scaled = -scaled
    if scaled > fmt.max_val:
        scaled = fmt.max_val

    fbits = struct.unpack('>I', struct.pack('>f', scaled))[0]
    f32_exp = ((fbits >> 23) & 0xFF) - 127
    f32_man = fbits & 0x7FFFFF

    fp8_exp = f32_exp + fmt.exp_bias
    max_exp = (1 << fmt.exp_bits) - 1

    nan_only_at_max_man = (fmt.man_bits >= 3)
    max_normal_exp = max_exp if nan_only_at_max_man else (max_exp - 1)
    max_man_at_top = ((1 << fmt.man_bits) - 2) if nan_only_at_max_man else ((1 << fmt.man_bits) - 1)

    if fp8_exp <= 0:
        return sign << 7

    if fp8_exp > max_normal_exp:
        return (sign << 7) | (max_normal_exp << fmt.man_bits) | max_man_at_top

    shift = 23 - fmt.man_bits
    fp8_man = f32_man >> shift
    round_bit = (f32_man >> (shift - 1)) & 1
    sticky_mask = (1 << (shift - 1)) - 1
    sticky = f32_man & sticky_mask

    if round_bit:
        if sticky or (fp8_man & 1):
            fp8_man += 1
            max_man_here = max_man_at_top if (nan_only_at_max_man and fp8_exp == max_exp) else ((1 << fmt.man_bits) - 1)
            if fp8_man > max_man_here:
                fp8_man = 0
                fp8_exp += 1
                if fp8_exp > max_normal_exp:
                    return (sign << 7) | (max_normal_exp << fmt.man_bits) | max_man_at_top

    if nan_only_at_max_man and fp8_exp == max_exp and fp8_man > max_man_at_top:
        fp8_man = max_man_at_top

    return (sign << 7) | (fp8_exp << fmt.man_bits) | (fp8_man & ((1 << fmt.man_bits) - 1))


def fp8_to_float(value, scale, fmt):
    sign = (value >> 7) & 1
    max_exp = (1 << fmt.exp_bits) - 1
    exp_val = (value >> fmt.man_bits) & ((1 << fmt.exp_bits) - 1)
    man_val = value & ((1 << fmt.man_bits) - 1)

    if exp_val == 0 and man_val == 0:
        return -0.0 if sign else 0.0

    nan_only_at_max_man = (fmt.man_bits >= 3)
    if nan_only_at_max_man:
        if exp_val == max_exp and man_val == ((1 << fmt.man_bits) - 1):
            return 0.0
    else:
        if exp_val == max_exp:
            return 0.0

    if exp_val == 0:
        mantissa = float(man_val) / float(1 << fmt.man_bits)
        unbiased_exp = 1 - fmt.exp_bias
    else:
        mantissa = 1.0 + float(man_val) / float(1 << fmt.man_bits)
        unbiased_exp = exp_val - fmt.exp_bias

    result = math.ldexp(mantissa, unbiased_exp)
    if sign:
        result = -result
    return result * scale


def quantize_tensor(input_data, scale, fmt):
    return [float_to_fp8(v, scale, fmt) for v in input_data]


def dequantize_tensor(input_data, scale, fmt):
    return [fp8_to_float(v, scale, fmt) for v in input_data]


def compute_quantization_error(original, reconstructed):
    N = len(original)
    mse = 0.0
    for i in range(N):
        diff = original[i] - reconstructed[i]
        mse += diff * diff
    return mse / N


# ============================================================================
# Q08 -- All-Reduce Ring Algorithm
# ============================================================================

class AllReduceStats:
    def __init__(self):
        self.total_bytes_transferred = 0
        self.num_steps = 0


def ring_allreduce(node_data, num_nodes):
    stats = AllReduceStats()
    V = len(node_data[0])
    chunk_size = V // num_nodes

    # Phase 1: Reduce-Scatter
    for s in range(num_nodes - 1):
        send_bufs = [[0.0] * chunk_size for _ in range(num_nodes)]
        for i in range(num_nodes):
            send_chunk = (i - s) % num_nodes
            offset = send_chunk * chunk_size
            for e in range(chunk_size):
                send_bufs[i][e] = node_data[i][offset + e]
        for i in range(num_nodes):
            recv_from = (i - 1 + num_nodes) % num_nodes
            recv_chunk = (i - s - 1) % num_nodes
            offset = recv_chunk * chunk_size
            for e in range(chunk_size):
                node_data[i][offset + e] += send_bufs[recv_from][e]
        stats.total_bytes_transferred += num_nodes * chunk_size * 4
        stats.num_steps += 1

    # Phase 2: All-Gather
    for s in range(num_nodes - 1):
        send_bufs = [[0.0] * chunk_size for _ in range(num_nodes)]
        for i in range(num_nodes):
            send_chunk = (i - s + 1) % num_nodes
            offset = send_chunk * chunk_size
            for e in range(chunk_size):
                send_bufs[i][e] = node_data[i][offset + e]
        for i in range(num_nodes):
            recv_from = (i - 1 + num_nodes) % num_nodes
            recv_chunk = (i - s) % num_nodes
            offset = recv_chunk * chunk_size
            for e in range(chunk_size):
                node_data[i][offset + e] = send_bufs[recv_from][e]
        stats.total_bytes_transferred += num_nodes * chunk_size * 4
        stats.num_steps += 1

    return stats


# ============================================================================
# Q09 -- Reduce-Scatter with Recursive Halving
# ============================================================================

class ReduceScatterStats:
    def __init__(self):
        self.total_bytes_transferred = 0
        self.num_steps = 0


def recursive_halving_reduce_scatter(node_data, num_nodes):
    stats = ReduceScatterStats()
    V = len(node_data[0])
    num_steps_total = int(math.log2(num_nodes) + 0.5)
    active_size = V

    for step in range(num_steps_total):
        half = active_size // 2
        stride = num_nodes >> (step + 1)
        group_size = num_nodes >> step

        snapshot = [list(nd) for nd in node_data]

        for group_start in range(0, num_nodes, group_size):
            for j in range(stride):
                node_lo = group_start + j
                node_hi = group_start + j + stride

                for e in range(half):
                    node_data[node_lo][e] = snapshot[node_lo][e] + snapshot[node_hi][e]
                for e in range(half):
                    node_data[node_hi][e] = snapshot[node_lo][half + e] + snapshot[node_hi][half + e]

                stats.total_bytes_transferred += 2 * half * 4

        active_size = half
        stats.num_steps += 1

    return stats


def ring_reduce_scatter(node_data, num_nodes):
    stats = ReduceScatterStats()
    V = len(node_data[0])
    chunk_size = V // num_nodes

    for s in range(num_nodes - 1):
        send_bufs = [[0.0] * chunk_size for _ in range(num_nodes)]
        for i in range(num_nodes):
            send_chunk = (i - s) % num_nodes
            offset = send_chunk * chunk_size
            for e in range(chunk_size):
                send_bufs[i][e] = node_data[i][offset + e]
        for i in range(num_nodes):
            recv_from = (i - 1 + num_nodes) % num_nodes
            recv_chunk = (i - s - 1) % num_nodes
            offset = recv_chunk * chunk_size
            for e in range(chunk_size):
                node_data[i][offset + e] += send_bufs[recv_from][e]
        stats.total_bytes_transferred += num_nodes * chunk_size * 4
        stats.num_steps += 1

    # Move each node's fully-reduced chunk to front
    for i in range(num_nodes):
        offset = i * chunk_size
        chunk = node_data[i][offset:offset + chunk_size]
        for e in range(chunk_size):
            node_data[i][e] = chunk[e]

    return stats


# ============================================================================
# Q10 -- Roofline Model Analysis
# ============================================================================

class RooflineModel:
    def __init__(self, peak_compute_flops, peak_bandwidth_bytes):
        self.peak_compute_flops = peak_compute_flops
        self.peak_bandwidth_bytes = peak_bandwidth_bytes

    def ridge_point(self):
        return self.peak_compute_flops / self.peak_bandwidth_bytes

    def attainable_performance(self, arithmetic_intensity):
        return min(self.peak_compute_flops, self.peak_bandwidth_bytes * arithmetic_intensity)

    def is_memory_bound(self, arithmetic_intensity):
        return arithmetic_intensity < self.ridge_point()

    def utilization(self, arithmetic_intensity):
        return self.attainable_performance(arithmetic_intensity) / self.peak_compute_flops


def ai_vector_add(N):
    return float(N) / (3.0 * N * 4)


def ai_matmul(M, K, N):
    flops = 2.0 * M * N * K
    bytes_ = (float(M) * K + float(K) * N + float(M) * N) * 4
    return flops / bytes_


def ai_reduction(N):
    return float(N - 1) / ((float(N) + 1.0) * 4)


# ============================================================================
# Q11 -- Bitwise Data Packing
# ============================================================================

class BitPacker:
    def __init__(self):
        self.buffer = bytearray(256)
        self.bit_position = 0

    def pack(self, value, num_bits):
        for i in range(num_bits - 1, -1, -1):
            bit = (value >> i) & 1
            byte_idx = self.bit_position // 8
            bit_offset = self.bit_position % 8
            if bit:
                self.buffer[byte_idx] |= (1 << (7 - bit_offset))
            else:
                self.buffer[byte_idx] &= ~(1 << (7 - bit_offset))
            self.bit_position += 1

    def reset_read(self):
        self.bit_position = 0

    def unpack(self, num_bits):
        result = 0
        for _ in range(num_bits):
            byte_idx = self.bit_position // 8
            bit_offset = self.bit_position % 8
            bit = (self.buffer[byte_idx] >> (7 - bit_offset)) & 1
            result = (result << 1) | bit
            self.bit_position += 1
        return result

    def get_total_bytes(self):
        return (self.bit_position + 7) // 8


# ============================================================================
# Q12 -- DMA Double Buffering
# ============================================================================

LOAD_LATENCY = 100
COMPUTE_LATENCY = 80
STORE_LATENCY = 100


class DMAStats:
    def __init__(self):
        self.total_cycles = 0
        self.compute_cycles = 0
        self.idle_cycles = 0
        self.num_tiles = 0


def single_buffer_pipeline(num_tiles):
    stats = DMAStats()
    stats.num_tiles = num_tiles
    stats.total_cycles = num_tiles * (LOAD_LATENCY + COMPUTE_LATENCY + STORE_LATENCY)
    stats.compute_cycles = num_tiles * COMPUTE_LATENCY
    stats.idle_cycles = stats.total_cycles - stats.compute_cycles
    return stats


def double_buffer_pipeline(num_tiles):
    stats = DMAStats()
    stats.num_tiles = num_tiles
    stats.compute_cycles = num_tiles * COMPUTE_LATENCY

    if num_tiles == 1:
        stats.total_cycles = LOAD_LATENCY + COMPUTE_LATENCY + STORE_LATENCY
    else:
        overlap = max(LOAD_LATENCY, COMPUTE_LATENCY, STORE_LATENCY)
        stats.total_cycles = (LOAD_LATENCY
                              + (num_tiles - 1) * overlap
                              + COMPUTE_LATENCY + STORE_LATENCY)

    stats.idle_cycles = stats.total_cycles - stats.compute_cycles
    return stats


def compute_speedup(num_tiles):
    s = single_buffer_pipeline(num_tiles)
    d = double_buffer_pipeline(num_tiles)
    return s.total_cycles / d.total_cycles


def compute_utilization(stats):
    return stats.compute_cycles / stats.total_cycles


# ============================================================================
# Q13 -- Softmax Kernel with Numerical Stability
# ============================================================================

def naive_softmax(input_):
    N = len(input_)
    output = [0.0] * N
    s = 0.0
    for i in range(N):
        output[i] = math.exp(input_[i])
        s += output[i]
    for i in range(N):
        output[i] /= s
    return output


def stable_softmax(input_):
    N = len(input_)
    max_val = max(input_)
    output = [0.0] * N
    s = 0.0
    for i in range(N):
        output[i] = math.exp(input_[i] - max_val)
        s += output[i]
    for i in range(N):
        output[i] /= s
    return output


def online_softmax(input_):
    N = len(input_)
    max_val = float('-inf')
    sum_val = 0.0

    for i in range(N):
        if input_[i] > max_val:
            sum_val = sum_val * math.exp(max_val - input_[i]) + 1.0
            max_val = input_[i]
        else:
            sum_val += math.exp(input_[i] - max_val)

    output = [0.0] * N
    for i in range(N):
        output[i] = math.exp(input_[i] - max_val) / sum_val
    return output


def tiled_softmax(input_, tile_size):
    N = len(input_)
    num_tiles = (N + tile_size - 1) // tile_size
    tile_max = [0.0] * num_tiles
    tile_sum = [0.0] * num_tiles

    for t in range(num_tiles):
        start = t * tile_size
        end = min(start + tile_size, N)

        local_max = input_[start]
        for i in range(start + 1, end):
            local_max = max(local_max, input_[i])
        tile_max[t] = local_max

        local_sum = 0.0
        for i in range(start, end):
            local_sum += math.exp(input_[i] - local_max)
        tile_sum[t] = local_sum

    global_max = max(tile_max)

    global_sum = 0.0
    for t in range(num_tiles):
        global_sum += tile_sum[t] * math.exp(tile_max[t] - global_max)

    output = [0.0] * N
    for i in range(N):
        output[i] = math.exp(input_[i] - global_max) / global_sum
    return output


# ============================================================================
# Q14 -- Scaled Dot-Product Attention
# ============================================================================

class AttentionStats:
    def __init__(self):
        self.peak_memory_bytes = 0
        self.flops = 0


def row_softmax(row):
    max_val = max(row)
    exp_row = [math.exp(x - max_val) for x in row]
    s = sum(exp_row)
    return [x / s for x in exp_row]


def naive_attention(Q, K, V, seq_len, d_model, causal, stats):
    scale = 1.0 / math.sqrt(d_model)
    S = [[0.0] * seq_len for _ in range(seq_len)]
    stats.peak_memory_bytes = seq_len * seq_len * 4

    for i in range(seq_len):
        for j in range(seq_len):
            dot = 0.0
            for k in range(d_model):
                dot += Q[i][k] * K[j][k]
            S[i][j] = dot * scale

    if causal:
        for i in range(seq_len):
            for j in range(i + 1, seq_len):
                S[i][j] = float('-inf')

    for i in range(seq_len):
        S[i] = row_softmax(S[i])

    O = [[0.0] * d_model for _ in range(seq_len)]
    for i in range(seq_len):
        for j in range(d_model):
            val = 0.0
            for k in range(seq_len):
                val += S[i][k] * V[k][j]
            O[i][j] = val

    stats.flops = 4 * seq_len * seq_len * d_model
    return O


def tiled_attention(Q, K, V, seq_len, d_model, causal, block_size, stats):
    scale = 1.0 / math.sqrt(d_model)
    num_blocks = (seq_len + block_size - 1) // block_size
    stats.peak_memory_bytes = block_size * block_size * 4

    O = [[0.0] * d_model for _ in range(seq_len)]
    row_max_v = [float('-inf')] * seq_len
    row_sum_v = [0.0] * seq_len

    for bq in range(num_blocks):
        q_start = bq * block_size
        q_end = min(q_start + block_size, seq_len)

        for bkv in range(num_blocks):
            kv_start = bkv * block_size
            kv_end = min(kv_start + block_size, seq_len)
            bq_size = q_end - q_start
            bkv_size = kv_end - kv_start

            S_block = [[0.0] * bkv_size for _ in range(bq_size)]
            for qi in range(bq_size):
                for ki in range(bkv_size):
                    dot = 0.0
                    for d in range(d_model):
                        dot += Q[q_start + qi][d] * K[kv_start + ki][d]
                    S_block[qi][ki] = dot * scale

            if causal:
                for qi in range(bq_size):
                    for ki in range(bkv_size):
                        if (kv_start + ki) > (q_start + qi):
                            S_block[qi][ki] = float('-inf')

            for qi in range(bq_size):
                gqi = q_start + qi

                local_max = max(S_block[qi])
                prev_max = row_max_v[gqi]
                new_max = max(prev_max, local_max)
                correction = math.exp(prev_max - new_max) if prev_max != float('-inf') else 0.0

                p_row = [0.0] * bkv_size
                local_sum = 0.0
                for ki in range(bkv_size):
                    p_row[ki] = math.exp(S_block[qi][ki] - new_max)
                    local_sum += p_row[ki]

                new_sum = row_sum_v[gqi] * correction + local_sum

                for d in range(d_model):
                    O[gqi][d] *= correction
                    for ki in range(bkv_size):
                        O[gqi][d] += p_row[ki] * V[kv_start + ki][d]

                row_max_v[gqi] = new_max
                row_sum_v[gqi] = new_sum

    for i in range(seq_len):
        for d in range(d_model):
            O[i][d] /= row_sum_v[i]

    stats.flops = 4 * seq_len * seq_len * d_model
    return O


# ============================================================================
# Q15 -- KV Cache Manager
# ============================================================================

class KVCache:
    def __init__(self, num_layers, num_heads, head_dim, max_seq_len):
        self.num_layers = num_layers
        self.num_heads = num_heads
        self.head_dim = head_dim
        self.max_seq_len = max_seq_len
        self.current_pos = 0

        self.keys = [[[0.0] * (max_seq_len * head_dim)
                       for _ in range(num_heads)]
                      for _ in range(num_layers)]
        self.values = [[[0.0] * (max_seq_len * head_dim)
                         for _ in range(num_heads)]
                        for _ in range(num_layers)]

    def append(self, new_keys, new_values):
        if self.current_pos >= self.max_seq_len:
            return False
        for l in range(self.num_layers):
            for h in range(self.num_heads):
                offset = self.current_pos * self.head_dim
                for d in range(self.head_dim):
                    self.keys[l][h][offset + d] = new_keys[l][h][d]
                    self.values[l][h][offset + d] = new_values[l][h][d]
        self.current_pos += 1
        return True

    def get_keys(self, layer, head):
        return self.keys[layer][head], self.current_pos

    def get_values(self, layer, head):
        return self.values[layer][head], self.current_pos

    def memory_bytes(self):
        return 2 * self.num_layers * self.num_heads * self.max_seq_len * self.head_dim * 4

    def reset(self):
        self.current_pos = 0


def compute_kv_cache_size_gb(num_layers, num_heads, head_dim, max_seq_len, bytes_per_value=2):
    total_bytes = 2.0 * num_layers * num_heads * head_dim * max_seq_len * bytes_per_value
    return total_bytes / (1024.0 * 1024.0 * 1024.0)


# ============================================================================
# Q16 -- XLA-Style Tile Scheduler
# ============================================================================

class TileOp:
    def __init__(self, id_, name, output_size, compute_cycles, deps=None):
        self.id = id_
        self.name = name
        self.output_size = output_size
        self.compute_cycles = compute_cycles
        self.deps = deps or []
        self.consumers = []


class ScheduleEntry:
    def __init__(self, op_id, start_cycle, end_cycle):
        self.op_id = op_id
        self.start_cycle = start_cycle
        self.end_cycle = end_cycle


class TileScheduler:
    def __init__(self, sram_capacity):
        self.ops = []
        self.sram_capacity = sram_capacity

    def add_op(self, name, output_size, compute_cycles, deps=None):
        id_ = len(self.ops)
        op = TileOp(id_, name, output_size, compute_cycles, deps or [])
        self.ops.append(op)
        for dep_id in op.deps:
            self.ops[dep_id].consumers.append(id_)
        return id_

    def topological_sort(self):
        n = len(self.ops)
        in_degree = [len(self.ops[i].deps) for i in range(n)]
        q = deque()
        for i in range(n):
            if in_degree[i] == 0:
                q.append(i)
        order = []
        while q:
            node = q.popleft()
            order.append(node)
            for consumer in self.ops[node].consumers:
                in_degree[consumer] -= 1
                if in_degree[consumer] == 0:
                    q.append(consumer)
        return order

    def schedule_greedy(self):
        order = self.topological_sort()
        n = len(self.ops)
        schedule = []
        end_times = [0] * n
        current_cycle = 0

        for op_id in order:
            earliest = 0
            for dep in self.ops[op_id].deps:
                earliest = max(earliest, end_times[dep])

            start = max(earliest, current_cycle)

            def compute_sram_at(cycle):
                sram_used = 0
                for entry in schedule:
                    eid = entry.op_id
                    if entry.end_cycle <= cycle:
                        if not self.ops[eid].consumers:
                            continue
                        all_done = True
                        for c in self.ops[eid].consumers:
                            consumer_scheduled = False
                            for se in schedule:
                                if se.op_id == c:
                                    consumer_scheduled = True
                                    if se.end_cycle > cycle:
                                        all_done = False
                                    break
                            if not consumer_scheduled:
                                all_done = False
                            if not all_done:
                                break
                        if not all_done:
                            sram_used += self.ops[eid].output_size
                return sram_used

            sram_needed = self.ops[op_id].output_size
            while compute_sram_at(start + self.ops[op_id].compute_cycles) + sram_needed > self.sram_capacity:
                start += 1
                if start > 1000000:
                    break

            end = start + self.ops[op_id].compute_cycles
            end_times[op_id] = end
            schedule.append(ScheduleEntry(op_id, start, end))
            current_cycle = end

        return schedule

    def compute_total_cycles(self, schedule):
        return max((e.end_cycle for e in schedule), default=0)

    def peak_sram_usage(self, schedule):
        if not schedule:
            return 0
        n = len(self.ops)
        op_end = [0] * n
        for e in schedule:
            op_end[e.op_id] = e.end_cycle

        time_points = set()
        for e in schedule:
            time_points.add(e.end_cycle)

        peak = 0
        for t in time_points:
            sram_used = 0
            for e in schedule:
                eid = e.op_id
                tile_born = e.end_cycle
                if tile_born > t:
                    continue
                if not self.ops[eid].consumers:
                    continue
                tile_dies = max(op_end[c] for c in self.ops[eid].consumers)
                if t >= tile_born and t < tile_dies:
                    sram_used += self.ops[eid].output_size
            peak = max(peak, sram_used)
        return peak


# ============================================================================
# Q17 -- 2D Convolution with im2col
# ============================================================================

def compute_output_dim(input_dim, kernel_dim, stride, padding):
    return (input_dim + 2 * padding - kernel_dim) // stride + 1


def naive_conv2d(input_, kernel, H, W, KH, KW, stride, padding):
    OH = compute_output_dim(H, KH, stride, padding)
    OW = compute_output_dim(W, KW, stride, padding)
    output = [[0.0] * OW for _ in range(OH)]
    for oh in range(OH):
        for ow in range(OW):
            s = 0.0
            for kh in range(KH):
                for kw in range(KW):
                    ih = oh * stride + kh - padding
                    iw = ow * stride + kw - padding
                    if 0 <= ih < H and 0 <= iw < W:
                        s += input_[ih][iw] * kernel[kh][kw]
            output[oh][ow] = s
    return output


def im2col(input_, H, W, KH, KW, stride, padding, OH, OW):
    patch_size = KH * KW
    col_matrix = [[0.0] * patch_size for _ in range(OH * OW)]
    for oh in range(OH):
        for ow in range(OW):
            row = oh * OW + ow
            for kh in range(KH):
                for kw in range(KW):
                    ih = oh * stride + kh - padding
                    iw = ow * stride + kw - padding
                    col = kh * KW + kw
                    if 0 <= ih < H and 0 <= iw < W:
                        col_matrix[row][col] = input_[ih][iw]
                    else:
                        col_matrix[row][col] = 0.0
    return col_matrix


def im2col_conv2d(input_, kernel, H, W, KH, KW, stride, padding):
    OH = compute_output_dim(H, KH, stride, padding)
    OW = compute_output_dim(W, KW, stride, padding)
    patch_size = KH * KW

    col_matrix = im2col(input_, H, W, KH, KW, stride, padding, OH, OW)

    kernel_flat = [0.0] * patch_size
    for kh in range(KH):
        for kw in range(KW):
            kernel_flat[kh * KW + kw] = kernel[kh][kw]

    output = [[0.0] * OW for _ in range(OH)]
    for i in range(OH * OW):
        dot = 0.0
        for j in range(patch_size):
            dot += col_matrix[i][j] * kernel_flat[j]
        output[i // OW][i % OW] = dot
    return output


# ============================================================================
# Q18 -- VLIW Instruction Packing
# ============================================================================

ALU, MUL, MEM, BRANCH = 0, 1, 2, 3
NUM_UNITS = 4


class Operation:
    def __init__(self, id_, name, unit, reads, writes, deps=None):
        self.id = id_
        self.name = name
        self.unit = unit
        self.reads = reads
        self.writes = writes
        self.deps = deps or []


class VLIWWord:
    def __init__(self):
        self.ops = [-1] * NUM_UNITS


class VLIWScheduler:
    def __init__(self):
        self.operations = []

    def add_op(self, name, unit, reads, writes, deps=None):
        id_ = len(self.operations)
        self.operations.append(Operation(id_, name, unit, reads, writes, deps or []))
        return id_

    def has_data_hazard(self, op_a_id, op_b_id):
        a = self.operations[op_a_id]
        b = self.operations[op_b_id]
        # RAW
        for w in a.writes:
            for r in b.reads:
                if w == r:
                    return True
        # WAW
        for wa in a.writes:
            for wb in b.writes:
                if wa == wb:
                    return True
        # WAR
        for r in a.reads:
            for w in b.writes:
                if r == w:
                    return True
        return False

    def can_pack_together(self, word, op_id, scheduled_ops):
        op = self.operations[op_id]
        if word.ops[op.unit] >= 0:
            return False
        for u in range(NUM_UNITS):
            if word.ops[u] >= 0:
                if self.has_data_hazard(word.ops[u], op_id) or self.has_data_hazard(op_id, word.ops[u]):
                    return False
        for dep in op.deps:
            if dep not in scheduled_ops:
                return False
        return True

    def schedule(self):
        n = len(self.operations)
        if n == 0:
            return []
        scheduled_ops = set()
        is_scheduled = [False] * n
        words = []

        while len(scheduled_ops) < n:
            word = VLIWWord()
            ready = []
            for i in range(n):
                if is_scheduled[i]:
                    continue
                ok = True
                for dep in self.operations[i].deps:
                    if not is_scheduled[dep]:
                        ok = False
                        break
                if ok:
                    ready.append(i)

            for op_id in ready:
                if self.can_pack_together(word, op_id, scheduled_ops):
                    word.ops[self.operations[op_id].unit] = op_id

            for u in range(NUM_UNITS):
                if word.ops[u] >= 0:
                    scheduled_ops.add(word.ops[u])
                    is_scheduled[word.ops[u]] = True

            words.append(word)

        return words

    def compute_ilp(self, words):
        if not words:
            return 0.0
        return len(self.operations) / len(words)

    def count_nops(self, words):
        count = 0
        for w in words:
            for u in range(NUM_UNITS):
                if w.ops[u] == -1:
                    count += 1
        return count

    def print_schedule(self, words):
        unit_names = ["ALU", "MUL", "MEM", "BRANCH"]
        print("  VLIW Schedule:")
        header = f"  {'Word':>4} | " + " | ".join(f"{n:>8}" for n in unit_names)
        print(header)
        print("  " + "-" * (len(header) - 2))
        for idx, w in enumerate(words):
            slots = []
            for u in range(NUM_UNITS):
                if w.ops[u] >= 0:
                    slots.append(f"{self.operations[w.ops[u]].name:>8}")
                else:
                    slots.append(f"{'NOP':>8}")
            print(f"  {idx:>4} | " + " | ".join(slots))


# ============================================================================
# Q19 -- MapReduce Framework
# ============================================================================

def map_phase(input_data, mapper_fn):
    result = []
    for item in input_data:
        result.extend(mapper_fn(item))
    return result


def shuffle_phase(mapped_data):
    groups = OrderedDict()
    for key, value in mapped_data:
        if key not in groups:
            groups[key] = []
        groups[key].append(value)
    return groups


def reduce_phase(shuffled_data, reducer_fn):
    result = {}
    for key, values in shuffled_data.items():
        result[key] = reducer_fn(key, values)
    return result


def mapreduce(input_data, mapper_fn, reducer_fn):
    mapped = map_phase(input_data, mapper_fn)
    shuffled = shuffle_phase(mapped)
    return reduce_phase(shuffled, reducer_fn)


def word_count_mapper(text):
    return [(word, 1) for word in text.split()]


def word_count_reducer(key, values):
    return sum(values)


def inverted_index_mapper(doc_input):
    doc_id, text = doc_input.split(":", 1)
    words = set(text.split())
    return [(word, doc_id) for word in words]


def inverted_index_reducer(key, values):
    return values


def mean_mapper(entry):
    group, value = entry.split(":", 1)
    return [(group, float(value))]


def mean_reducer(key, values):
    return sum(values) / len(values)


# ============================================================================
# Q20 -- Thread Pool with Work Stealing
# ============================================================================

class Task:
    def __init__(self, task_id, duration, dependencies=None):
        self.id = task_id
        self.duration = duration
        self.dependencies = dependencies or []


class ScheduleEntryQ20:
    def __init__(self, task_id, worker_id, start_time, end_time):
        self.task_id = task_id
        self.worker_id = worker_id
        self.start_time = start_time
        self.end_time = end_time


class RunResult:
    def __init__(self):
        self.makespan = 0
        self.utilization = []
        self.num_steals = 0
        self.schedule = []


class ThreadPool:
    def __init__(self, num_workers):
        self.num_workers = num_workers
        self.queues = [deque() for _ in range(num_workers)]
        self.tasks = {}
        self.pending = []
        self.completed = set()
        self.rr_counter = 0

    def submit(self, task_id, duration, dependencies=None):
        task = Task(task_id, duration, dependencies)
        self.tasks[task_id] = task
        if not dependencies or all(d in self.completed for d in dependencies):
            worker = self.rr_counter % self.num_workers
            self.queues[worker].append(task)
            self.rr_counter += 1
        else:
            self.pending.append(task)

    def run(self):
        result = RunResult()
        result.utilization = [0.0] * self.num_workers

        current_time = 0
        worker_busy_time = [0] * self.num_workers
        # (end_time, worker_id, task_id) for running tasks
        running = [None] * self.num_workers  # (end_time, task)

        total_tasks = len(self.tasks)

        while len(self.completed) < total_tasks:
            # Check completions
            for w in range(self.num_workers):
                if running[w] is not None and running[w][0] <= current_time:
                    end_time, task = running[w]
                    self.completed.add(task.id)
                    result.schedule.append(ScheduleEntryQ20(
                        task.id, w, end_time - task.duration, end_time))
                    running[w] = None

            # Release pending tasks whose deps are met
            still_pending = []
            for task in self.pending:
                if all(d in self.completed for d in task.dependencies):
                    worker = self.rr_counter % self.num_workers
                    self.queues[worker].append(task)
                    self.rr_counter += 1
                else:
                    still_pending.append(task)
            self.pending = still_pending

            # Assign / steal for idle workers
            idle_workers = [w for w in range(self.num_workers)
                           if running[w] is None]
            # Sort: workers with empty queues first so they steal before
            # workers with tasks dequeue
            idle_workers.sort(key=lambda w: len(self.queues[w]))
            for w in idle_workers:
                if running[w] is not None:
                    continue
                if self.queues[w]:
                    task = self.queues[w].popleft()
                    running[w] = (current_time + task.duration, task)
                    worker_busy_time[w] += task.duration
                else:
                    # Work stealing: find worker with longest queue
                    best_victim = -1
                    best_len = 0
                    for v in range(self.num_workers):
                        if v == w:
                            continue
                        if len(self.queues[v]) > best_len:
                            best_len = len(self.queues[v])
                            best_victim = v
                    if best_victim >= 0 and best_len > 0:
                        task = self.queues[best_victim].pop()
                        running[w] = (current_time + task.duration, task)
                        worker_busy_time[w] += task.duration
                        result.num_steals += 1

            # Check if we need to advance time
            if len(self.completed) >= total_tasks:
                break

            # Find next event time
            next_time = float('inf')
            any_running = False
            for w in range(self.num_workers):
                if running[w] is not None:
                    any_running = True
                    next_time = min(next_time, running[w][0])

            if not any_running:
                # No one is running, but tasks remain — they must be pending
                # Advance by 1 to re-check
                current_time += 1
            else:
                current_time = next_time

        result.makespan = current_time
        if result.makespan > 0:
            result.utilization = [bt / result.makespan for bt in worker_busy_time]

        return result


# ============================================================================
# Q21 -- Async Network Message Router
# ============================================================================

class Message:
    def __init__(self, src, dst, content, send_time, arrive_time):
        self.src = src
        self.dst = dst
        self.content = content
        self.send_time = send_time
        self.arrive_time = arrive_time


class Network:
    def __init__(self, num_nodes, latency_matrix):
        self.num_nodes = num_nodes
        self.latency_matrix = latency_matrix
        self.current_time = 0
        self.inboxes = [[] for _ in range(num_nodes)]
        self.in_flight = []  # list of (arrive_time, send_order, Message)
        self.send_counter = 0

    def send(self, src, dst, message, timestamp):
        lat = self.latency_matrix[src][dst]
        if lat == 0:
            return
        arrive = timestamp + lat
        msg = Message(src, dst, message, timestamp, arrive)
        self.in_flight.append((arrive, self.send_counter, msg))
        self.send_counter += 1
        self.current_time = max(self.current_time, timestamp)

    def broadcast(self, src, message, timestamp):
        for dst in range(self.num_nodes):
            if dst != src:
                self.send(src, dst, message, timestamp)

    def step(self):
        self.current_time += 1
        delivered = []
        remaining = []
        # Sort in-flight by (arrive_time, send_order) for stable delivery
        self.in_flight.sort()
        for arrive_time, order, msg in self.in_flight:
            if arrive_time <= self.current_time:
                self.inboxes[msg.dst].append(msg)
                delivered.append(msg)
            else:
                remaining.append((arrive_time, order, msg))
        self.in_flight = remaining
        return delivered

    def get_inbox(self, node):
        return self.inboxes[node]

    def shortest_path_latency(self, src, dst):
        dist = [float('inf')] * self.num_nodes
        dist[src] = 0
        pq = [(0, src)]
        while pq:
            d, u = heapq.heappop(pq)
            if d > dist[u]:
                continue
            for v in range(self.num_nodes):
                lat = self.latency_matrix[u][v]
                if lat > 0 and dist[u] + lat < dist[v]:
                    dist[v] = dist[u] + lat
                    heapq.heappush(pq, (dist[v], v))
        return dist[dst] if dist[dst] != float('inf') else -1


# ============================================================================
# Q22 -- Pipeline Parallelism Simulator
# ============================================================================

def naive_execution(stage_latencies, num_microbatches):
    return num_microbatches * sum(stage_latencies)


class PipelineScheduler:
    def __init__(self, num_stages, num_microbatches, stage_latencies):
        self.num_stages = num_stages
        self.num_microbatches = num_microbatches
        self.stage_latencies = stage_latencies
        self.start_time = [[0] * num_microbatches for _ in range(num_stages)]
        self.finish_time = [[0] * num_microbatches for _ in range(num_stages)]

    def simulate(self):
        S = self.num_stages
        M = self.num_microbatches
        for s in range(S):
            for m in range(M):
                if s == 0 and m == 0:
                    self.start_time[s][m] = 0
                elif s == 0:
                    self.start_time[s][m] = self.finish_time[s][m - 1]
                elif m == 0:
                    self.start_time[s][m] = self.finish_time[s - 1][m]
                else:
                    self.start_time[s][m] = max(self.finish_time[s][m - 1],
                                                 self.finish_time[s - 1][m])
                self.finish_time[s][m] = self.start_time[s][m] + self.stage_latencies[s]

    def get_stats(self):
        S = self.num_stages
        M = self.num_microbatches
        total_time = self.finish_time[S - 1][M - 1]

        stage_busy = [0] * S
        stage_idle = [0] * S
        for s in range(S):
            stage_busy[s] = M * self.stage_latencies[s]
            stage_idle[s] = total_time - stage_busy[s]

        total_busy = sum(stage_busy)
        total_idle = S * total_time - total_busy
        bubble_ratio = total_idle / (S * total_time) if total_time > 0 else 0.0
        throughput = M / total_time if total_time > 0 else 0.0

        return {
            "total_time": total_time,
            "bubble_ratio": bubble_ratio,
            "throughput": throughput,
            "stage_busy_time": stage_busy,
            "stage_idle_time": stage_idle,
        }


# ============================================================================
# Q23 -- Memory Pool Allocator
# ============================================================================

class MemoryPool:
    def __init__(self, total_size):
        self.total_size = total_size
        self.free_list = [[0, total_size]]
        self.alloc_map = {}

    def alloc(self, size):
        if size <= 0:
            return -1
        for i, (offset, block_size) in enumerate(self.free_list):
            if block_size >= size:
                alloc_offset = offset
                self.alloc_map[alloc_offset] = size
                if block_size == size:
                    self.free_list.pop(i)
                else:
                    self.free_list[i] = [offset + size, block_size - size]
                return alloc_offset
        return -1

    def free(self, offset):
        if offset not in self.alloc_map:
            return
        size = self.alloc_map.pop(offset)
        # Insert back into free list sorted by offset
        inserted = False
        for i, (fo, _) in enumerate(self.free_list):
            if offset < fo:
                self.free_list.insert(i, [offset, size])
                inserted = True
                break
        if not inserted:
            self.free_list.append([offset, size])

    def coalesce(self):
        if len(self.free_list) <= 1:
            return
        merged = [self.free_list[0]]
        for i in range(1, len(self.free_list)):
            prev = merged[-1]
            curr = self.free_list[i]
            if prev[0] + prev[1] == curr[0]:
                merged[-1] = [prev[0], prev[1] + curr[1]]
            else:
                merged.append(curr)
        self.free_list = merged

    def fragmentation_ratio(self):
        if not self.free_list:
            return 0.0
        total_free = sum(s for _, s in self.free_list)
        if total_free == 0:
            return 0.0
        largest = max(s for _, s in self.free_list)
        if len(self.free_list) <= 1:
            return 0.0
        return 1.0 - largest / total_free

    def largest_free_block(self):
        if not self.free_list:
            return 0
        return max(s for _, s in self.free_list)

    def num_allocations(self):
        return len(self.alloc_map)


# ============================================================================
# Q24 -- Gradient Accumulation & Mixed Precision Training
# ============================================================================

def to_bf16(x):
    if math.isnan(x):
        return float('nan')
    _FP32_MAX = 3.4028235e+38
    if x > _FP32_MAX:
        return float('inf')
    if x < -_FP32_MAX:
        return float('-inf')
    bits = struct.unpack('>I', struct.pack('>f', x))[0]
    bits = bits & 0xFFFF0000
    return struct.unpack('>f', struct.pack('>I', bits))[0]


def has_overflow(values):
    for v in values:
        if math.isinf(v) or math.isnan(v):
            return True
    return False


def compute_mse_gradient_bf16(w, x, y, loss_scale):
    w_bf16 = to_bf16(w)
    N = len(x)
    grad = 0.0
    for i in range(N):
        pred = to_bf16(w_bf16 * x[i])
        diff = to_bf16(pred - y[i])
        grad += diff * x[i]
    grad = (2.0 / N) * grad
    return to_bf16(grad * loss_scale)


def mixed_precision_step(w_fp32, grad_scaled, loss_scale, lr):
    grad_fp32 = grad_scaled / loss_scale
    if has_overflow([grad_fp32]):
        return w_fp32, False
    w_fp32 -= lr * grad_fp32
    return w_fp32, True


def gradient_accumulation(w_init, data_x, data_y, loss_scale, lr, accum_steps):
    w = w_init
    num_batches = len(data_x)
    accumulated = 0.0
    step_count = 0

    for i in range(num_batches):
        grad_scaled = compute_mse_gradient_bf16(w, data_x[i], data_y[i], loss_scale)
        grad = grad_scaled / loss_scale
        if not has_overflow([grad]):
            accumulated += grad
        step_count += 1

        if step_count == accum_steps:
            avg_grad = accumulated / accum_steps
            if not has_overflow([avg_grad]):
                w -= lr * avg_grad
            accumulated = 0.0
            step_count = 0

    return w


def find_loss_scale(w, x, y, initial_scale, growth_interval=10):
    scale = initial_scale
    consecutive_ok = 0

    for i in range(len(x)):
        grad_scaled = compute_mse_gradient_bf16(w, [x[i]], [y[i]], scale)
        grad = grad_scaled / scale

        if has_overflow([grad]):
            scale /= 2.0
            consecutive_ok = 0
        else:
            consecutive_ok += 1
            if consecutive_ok >= growth_interval:
                scale *= 2.0
                consecutive_ok = 0

    return scale
