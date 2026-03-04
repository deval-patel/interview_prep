# Google Embedded/Systems Interview - Practice Set 2
**Candidate:** Deval Patel | **Focus:** Systems, Kernel, & Modern C++ Infrastructure

---

## Section 1: Modern C++ for C Engineers
*Focus on RAII and resource management for hardware handles.*

### Q1. RAII Wrapper for Hardware Handles
You are working with a legacy C API that provides:
`HANDLE OpenDevice(int id);` and `void CloseDevice(HANDLE h);`.
Design a C++ class that wraps this `HANDLE` to ensure it is automatically closed when the object goes out of scope. 
* How do you prevent two objects from owning (and thus double-closing) the same handle?
* Implement the "Move" logic to transfer a handle from one object to another.

### Q2. Memory Alignment and Performance
[cite_start]In your experience with GPU/NPU drivers[cite: 8, 16], memory alignment is critical. 
* Write a snippet to allocate a buffer that is aligned to a 64-byte cache line boundary.
* Explain "False Sharing" in the context of a multi-core system and how you would fix it using `alignas`.

---

## Section 2: Kernel Internals & Architecture
[cite_start]*Based on your Core OS and Driver experience[cite: 5, 14].*

### Q3. TLB Management & Virtual Memory
* Describe the lifecycle of a Translation Lookaside Buffer (TLB) entry from creation to invalidation.
* In a multi-core environment, explain the "TLB Shootdown" process. Why is it a performance bottleneck?

### Q4. GPU/NPU Scheduling & Latency
[cite_start]Based on your work optimizing eNPU firmware for 2-3µs latency:
* How do you minimize context-switch overhead in a real-time environment?
* What are the architectural trade-offs between "Polling" a hardware status register vs. using "Interrupts"?

---

## Section 3: Concurrency & Memory Models

### Q5. C++ Memory Barriers (Acquire/Release)
* Explain the difference between `std::memory_order_relaxed` and `std::memory_order_acquire/release`.
* Provide a use case where `relaxed` is sufficient (e.g., performance counters) vs. where `acquire/release` is required (e.g., passing a pointer between threads).

### Q6. Lock-Free Design
Design a thread-safe, single-producer single-consumer (SPSC) queue using only atomic operations (no mutexes). 
* [cite_start]Why is this preferred for high-frequency logging systems like the one you designed for GPUView?

---

## Section 4: System Design

### Q7. High-Performance Telemetry System
[cite_start]Design a system to capture "packet-level" start/stop telemetry from a GPU driver without stalling the GPU scheduler. 
* How do you handle the data if the "Producer" (the driver) is faster than the "Consumer" (the logger)?