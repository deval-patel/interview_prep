# TPU Kernel Engineer — Interview Prep

## Project State (as of 2026-03-21)

### Directory layout
```
practice_anthropic/
  practice/        # 24 stub files (q01–q24) — fill in the TODOs to practice
  solutions/
    solutions.py   # Complete reference implementations for all 24 questions
  run_tests.py     # Test runner (see below)
  CLAUDE.md        # This file
```

### How to run tests

`run_tests.py` combines the solution implementations with each practice file's
test framework and runs them in a subprocess. It takes an inclusive question range:

```bash
# Run all 24 questions
python run_tests.py 1 24

# Run a single question
python run_tests.py 13 13

# Run a batch
python run_tests.py 7 12
```

To verify a practice attempt: copy your implementations into the corresponding
practice file, then run `run_tests.py`. The runner pulls the test harness from
the practice file and the reference implementation from solutions.py — so your
code is what gets tested.

### Solutions status
All 24 questions pass. The following bugs were found and fixed in `solutions.py`
during the 2026-03-21 session (do NOT revert these):

| Fix | Detail |
|-----|--------|
| `import random` added | Tests for Q04, Q07, Q14, Q17 use `random` |
| `stable_softmax(input_, N)` → `stable_softmax(input_)` | N param removed; use `len()` |
| `online_softmax(input_, N)` → `online_softmax(input_)` | same |
| `tiled_softmax(input_, N, tile_size)` → `tiled_softmax(input_, tile_size)` | same |
| Q01 `naive_matmul` — added `stats=None` | Q04 test passes a `MatmulStats` object |
| Q18 `VLIWScheduler.print_schedule` added | Method was missing from solution |
| Q24 `to_bf16` — overflow guard added | Values > fp32 max crashed `struct.pack` |
| Q24 `compute_mse_gradient_bf16` — wrap return in `to_bf16()` | So loss-scale overflow is detectable |

### Practice file adjustments (matching solutions)
These practice stubs were updated to match the corrected solution signatures:

| File | Change |
|------|--------|
| `q01_systolic_matmul.py` | `naive_matmul` now has `stats=None` param |
| `q10_roofline_analysis.py` | `ai_vector_add(n)` / `ai_reduction(n)` → uppercase `N` |
| `q13_softmax_kernel.py` | `input_arr` → `input_` in all four softmax stubs |
| `q17_conv2d_im2col.py` | `inp` → `input_` in `naive_conv2d`, `im2col`, `im2col_conv2d` |
| `q24_gradient_accumulation.py` | `to_bf16` docstring updated with overflow edge-case hint |

---

I am preparing for the following role: https://job-boards.greenhouse.io/anthropic/jobs/4720576008

I want to create cpp questions and solutions to practice for this upcoming onsite interview.

Preparing for a TPU Kernel Engineer role at a leading AI lab requires a specialized focus on the intersection of low-level systems programming and machine learning hardware. Given the technical nature of this position, the interview will likely dive deep into hardware architecture, performance optimization, and the software stack that bridges the two.

### 1. Hardware Architecture & Accelerators

Deep knowledge of how TPUs (Tensor Processing Units) and similar accelerators function is essential.

* **Systolic Arrays:** Understand how matrix multiplication is handled in hardware. Be prepared to discuss data flow, weight stationary vs. output stationary designs, and how these affect performance.
* **Memory Hierarchy:** Study the relationship between HBM (High Bandwidth Memory) and on-chip SRAM (like VMEM/CMEM). Focus on how to minimize data movement and maximize "arithmetic intensity."
* **VLIW & SIMD:** Many accelerator kernels rely on Very Long Instruction Word (VLIW) architectures and Single Instruction, Multiple Data (SIMD) operations. Be ready to explain how you would manually vectorize a loop or manage instruction-level parallelism.

### 2. Kernel Optimization Techniques

The core of the role involves writing high-performance code that squeezes every bit of utility out of the hardware.

* **Tiling & Fusion:** Practice explaining how to break large operations into "tiles" that fit into local SRAM. Discuss "operator fusion"—how to combine multiple operations (like a ReLU after a Conv) to save memory bandwidth.
* **Low-Precision Arithmetic:** The job description mentions low-precision inference. Refresh your knowledge of FP8, INT8, and bfloat16, specifically how quantization affects both hardware performance and numerical stability.
* **Collective Communications:** Understand how kernels scale across multiple chips. Review algorithms like All-Reduce, All-To-All, and Reduce-Scatter, particularly how they are implemented at the firmware or assembly level.

### 3. Systems Programming & Tooling

Since the work involves debugging at the assembly level and optimizing large-scale systems, focus on:

* **Performance Profiling:** Be ready to walk through your process for identifying bottlenecks. How do you distinguish between being memory-bound vs. compute-bound? Familiarize yourself with tools that look at instruction pipeline stalls or memory latency.
* **Bit-Level Manipulation:** This is critical for custom communication protocols and low-level data packing. Practice puzzles that involve efficient bitwise operations for data compression or flag management.
* **Assembly Debugging:** If you are comfortable with assembly, be prepared to discuss specific instances where you had to drop down from a high-level language to assembly to solve a performance regression or a race condition.

### 4. The ML Context

While the role is systems-heavy, the goal is to support ML research.

* **Compiler Infrastructure:** Review the basics of XLA (Accelerated Linear Algebra) and how ML frameworks like JAX or PyTorch lower their operations into kernels.
* **Transformer Architecture:** Understand the computational bottlenecks of transformers (e.g., the KV cache in LLM sampling). Think about how custom kernels could speed up the "Attention" mechanism.

### 5. Culture & Interview Format

Anthropic places a high value on collaborative engineering and their specific mission.

* **Pair Programming:** Since they explicitly mention loving pair programming, treat the coding sessions as a collaboration. Think out loud, be receptive to feedback, and explain the trade-offs of your approach (e.g., "This approach is faster but uses more SRAM; is that a trade-off we want to make here?").
* **Big Science & Impact:** Be prepared to discuss why you want to work on AI safety and reliability. Read their recent research on "Constitutional AI" or "Mechanistic Interpretability" to understand the types of models your kernels will be supporting.

### 6. Preparation Checklist

* **Review Computer Architecture:** Focus on cache coherence, memory controllers, and interconnects (like NVLink or ICI).
* **Practice Low-Level C/C++:** Focus on memory-mapped I/O, concurrency primitives, and pointer arithmetic, while keeping modern performance idioms in mind.
* **Whiteboard Kernel Logic:** Pick a simple operation (like a 2D convolution) and try to "pseudo-code" how you would implement it on a raw accelerator without high-level library support, focusing on memory tiles and loops.