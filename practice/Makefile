# Makefile for Google Embedded SWE Interview Practice Questions
# Compile with: make all
# Run tests: make test
# Clean: make clean

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g -O2
LDFLAGS = -pthread

# Source files
SOURCES = $(wildcard q*.cpp)
TARGETS = $(SOURCES:.cpp=)

# Default target
.PHONY: all
all: $(TARGETS)
	@echo "Built all practice questions"

# Pattern rule for building executables
%: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

# Run all tests
.PHONY: test
test: all
	@echo "========================================"
	@echo "Running all practice question tests"
	@echo "========================================"
	@failed=0; \
	for target in $(TARGETS); do \
		echo ""; \
		echo "--- Testing $$target ---"; \
		./$$target || failed=$$((failed + 1)); \
	done; \
	echo ""; \
	echo "========================================"; \
	if [ $$failed -eq 0 ]; then \
		echo "All tests passed!"; \
	else \
		echo "$$failed test(s) had failures"; \
	fi

# Run specific question test (finds the full binary name from the short alias)
.PHONY: test-%
test-%:
	@src=$$(ls $*_*.cpp 2>/dev/null | head -1); \
	if [ -z "$$src" ]; then echo "No source found for $*"; exit 1; fi; \
	binary=$${src%.cpp}; \
	$(MAKE) $$binary && ./$$binary

# Individual targets for convenience
.PHONY: q01 q02 q03 q04 q05 q06 q07 q08 q09 q10
.PHONY: q11 q12 q13 q14 q15 q16 q17 q18 q19 q20
.PHONY: q21 q22 q23

q01: q01_unique_values
q02: q02_missing_number
q03: q03_count_distinct
q04: q04_swap_no_temp
q05: q05_reverse_bits
q06: q06_extract_bits
q07: q07_atomic_bit_ops
q08: q08_single_non_duplicate
q09: q09_two_non_duplicates
q10: q10_external_sort
q11: q11_ring_buffer
q12: q12_memory_pool
q13: q13_lru_cache
q14: q14_button_debounce
q15: q15_endianness
q16: q16_volatile_demo
q17: q17_struct_packing
q18: q18_data_logger
q19: q19_ota_update
q20: q20_task_scheduler
q21: q21_stack_overflow
q22: q22_memory_leak
q23: q23_hard_fault

# Clean build artifacts
.PHONY: clean
clean:
	rm -f $(TARGETS)
	rm -f *.o
	rm -f *.dSYM
	@echo "Cleaned build artifacts"

# Help target
.PHONY: help
help:
	@echo "Google Embedded SWE Interview Practice - Build System"
	@echo ""
	@echo "Usage:"
	@echo "  make all        - Build all practice questions"
	@echo "  make test       - Build and run all tests"
	@echo "  make q01        - Build question 1 (unique values)"
	@echo "  make test-q01   - Build and test question 1"
	@echo "  make clean      - Remove build artifacts"
	@echo ""
	@echo "Questions:"
	@echo "  q01 - Unique Values with Memory Constraint"
	@echo "  q02 - Find Missing Number (XOR)"
	@echo "  q03 - Count Distinct (Approximate)"
	@echo "  q04 - Swap Without Temp"
	@echo "  q05 - Reverse Bits"
	@echo "  q06 - Extract Bit Field"
	@echo "  q07 - Atomic Bit Operations"
	@echo "  q08 - Single Non-Duplicate"
	@echo "  q09 - Two Non-Duplicates"
	@echo "  q10 - External Merge Sort"
	@echo "  q11 - Ring Buffer"
	@echo "  q12 - Memory Pool"
	@echo "  q13 - LRU Cache"
	@echo "  q14 - Button Debounce"
	@echo "  q15 - Endianness Conversion"
	@echo "  q16 - Volatile Demo"
	@echo "  q17 - Struct Packing"
	@echo "  q18 - Data Logger"
	@echo "  q19 - OTA Update"
	@echo "  q20 - Task Scheduler"
	@echo "  q21 - Stack Overflow Detection"
	@echo "  q22 - Memory Leak Detection"
	@echo "  q23 - Hard Fault Analysis"

# Compiler info
.PHONY: info
info:
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Sources: $(SOURCES)"
	@echo "Targets: $(TARGETS)"
