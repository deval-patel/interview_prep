/**
 * Q11: Thread-Safe Ring Buffer (Circular Buffer)
 *
 * PROBLEM:
 * Implement a fixed-size ring buffer (circular buffer) that is safe to use
 * between a producer thread/ISR and a consumer thread.
 *
 * Ring buffers are fundamental in embedded systems for:
 * - UART receive/transmit buffers
 * - Audio sample buffers
 * - Sensor data queuing
 * - Inter-process communication
 *
 * REQUIREMENTS:
 * - Fixed capacity (no dynamic allocation)
 * - FIFO ordering
 * - O(1) enqueue and dequeue
 * - Thread-safe for single producer, single consumer (SPSC)
 * - Handle full and empty states gracefully
 *
 * DESIGN CONSIDERATIONS:
 * - Head/tail pointers vs count variable
 * - Power-of-2 size for efficient modulo
 * - Lock-free vs mutex-based
 * - Volatile for memory visibility
 *
 * INTERVIEW DISCUSSION:
 * - Why is power-of-2 size efficient? (mask instead of modulo)
 * - Lock-free SPSC: why is it safe? (single writer per variable)
 * - Full vs empty detection with same head==tail state
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>

// ============ YOUR IMPLEMENTATION ============

/**
 * Fixed-size ring buffer template.
 *
 * CAPACITY must be a power of 2 for efficient modulo operations.
 */
template<typename T, size_t CAPACITY>
class RingBuffer {
    static_assert((CAPACITY & (CAPACITY - 1)) == 0,
                  "CAPACITY must be a power of 2");

private:
    T buffer[CAPACITY];

    // TODO: Add head/tail tracking variables
    // For SPSC lock-free, use std::atomic or volatile

public:
    RingBuffer() {
        // TODO: Initialize head and tail to 0
    }

    /**
     * Add an element to the buffer.
     *
     * @param item The item to enqueue
     * @return true if successful, false if buffer is full
     */
    bool enqueue(const T& item) {
        // TODO: Implement
        // 1. Check if full
        // 2. Write to buffer[head % CAPACITY] or buffer[head & (CAPACITY-1)]
        // 3. Increment head
        // 4. Return success

        return false;  // Placeholder
    }

    /**
     * Remove and return an element from the buffer.
     *
     * @param item Output parameter for dequeued item
     * @return true if successful, false if buffer is empty
     */
    bool dequeue(T* item) {
        // TODO: Implement
        // 1. Check if empty
        // 2. Read from buffer[tail % CAPACITY]
        // 3. Increment tail
        // 4. Return success

        return false;  // Placeholder
    }

    /**
     * Check if buffer is empty.
     */
    bool is_empty() const {
        // TODO: Implement
        // Empty when head == tail
        return true;  // Placeholder
    }

    /**
     * Check if buffer is full.
     */
    bool is_full() const {
        // TODO: Implement
        // Full when (head - tail) == CAPACITY
        return false;  // Placeholder
    }

    /**
     * Get current number of elements in buffer.
     */
    size_t size() const {
        // TODO: Implement
        return 0;  // Placeholder
    }

    /**
     * Get buffer capacity.
     */
    size_t capacity() const {
        return CAPACITY;
    }

    /**
     * Clear all elements.
     */
    void clear() {
        // TODO: Implement
    }

    /**
     * Peek at front element without removing.
     *
     * @param item Output parameter for front item
     * @return true if buffer is not empty
     */
    bool peek(T* item) const {
        // TODO: Implement
        return false;  // Placeholder
    }
};

/**
 * BONUS: Implement a ring buffer that can overwrite old data when full.
 *
 * Useful for logging systems where you want the most recent data.
 */
template<typename T, size_t CAPACITY>
class OverwritingRingBuffer {
    static_assert((CAPACITY & (CAPACITY - 1)) == 0,
                  "CAPACITY must be a power of 2");

private:
    T buffer[CAPACITY];
    // TODO: Add tracking variables

public:
    OverwritingRingBuffer() {
        // TODO: Initialize
    }

    /**
     * Add an element, overwriting oldest if full.
     * Always succeeds.
     */
    void enqueue(const T& item) {
        // TODO: Implement
    }

    bool dequeue(T* item) {
        // TODO: Implement
        return false;
    }

    size_t size() const {
        return 0;  // Placeholder
    }
};

// ============ TEST FRAMEWORK ============

