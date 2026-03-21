# Interview Prep

Practice questions and solutions for two upcoming onsite interviews.

---

## Sections

### 1. `practice_anthropic/` — Anthropic TPU Kernel Engineer

Python-based questions covering TPU/accelerator architecture, kernel optimization, and ML systems.

**Run tests:**
```bash
cd practice_anthropic

# Run all 24 questions
python run_tests.py 1 24

# Run a single question
python run_tests.py 13 13

# Run a range
python run_tests.py 7 12
```

**Questions:**
| # | File | Topic |
|---|------|-------|
| 1 | `q01_systolic_matmul.py` | Systolic array matrix multiply |
| 2 | `q02_memory_hierarchy_sim.py` | HBM/SRAM memory hierarchy simulation |
| 3 | `q03_simd_vectorize.py` | SIMD vectorization |
| 4 | `q04_tiled_matmul.py` | Tiled matrix multiply (SRAM tiling) |
| 5 | `q05_operator_fusion.py` | Operator fusion |
| 6 | `q06_bfloat16_arithmetic.py` | bfloat16 arithmetic |
| 7 | `q07_fp8_quantization.py` | FP8 quantization |
| 8 | `q08_allreduce.py` | All-Reduce collective |
| 9 | `q09_reduce_scatter.py` | Reduce-Scatter collective |
| 10 | `q10_roofline_analysis.py` | Roofline model / arithmetic intensity |
| 11 | `q11_data_packing.py` | Bit-level data packing |
| 12 | `q12_dma_double_buffer.py` | DMA double buffering |
| 13 | `q13_softmax_kernel.py` | Softmax kernel (stable, online, tiled) |
| 14 | `q14_attention_mechanism.py` | Attention mechanism |
| 15 | `q15_kv_cache.py` | KV cache for LLM inference |
| 16 | `q16_xla_tile_scheduler.py` | XLA tile scheduler |
| 17 | `q17_conv2d_im2col.py` | Conv2D via im2col |
| 18 | `q18_vliw_instruction_pack.py` | VLIW instruction packing |
| 19 | `q19_mapreduce.py` | MapReduce |
| 20 | `q20_thread_pool_work_stealing.py` | Thread pool / work stealing |
| 21 | `q21_async_network_router.py` | Async network router |
| 22 | `q22_pipeline_parallelism.py` | Pipeline parallelism |
| 23 | `q23_memory_pool_allocator.py` | Memory pool allocator |
| 24 | `q24_gradient_accumulation.py` | Gradient accumulation (bf16) |

---

### 2. `practice_general/` — Google Embedded SWE L4

C++ questions covering embedded systems, bit manipulation, memory management, and low-level debugging.

**Build and run:**
```bash
cd practice_general/practice

make all          # build all 23 questions
make test         # build and run all tests
make q04          # build a single question
make test-q04     # build and run a single question
make clean        # remove binaries
make help         # show all targets
```

**Questions:**

| # | File | Topic | Category |
|---|------|-------|----------|
| 1 | `q01_unique_values.cpp` | Unique values with 512B RAM limit | Bitmap & Memory-Efficient Dedup |
| 2 | `q02_missing_number.cpp` | Find missing number using XOR | Bitmap & Memory-Efficient Dedup |
| 3 | `q03_count_distinct.cpp` | Approximate counting (HyperLogLog) | Bitmap & Memory-Efficient Dedup |
| 4 | `q04_swap_no_temp.cpp` | XOR swap with edge cases | Bit Manipulation |
| 5 | `q05_reverse_bits.cpp` | Bit reversal (3 approaches) | Bit Manipulation |
| 10 | `q10_external_sort.cpp` | External merge sort | Memory-Constrained Algorithms |
| 11 | `q11_ring_buffer.cpp` | Lock-free SPSC ring buffer | Memory-Constrained Algorithms |
| 12 | `q12_memory_pool.cpp` | Fixed-size block allocator | Memory-Constrained Algorithms |
| 13 | `q13_lru_cache.cpp` | O(1) LRU cache | Memory-Constrained Algorithms |
| 14 | `q14_button_debounce.cpp` | GPIO debounce algorithms | Hardware & Low-Level |
| 15 | `q15_endianness.cpp` | Byte order conversion | Hardware & Low-Level |
| 16 | `q16_volatile_demo.cpp` | Understanding volatile | Hardware & Low-Level |
| 17 | `q17_struct_packing.cpp` | Struct alignment & padding | Hardware & Low-Level |
| 18 | `q18_data_logger.cpp` | Flash data logger with wear leveling | System Design |
| 19 | `q19_ota_update.cpp` | OTA firmware update system | System Design |
| 20 | `q20_task_scheduler.cpp` | Priority-based task scheduler | System Design |
| 21 | `q21_stack_overflow.cpp` | Stack overflow detection | Debugging & Analysis |
| 22 | `q22_memory_leak.cpp` | Memory leak detection | Debugging & Analysis |
| 23 | `q23_hard_fault.cpp` | ARM Cortex-M fault analysis | Debugging & Analysis |

Also includes `practice_general/Document1_Practice_Questions.md`, `Document2_Solutions.md`, and `Document3_Behavioral_Questions_Answers.md` for conceptual/behavioral prep.
