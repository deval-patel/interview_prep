"""
Q23 — Memory Pool Allocator [Medium]

Topic: Pure Programming — Systems

Problem:
  Implement a simple memory pool allocator that manages a fixed-size buffer.
  This is a common systems programming pattern used in performance-critical
  code where malloc/free overhead is unacceptable.

  Implement:
  1. MemoryPool(total_size) — initialize with a contiguous block
  2. alloc(size) — allocate a block, return offset (or -1 if no space).
     Use first-fit strategy.
  3. free(offset) — free a previously allocated block
  4. coalesce() — merge adjacent free blocks
  5. Track: fragmentation ratio, largest free block, number of allocations

Constraints:
  - All sizes and offsets are in bytes (ints for simulation)
  - First-fit allocation: scan free list, pick first block that fits
  - Free list is a list of (offset, size) pairs
  - Allocated blocks tracked separately for bookkeeping
  - alloc(0) should return -1 (invalid)

Key Concepts:
  - Memory allocation strategies (first-fit, best-fit, worst-fit)
  - External fragmentation
  - Free list coalescing
  - Pool allocators for real-time / embedded systems
"""


# ============ YOUR IMPLEMENTATION ============

class MemoryPool:
    """
    Fixed-size memory pool with first-fit allocation.

    Internal state:
      - free_list: sorted list of [offset, size] pairs for free regions
      - alloc_map: dict mapping offset -> size for allocated regions
      - total_size: total pool size

    Initially, the entire pool is one free block: [0, total_size].
    """

    def __init__(self, total_size: int):
        self.total_size = total_size
        self.free_list: list[list[int]] = []   # sorted by offset
        self.alloc_map: dict[int, int] = {}    # offset -> size
        # TODO: Initialize free list with one block spanning the entire pool

    def alloc(self, size: int) -> int:
        """
        Allocate 'size' bytes using first-fit strategy.

        Scan the free list for the first block with block_size >= size.
        If found:
          - Record the allocation in alloc_map
          - If the free block is exactly 'size', remove it from free list
          - If larger, shrink the free block (advance offset, reduce size)
          - Return the offset of the allocated block
        If not found, return -1.
        If size <= 0, return -1.
        """
        # TODO: Implement first-fit allocation
        return -1

    def free(self, offset: int):
        """
        Free the block at the given offset.

        Look up 'offset' in alloc_map to find the block size.
        Remove from alloc_map and add back to free_list (keep sorted by offset).
        Does nothing if offset is not found in alloc_map.
        """
        # TODO: Implement free
        pass

    def coalesce(self):
        """
        Merge adjacent free blocks.

        Scan the sorted free list. If free_list[i].offset + free_list[i].size
        equals free_list[i+1].offset, merge them into one block.
        Repeat until no more merges are possible.
        """
        # TODO: Implement coalescing of adjacent free blocks
        pass

    def fragmentation_ratio(self) -> float:
        """
        External fragmentation metric.

        fragmentation = 1 - (largest_free_block / total_free_space)
        Returns 0.0 if no free space or only one free block.
        """
        # TODO: Compute fragmentation
        return 0.0

    def largest_free_block(self) -> int:
        """Size of the largest contiguous free region."""
        # TODO: Return largest free block size, or 0 if none
        return 0

    def num_allocations(self) -> int:
        """Current number of active allocations."""
        return len(self.alloc_map)


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q23: Memory Pool Allocator ===\n")
    passed = 0
    total = 0

    # Test 1: Basic alloc returns valid offset
    total += 1
    pool = MemoryPool(1024)
    off = pool.alloc(256)
    if 0 <= off < 1024:
        print(f"PASS [Basic alloc returns valid offset: {off}]")
        passed += 1
    else:
        print(f"FAIL [Basic alloc returns valid offset: {off}]")

    # Test 2: Sequential allocs don't overlap
    total += 1
    pool = MemoryPool(1024)
    a = pool.alloc(256)
    b = pool.alloc(256)
    c = pool.alloc(256)
    valid = (a >= 0 and b >= 0 and c >= 0)
    no_overlap = (b >= a + 256) and (c >= b + 256)
    if valid and no_overlap:
        print(f"PASS [Sequential allocs don't overlap: a={a} b={b} c={c}]")
        passed += 1
    else:
        print(f"FAIL [Sequential allocs don't overlap: a={a} b={b} c={c}]")

    # Test 3: Free makes space available for reallocation
    total += 1
    pool = MemoryPool(512)
    a = pool.alloc(256)
    b = pool.alloc(256)
    fail1 = pool.alloc(1)  # Pool is full
    pool.free(a)
    c = pool.alloc(128)  # Should succeed in freed space
    if fail1 == -1 and c >= 0:
        print(f"PASS [Free makes space available: fail={fail1}, realloc={c}]")
        passed += 1
    else:
        print(f"FAIL [Free makes space available: fail={fail1}, realloc={c}]")

    # Test 4: Coalesce merges adjacent free blocks
    total += 1
    pool = MemoryPool(1024)
    a = pool.alloc(256)
    b = pool.alloc(256)
    c = pool.alloc(256)
    pool.free(a)
    pool.free(b)
    before = pool.largest_free_block()
    pool.coalesce()
    after = pool.largest_free_block()
    if before == 256 and after == 512:
        print(f"PASS [Coalesce merges: before={before}, after={after}]")
        passed += 1
    else:
        print(f"FAIL [Coalesce merges: before={before}, after={after}]")

    # Test 5: Allocation fails when pool is full
    total += 1
    pool = MemoryPool(512)
    pool.alloc(512)
    fail = pool.alloc(1)
    if fail == -1:
        print(f"PASS [Alloc fails when full: {fail}]")
        passed += 1
    else:
        print(f"FAIL [Alloc fails when full: {fail}]")

    # Test 6: Fragmentation after many alloc/free cycles
    total += 1
    pool = MemoryPool(1024)
    a = pool.alloc(256)
    b = pool.alloc(256)
    c = pool.alloc(256)
    d = pool.alloc(256)
    pool.free(a)
    pool.free(c)
    frag = pool.fragmentation_ratio()
    # Two free blocks of 256, total free = 512, largest = 256
    # fragmentation = 1 - 256/512 = 0.5
    if abs(frag - 0.5) < 0.01:
        print(f"PASS [Fragmentation ratio: {frag:.3f}, expected ~0.5]")
        passed += 1
    else:
        print(f"FAIL [Fragmentation ratio: {frag:.3f}, expected ~0.5]")

    # Test 7: alloc(0) edge case
    total += 1
    pool = MemoryPool(1024)
    off = pool.alloc(0)
    if off == -1:
        print(f"PASS [alloc(0) returns -1: {off}]")
        passed += 1
    else:
        print(f"FAIL [alloc(0) returns -1: {off}]")

    print(f"\nPassed {passed}/{total} tests")
