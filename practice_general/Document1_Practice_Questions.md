# Google Embedded Software Engineer Interview
# Practice Questions (Document 1 of 3)

**Prepared for:** Deval Patel  
**Interview Level:** L4 (aiming for L5)  
**Focus Areas:** Embedded C++, Algorithms, Bit Manipulation, Synchronization

---

## Section 1: Data Structures & Algorithms

*These questions test fundamental CS knowledge with embedded systems relevance.*

### Q1. LRU Cache Implementation

Design and implement a data structure for a Least Recently Used (LRU) cache with O(1) time complexity for both get and put operations. The cache should have a fixed capacity and evict the least recently used item when the capacity is exceeded.

### Q2. Find Missing Number

Given an array containing n distinct numbers in the range [0, n], find the one number that is missing from the array. Solve it in O(n) time and O(1) space.

### Q3. Merge K Sorted Lists

You are given an array of k linked-lists, each linked-list is sorted in ascending order. Merge all the linked-lists into one sorted linked-list and return it.

### Q4. Longest Consecutive Sequence

Given an unsorted array of integers, find the length of the longest consecutive elements sequence. Your algorithm should run in O(n) time complexity.

### Q5. Validate Binary Search Tree

Given the root of a binary tree, determine if it is a valid binary search tree (BST). A valid BST is defined as follows: (1) The left subtree contains only nodes with keys less than the node's key, (2) The right subtree contains only nodes with keys greater than the node's key, (3) Both left and right subtrees must also be binary search trees.

---

## Section 2: Bit Manipulation

*Critical for embedded systems - hardware register manipulation, flags, and performance optimization.*

### Q6. Count Set Bits (Population Count)

Write a function to count the number of 1 bits (set bits) in a 32-bit unsigned integer. Implement at least two different approaches and discuss their trade-offs.

### Q7. Check Power of Two

Determine if a given positive integer is a power of two using bit manipulation in O(1) time. Explain why your solution works.

### Q8. Single Number (XOR Application)

Given a non-empty array of integers where every element appears twice except for one, find that single element. Your solution must run in O(n) time and use O(1) space.

### Q9. Reverse Bits

Reverse the bits of a given 32-bit unsigned integer. For example, given 43261596 (00000010100101000001111010011100 in binary), return 964176192 (00111001011110000010100101000000 in binary).

### Q10. Extract Bits from Register

Write a function to extract bits from position 'p' to 'p+n-1' from a 32-bit integer. For example, extracting 3 bits starting at position 2 from 0b11010110 should return 0b101.

### Q11. Set/Clear/Toggle Specific Bits

Implement three functions: (1) Set the nth bit of an integer, (2) Clear the nth bit, (3) Toggle the nth bit. Explain common use cases in embedded systems for each operation.

### Q12. Find Position of Rightmost Set Bit

Write an efficient algorithm to find the position of the rightmost set bit (least significant bit that is 1) in a given integer. Return -1 if no bit is set.

---

## Section 3: Embedded Systems & Memory

### Q13. Implement a Simple Memory Allocator

Design a simple memory allocator with malloc() and free() functions. Discuss memory fragmentation, coalescing adjacent free blocks, and alignment requirements.

### Q14. Circular Buffer Implementation

Implement a circular buffer (ring buffer) with enqueue and dequeue operations. This is commonly used for UART buffers and data streaming. Handle the full and empty conditions correctly.

### Q15. Memory-Mapped I/O Register Access

You have a hardware register at memory address 0x40021000. Write code to: (1) Read the entire 32-bit register, (2) Set bit 5 without affecting other bits, (3) Clear bits 8-11 (4 bits) without affecting others. Use volatile correctly and explain why it's necessary.

### Q16. Aligned Memory Allocation

Implement `aligned_malloc(size_t size, size_t alignment)` and `aligned_free(void* ptr)` functions in C++. The `aligned_malloc` function should return a pointer to memory that is aligned to the specified alignment boundary. The alignment is guaranteed to be a power of 2. Your implementation should work with the standard `malloc`/`free` and handle the bookkeeping needed to properly free the aligned memory later.

---

## Section 4: Synchronization & Concurrency

### Q17. Producer-Consumer with Mutex

Implement a thread-safe producer-consumer queue using mutexes and condition variables. Multiple producers can add items, and multiple consumers can remove items. Ensure no race conditions or deadlocks.

### Q18. Reader-Writer Lock

Design a reader-writer lock where multiple readers can access a resource simultaneously, but writers require exclusive access. Explain how to prevent writer starvation.

### Q19. Detect Race Condition

Given the following code snippet, identify the race condition and explain how to fix it:

```c++
int counter = 0;
void increment() {
    counter++;
}
// Called by multiple threads
```

### Q20. Spinlock vs Mutex

Explain the difference between a spinlock and a mutex. When would you use each in an embedded system? Discuss scenarios where spinlocks are preferred over mutexes.

### Q21. Priority Inversion Problem

Explain priority inversion in RTOS. Describe a scenario where it occurs and two methods to prevent it (priority inheritance and priority ceiling).

---

## Section 5: Embedded Domain Questions

### Q22. Interrupt Service Routine (ISR) Design

What are the key principles for writing an ISR? Discuss: (1) ISR execution time constraints, (2) What should/shouldn't be done in an ISR, (3) How to communicate data between ISR and main loop safely.

### Q23. Volatile Keyword Usage

Explain when and why you need the 'volatile' keyword in embedded C/C++. Provide examples of: (1) Memory-mapped registers, (2) Variables modified by ISRs, (3) Multi-threaded shared variables.

### Q24. Endianness Conversion

Write functions to convert a 32-bit integer between big-endian and little-endian format. When is this necessary in embedded systems (e.g., network protocols, cross-platform data exchange)?

### Q25. Watchdog Timer

What is a watchdog timer and why is it used in embedded systems? How would you design a system to handle watchdog resets gracefully? Discuss the trade-off between watchdog timeout duration and system responsiveness.

### Q26. DMA (Direct Memory Access)

Explain how DMA works and its benefits in embedded systems. What are the cache coherency issues that can arise with DMA, and how do you address them?

---

## Section 6: System Design (Embedded Focus)

### Q27. Design a Low-Power Sensor Node

Design an embedded system for a battery-powered sensor that reads temperature every minute and transmits data to a gateway. Discuss: (1) Power management strategies, (2) Sleep modes, (3) Wake-up mechanisms, (4) Data buffering during offline periods.

### Q28. Design a Device Driver

Design a device driver for an I2C temperature sensor. Describe the architecture including: (1) Initialization sequence, (2) Read/write operations, (3) Error handling, (4) Interrupt vs polling for data ready, (5) Power management.

### Q29. Firmware Update Strategy

Design a safe over-the-air (OTA) firmware update mechanism for an embedded device. Address: (1) Dual-bank flash layout, (2) Rollback on failure, (3) Update verification, (4) Handling interrupted updates.

---

## Interview Tips

- Think out loud - explain your thought process
- Ask clarifying questions when assumptions are needed
- Start with a brute force approach, then optimize
- Consider edge cases and discuss time/space complexity
- Write actual code, not pseudocode (use C++ with STL as practiced)
- For embedded questions, mention hardware constraints, power, and real-time requirements
