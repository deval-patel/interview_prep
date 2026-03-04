# Google Embedded SWE L4 Interview Preparation

Practice questions and test suites for preparing for the Google Embedded Software Engineer L4 interview.

## Quick Start

```bash
cd practice

# Build all questions
make all

# Run all tests (most will fail initially - that's expected!)
make test

# Build and test a specific question
make q04
./q04_swap_no_temp
```

## Contents

```
interview_prep/
├── google_swe_1.cpp          # Your original interview question
├── practice/                 # Practice questions with test suites
│   ├── q01_unique_values.cpp
│   ├── q02_missing_number.cpp
│   ├── ...
│   ├── q23_hard_fault.cpp
│   └── Makefile
├── solutions/
│   └── solutions.cpp         # Reference implementations
└── README.md
```

## Questions by Category

### Category A: Bitmap & Memory-Efficient Deduplication
| # | File | Topic | Difficulty |
|---|------|-------|------------|
| 1 | `q01_unique_values.cpp` | Unique values with 512B RAM limit | Hard |
| 2 | `q02_missing_number.cpp` | Find missing number using XOR | Medium |
| 3 | `q03_count_distinct.cpp` | Approximate counting (HyperLogLog) | Hard |

### Category B: Bit Manipulation
| # | File | Topic | Difficulty |
|---|------|-------|------------|
| 4 | `q04_swap_no_temp.cpp` | XOR swap with edge cases | Medium |
| 5 | `q05_reverse_bits.cpp` | Bit reversal (3 approaches) | Medium |
| 6 | `q06_extract_bits.cpp` | Bit field operations | Medium |
| 7 | `q07_atomic_bit_ops.cpp` | Atomic register operations | Medium |
| 8 | `q08_single_non_duplicate.cpp` | Single unique (XOR) | Easy |
| 9 | `q09_two_non_duplicates.cpp` | Two uniques (partition XOR) | Hard |

### Category C: Memory-Constrained Algorithms
| # | File | Topic | Difficulty |
|---|------|-------|------------|
| 10 | `q10_external_sort.cpp` | External merge sort | Hard |
| 11 | `q11_ring_buffer.cpp` | Lock-free SPSC ring buffer | Medium |
| 12 | `q12_memory_pool.cpp` | Fixed-size block allocator | Medium |
| 13 | `q13_lru_cache.cpp` | O(1) LRU cache | Medium |

### Category D: Hardware & Low-Level
| # | File | Topic | Difficulty |
|---|------|-------|------------|
| 14 | `q14_button_debounce.cpp` | GPIO debounce algorithms | Medium |
| 15 | `q15_endianness.cpp` | Byte order conversion | Easy |
| 16 | `q16_volatile_demo.cpp` | Understanding volatile | Medium |
| 17 | `q17_struct_packing.cpp` | Struct alignment & padding | Medium |

### Category E: System Design (Embedded Focus)
| # | File | Topic | Difficulty |
|---|------|-------|------------|
| 18 | `q18_data_logger.cpp` | Flash data logger with wear leveling | Hard |
| 19 | `q19_ota_update.cpp` | OTA firmware update system | Hard |
| 20 | `q20_task_scheduler.cpp` | Priority-based task scheduler | Hard |

### Category F: Debugging & Analysis
| # | File | Topic | Difficulty |
|---|------|-------|------------|
| 21 | `q21_stack_overflow.cpp` | Stack overflow detection | Medium |
| 22 | `q22_memory_leak.cpp` | Memory leak detection | Medium |
| 23 | `q23_hard_fault.cpp` | ARM Cortex-M fault analysis | Medium |

## How to Use

### 1. Attempt the Problem
Each `.cpp` file contains:
- Problem description and constraints
- Function stubs to implement
- Test cases that will run against your implementation

```cpp
// Example from q04_swap_no_temp.cpp

// TODO: Implement this function
void swap_xor(int* a, int* b) {
    // Your implementation here
}
```

### 2. Run Tests
```bash
make q04            # Build
./q04_swap_no_temp  # Run tests
```

### 3. Check Solutions
After attempting, review `solutions/solutions.cpp` for reference implementations.

## Recommended Study Order

### Week 1-2: Bit Manipulation Fundamentals
1. Q4: Swap without temp
2. Q8: Single non-duplicate
3. Q5: Reverse bits
4. Q6: Extract bit field
5. Q9: Two non-duplicates
6. Q2: Missing number

### Week 3-4: Memory & Data Structures
1. Q11: Ring buffer
2. Q12: Memory pool
3. Q13: LRU cache
4. Q1: Unique values (memory constraint)
5. Q10: External sort

### Week 5-6: Embedded Systems
1. Q15: Endianness
2. Q16: Volatile
3. Q17: Struct packing
4. Q7: Atomic bit ops
5. Q14: Button debounce

### Week 7-8: System Design & Debugging
1. Q18: Data logger
2. Q19: OTA update
3. Q20: Task scheduler
4. Q21: Stack overflow
5. Q22: Memory leak
6. Q23: Hard fault

## Key Concepts to Master

### Bit Manipulation
- XOR properties: `a^a=0`, `a^0=a`
- Rightmost set bit: `n & (-n)`
- Mask creation: `((1 << width) - 1) << start`
- Power-of-2 modulo: `x & (n-1)` instead of `x % n`

### Memory Management
- Free list for O(1) pool allocation
- Head/tail indices for ring buffers
- Stack canaries and watermarks
- Alignment and padding rules

### Embedded Patterns
- `volatile` for hardware registers and ISR variables
- Critical sections with interrupt disable/enable
- Two-phase commit for power-safe operations
- A/B slot pattern for OTA updates

### Debugging
- ARM fault registers: CFSR, HFSR, BFAR, MMFAR
- Stacked PC shows faulting instruction
- Stack painting for usage analysis

## Interview Tips

1. **Clarify constraints** - Memory limit? Time limit? Real-time requirements?
2. **Think out loud** - Explain your approach before coding
3. **Start simple** - Get a working solution, then optimize
4. **Handle edge cases** - NULL, overflow, same-pointer, empty input
5. **Discuss trade-offs** - Space vs. time, complexity vs. maintainability

## Resources

- [Google L4 Interview Guide](https://www.hellointerview.com/guides/google/l4)
- [Bit Manipulation Questions](https://github.com/Devinterview-io/bit-manipulation-interview-questions)
- [Embedded C Interview Prep](https://tonyfu97.github.io/Embedded-C-Interview-Prep/)
- "Hacker's Delight" by Henry S. Warren (book)

Good luck with your interview!
