CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -g -O2 -pthread

# All question targets
TARGETS = \
	q01_systolic_matmul \
	q02_memory_hierarchy_sim \
	q03_simd_vectorize \
	q04_tiled_matmul \
	q05_operator_fusion \
	q06_bfloat16_arithmetic \
	q07_fp8_quantization \
	q08_allreduce \
	q09_reduce_scatter \
	q10_roofline_analysis \
	q11_data_packing \
	q12_dma_double_buffer \
	q13_softmax_kernel \
	q14_attention_mechanism \
	q15_kv_cache \
	q16_xla_tile_scheduler \
	q17_conv2d_im2col \
	q18_vliw_instruction_pack

.PHONY: all test clean help $(addprefix test-,$(TARGETS))

all: $(TARGETS)

# Individual build targets
q01_systolic_matmul: q01_systolic_matmul.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q02_memory_hierarchy_sim: q02_memory_hierarchy_sim.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q03_simd_vectorize: q03_simd_vectorize.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q04_tiled_matmul: q04_tiled_matmul.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q05_operator_fusion: q05_operator_fusion.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q06_bfloat16_arithmetic: q06_bfloat16_arithmetic.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q07_fp8_quantization: q07_fp8_quantization.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q08_allreduce: q08_allreduce.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q09_reduce_scatter: q09_reduce_scatter.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q10_roofline_analysis: q10_roofline_analysis.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q11_data_packing: q11_data_packing.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q12_dma_double_buffer: q12_dma_double_buffer.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q13_softmax_kernel: q13_softmax_kernel.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q14_attention_mechanism: q14_attention_mechanism.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q15_kv_cache: q15_kv_cache.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q16_xla_tile_scheduler: q16_xla_tile_scheduler.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q17_conv2d_im2col: q17_conv2d_im2col.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

q18_vliw_instruction_pack: q18_vliw_instruction_pack.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Run all tests
test: all
	@echo "========================================="
	@echo "Running all tests..."
	@echo "========================================="
	@for t in $(TARGETS); do \
		echo ""; \
		echo "----- Running $$t -----"; \
		./$$t; \
	done

# Individual test targets
test-q01_systolic_matmul: q01_systolic_matmul
	./q01_systolic_matmul

test-q02_memory_hierarchy_sim: q02_memory_hierarchy_sim
	./q02_memory_hierarchy_sim

test-q03_simd_vectorize: q03_simd_vectorize
	./q03_simd_vectorize

test-q04_tiled_matmul: q04_tiled_matmul
	./q04_tiled_matmul

test-q05_operator_fusion: q05_operator_fusion
	./q05_operator_fusion

test-q06_bfloat16_arithmetic: q06_bfloat16_arithmetic
	./q06_bfloat16_arithmetic

test-q07_fp8_quantization: q07_fp8_quantization
	./q07_fp8_quantization

test-q08_allreduce: q08_allreduce
	./q08_allreduce

test-q09_reduce_scatter: q09_reduce_scatter
	./q09_reduce_scatter

test-q10_roofline_analysis: q10_roofline_analysis
	./q10_roofline_analysis

test-q11_data_packing: q11_data_packing
	./q11_data_packing

test-q12_dma_double_buffer: q12_dma_double_buffer
	./q12_dma_double_buffer

test-q13_softmax_kernel: q13_softmax_kernel
	./q13_softmax_kernel

test-q14_attention_mechanism: q14_attention_mechanism
	./q14_attention_mechanism

test-q15_kv_cache: q15_kv_cache
	./q15_kv_cache

test-q16_xla_tile_scheduler: q16_xla_tile_scheduler
	./q16_xla_tile_scheduler

test-q17_conv2d_im2col: q17_conv2d_im2col
	./q17_conv2d_im2col

test-q18_vliw_instruction_pack: q18_vliw_instruction_pack
	./q18_vliw_instruction_pack

# Clean build artifacts
clean:
	rm -f $(TARGETS)
	rm -rf *.dSYM

# Help target
help:
	@echo "Available targets:"
	@echo "  all                       - Build all 18 questions"
	@echo "  test                      - Build and run all 18 questions"
	@echo "  clean                     - Remove all executables and dSYM dirs"
	@echo "  help                      - Print this help message"
	@echo ""
	@echo "Individual build targets:"
	@for t in $(TARGETS); do \
		echo "  $$t"; \
	done
	@echo ""
	@echo "Individual test targets:"
	@for t in $(TARGETS); do \
		echo "  test-$$t"; \
	done