bool test_basic_operations() {
    printf("--- Basic Operations ---\n");
    int passed = 0;
    int total = 0;

    RingBuffer<int, 8> rb;

    // Test empty state
    if (rb.is_empty() && !rb.is_full() && rb.size() == 0) {
        printf("PASS: Initial empty state\n");
        passed++;
    } else {
        printf("FAIL: Initial empty state\n");
    }
    total++;

    // Test single enqueue/dequeue
    if (rb.enqueue(42)) {
        int val;
        if (rb.dequeue(&val) && val == 42) {
            printf("PASS: Single enqueue/dequeue\n");
            passed++;
        } else {
            printf("FAIL: Single dequeue returned wrong value\n");
        }
    } else {
        printf("FAIL: Single enqueue failed\n");
    }
    total++;

    // Test fill to capacity
    rb.clear();
    bool fill_ok = true;
    for (int i = 0; i < 8; i++) {
        if (!rb.enqueue(i)) {
            fill_ok = false;
            break;
        }
    }
    if (fill_ok && rb.is_full() && rb.size() == 8) {
        printf("PASS: Fill to capacity\n");
        passed++;
    } else {
        printf("FAIL: Fill to capacity (full=%d, size=%zu)\n",
               rb.is_full(), rb.size());
    }
    total++;

    // Test enqueue when full (should fail)
    if (!rb.enqueue(999)) {
        printf("PASS: Enqueue fails when full\n");
        passed++;
    } else {
        printf("FAIL: Enqueue succeeded when full\n");
    }
    total++;

    // Test dequeue all
    bool dequeue_ok = true;
    for (int i = 0; i < 8; i++) {
        int val;
        if (!rb.dequeue(&val) || val != i) {
            dequeue_ok = false;
            break;
        }
    }
    if (dequeue_ok && rb.is_empty()) {
        printf("PASS: Dequeue all (FIFO order)\n");
        passed++;
    } else {
        printf("FAIL: Dequeue all\n");
    }
    total++;

    // Test dequeue when empty (should fail)
    int dummy;
    if (!rb.dequeue(&dummy)) {
        printf("PASS: Dequeue fails when empty\n");
        passed++;
    } else {
        printf("FAIL: Dequeue succeeded when empty\n");
    }
    total++;

    // Test wraparound
    rb.clear();
    for (int i = 0; i < 6; i++) rb.enqueue(i);
    for (int i = 0; i < 4; i++) { int v; rb.dequeue(&v); }
    for (int i = 6; i < 12; i++) rb.enqueue(i);

    bool wrap_ok = true;
    for (int i = 4; i < 12; i++) {
        int val;
        if (!rb.dequeue(&val) || val != i) {
            wrap_ok = false;
            printf("  Expected %d, got %d\n", i, val);
            break;
        }
    }
    if (wrap_ok && rb.is_empty()) {
        printf("PASS: Wraparound works correctly\n");
        passed++;
    } else {
        printf("FAIL: Wraparound\n");
    }
    total++;

    // Test peek
    rb.clear();
    rb.enqueue(100);
    rb.enqueue(200);
    int peeked;
    if (rb.peek(&peeked) && peeked == 100 && rb.size() == 2) {
        printf("PASS: Peek doesn't remove element\n");
        passed++;
    } else {
        printf("FAIL: Peek\n");
    }
    total++;

    printf("Basic tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_thread_safety() {
    printf("--- Thread Safety (SPSC) ---\n");

    RingBuffer<int, 1024> rb;
    const int NUM_ITEMS = 100000;
    std::atomic<bool> producer_done{false};
    std::atomic<int> items_produced{0};
    std::atomic<int> items_consumed{0};
    bool order_correct = true;

    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < NUM_ITEMS; i++) {
            while (!rb.enqueue(i)) {
                // Spin until space available
                std::this_thread::yield();
            }
            items_produced++;
        }
        producer_done = true;
    });

    // Consumer thread
    std::thread consumer([&]() {
        int expected = 0;
        while (expected < NUM_ITEMS) {
            int val;
            if (rb.dequeue(&val)) {
                if (val != expected) {
                    order_correct = false;
                }
                expected++;
                items_consumed++;
            } else if (producer_done && rb.is_empty()) {
                break;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    bool passed = (items_consumed == NUM_ITEMS) && order_correct;

    if (passed) {
        printf("PASS: SPSC thread safety (%d items, correct order)\n", NUM_ITEMS);
    } else {
        printf("FAIL: SPSC thread safety (consumed=%d, order_correct=%d)\n",
               items_consumed.load(), order_correct);
    }

    return passed;
}

bool test_overwriting_buffer() {
    printf("--- Overwriting Ring Buffer ---\n");
    int passed = 0;
    int total = 0;

    OverwritingRingBuffer<int, 4> rb;

    // Fill buffer
    for (int i = 0; i < 4; i++) {
        rb.enqueue(i);
    }

    // Overwrite with new values
    rb.enqueue(10);  // Overwrites 0
    rb.enqueue(11);  // Overwrites 1

    // Should get 2, 3, 10, 11
    std::vector<int> expected = {2, 3, 10, 11};
    std::vector<int> actual;
    int val;
    while (rb.dequeue(&val)) {
        actual.push_back(val);
    }

    if (actual == expected) {
        printf("PASS: Overwriting behavior correct\n");
        passed++;
    } else {
        printf("FAIL: Overwriting behavior\n");
        printf("  Expected: ");
        for (int v : expected) printf("%d ", v);
        printf("\n  Got: ");
        for (int v : actual) printf("%d ", v);
        printf("\n");
    }
    total++;

    printf("Overwriting tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

int main() {
    printf("Running Q11: Ring Buffer\n");
    printf("========================\n\n");

    bool all_passed = true;

    if (!test_basic_operations()) all_passed = false;
    if (!test_thread_safety()) all_passed = false;
    if (!test_overwriting_buffer()) all_passed = false;

    printf("========================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
