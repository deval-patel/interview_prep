# Solutions to Practice Questions (Document 2 of 3)

---

## Q1. LRU Cache Implementation - SOLUTION

### Approach:
Use a hash map (unordered_map) for O(1) lookup and a doubly linked list to maintain access order. Most recently used items go to the front.

### Code:
```cpp
class LRUCache {
    struct Node {
        int key, value;
        Node *prev, *next;
        Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };
    
    int capacity;
    unordered_map<int, Node*> cache;
    Node *head, *tail;
    
    void addToFront(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }
    
    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    
public:
    LRUCache(int cap) : capacity(cap) {
        head = new Node(0, 0);
        tail = new Node(0, 0);
        head->next = tail;
        tail->prev = head;
    }
    
    int get(int key) {
        if (!cache.count(key)) return -1;
        Node* node = cache[key];
        removeNode(node);
        addToFront(node);
        return node->value;
    }
    
    void put(int key, int value) {
        if (cache.count(key)) {
            Node* node = cache[key];
            node->value = value;
            removeNode(node);
            addToFront(node);
        } else {
            if (cache.size() == capacity) {
                Node* lru = tail->prev;
                cache.erase(lru->key);
                removeNode(lru);
                delete lru;
            }
            Node* newNode = new Node(key, value);
            cache[key] = newNode;
            addToFront(newNode);
        }
    }
};
```

### Complexity:
Time: O(1) for both get and put. Space: O(capacity)

---

## Q2. Find Missing Number - SOLUTION

### Approach:
Use XOR property: a ^ a = 0, and XOR is commutative. XOR all numbers in array with all numbers from 0 to n.

### Code:
```cpp
int missingNumber(vector<int>& nums) {
    int n = nums.size();
    int result = n; // Start with n
    
    for (int i = 0; i < n; i++) {
        result ^= i ^ nums[i];
    }
    
    return result;
}

// Alternative: Sum formula
int missingNumberSum(vector<int>& nums) {
    int n = nums.size();
    int expectedSum = n * (n + 1) / 2;
    int actualSum = 0;
    for (int num : nums) {
        actualSum += num;
    }
    return expectedSum - actualSum;
}
```

### Complexity:
Time: O(n), Space: O(1)

---

## Q3. Merge K Sorted Lists - SOLUTION

### Approach:
Use a min-heap (priority_queue) to track the smallest element among k lists.

### Code:
```cpp
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(nullptr) {}
};

struct Compare {
    bool operator()(ListNode* a, ListNode* b) {
        return a->val > b->val; // Min heap
    }
};

ListNode* mergeKLists(vector<ListNode*>& lists) {
    priority_queue<ListNode*, vector<ListNode*>, Compare> pq;
    
    // Add first node from each list
    for (ListNode* list : lists) {
        if (list) pq.push(list);
    }
    
    ListNode dummy(0);
    ListNode* tail = &dummy;
    
    while (!pq.empty()) {
        ListNode* node = pq.top();
        pq.pop();
        
        tail->next = node;
        tail = tail->next;
        
        if (node->next) {
            pq.push(node->next);
        }
    }
    
    return dummy.next;
}
```

### Complexity:
Time: O(N log k) where N is total nodes, Space: O(k) for heap

---

## Q4. Longest Consecutive Sequence - SOLUTION

### Approach:
Use unordered_set for O(1) lookup. For each number, check if it's the start of a sequence (num-1 not in set), then count consecutive numbers.

### Code:
```cpp
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> numSet(nums.begin(), nums.end());
    int maxLen = 0;
    
    for (int num : numSet) {
        // Check if it's the start of a sequence
        if (numSet.find(num - 1) == numSet.end()) {
            int currentNum = num;
            int currentLen = 1;
            
            // Count consecutive numbers
            while (numSet.find(currentNum + 1) != numSet.end()) {
                currentNum++;
                currentLen++;
            }
            
            maxLen = max(maxLen, currentLen);
        }
    }
    
    return maxLen;
}
```

### Complexity:
Time: O(n) - each number visited at most twice, Space: O(n)

---

## Q5. Validate BST - SOLUTION

### Approach:
Use recursion with valid range constraints. Each node must be within its min-max bounds.

### Code:
```cpp
struct TreeNode {
    int val;
    TreeNode *left, *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

bool isValidBST(TreeNode* root) {
    return validate(root, LONG_MIN, LONG_MAX);
}

bool validate(TreeNode* node, long minVal, long maxVal) {
    if (!node) return true;
    
    if (node->val <= minVal || node->val >= maxVal) {
        return false;
    }
    
    return validate(node->left, minVal, node->val) &&
           validate(node->right, node->val, maxVal);
}
```

### Complexity:
Time: O(n), Space: O(h) where h is height (recursion stack)

---

## Bit Manipulation Solutions

## Q6. Count Set Bits - SOLUTION

### Approach 1: Brian Kernighan's Algorithm
n & (n-1) clears the rightmost set bit. Count until n becomes 0.

### Code:
```cpp
// Method 1: Brian Kernighan's Algorithm - O(number of set bits)
int countSetBits1(uint32_t n) {
    int count = 0;
    while (n) {
        n &= (n - 1);  // Clear rightmost set bit
        count++;
    }
    return count;
}

// Method 2: Check each bit - O(32)
int countSetBits2(uint32_t n) {
    int count = 0;
    for (int i = 0; i < 32; i++) {
        if (n & (1 << i)) {
            count++;
        }
    }
    return count;
}

// Method 3: Built-in (fastest in practice)
int countSetBits3(uint32_t n) {
    return __builtin_popcount(n);
}
```

### Trade-offs:
Method 1 is best for sparse bits. Method 2 is constant time. Method 3 uses hardware instruction (POPCNT).

---

## Q7. Check Power of Two - SOLUTION

### Approach:
Power of two has exactly one set bit. Use n & (n-1) == 0.

### Code:
```cpp
bool isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// Why it works:
// Powers of 2: 0001, 0010, 0100, 1000 (one bit set)
// n-1:         0000, 0001, 0011, 0111 (all lower bits set)
// n & (n-1):   0000, 0000, 0000, 0000 (clears the only set bit)
```

### Complexity:
Time: O(1), Space: O(1)

---

## Q8. Single Number (XOR) - SOLUTION

### Approach:
XOR all elements. Duplicates cancel out (a ^ a = 0), leaving the single element.

### Code:
```cpp
int singleNumber(vector<int>& nums) {
    int result = 0;
    for (int num : nums) {
        result ^= num;
    }
    return result;
}

// Why it works:
// XOR properties: a ^ a = 0, a ^ 0 = a, XOR is commutative
// Example: [4,1,2,1,2] -> 4^1^2^1^2 = 4^(1^1)^(2^2) = 4^0^0 = 4
```

### Complexity:
Time: O(n), Space: O(1)

---

## Q9. Reverse Bits - SOLUTION

### Approach:
Extract each bit from input and place it in reversed position in result.

### Code:
```cpp
uint32_t reverseBits(uint32_t n) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        // Extract bit i from n
        uint32_t bit = (n >> i) & 1;
        // Place it at position (31-i) in result
        result |= (bit << (31 - i));
    }
    return result;
}

// Alternative: Swap pairs, then nibbles, then bytes...
uint32_t reverseBitsFast(uint32_t n) {
    n = ((n & 0xAAAAAAAA) >> 1) | ((n & 0x55555555) << 1);
    n = ((n & 0xCCCCCCCC) >> 2) | ((n & 0x33333333) << 2);
    n = ((n & 0xF0F0F0F0) >> 4) | ((n & 0x0F0F0F0F) << 4);
    n = ((n & 0xFF00FF00) >> 8) | ((n & 0x00FF00FF) << 8);
    n = (n >> 16) | (n << 16);
    return n;
}
```

### Complexity:
Time: O(1) - always 32 iterations, Space: O(1)

---

## Q10. Extract Bits from Register - SOLUTION

```cpp
uint32_t extractBits(uint32_t value, int pos, int n) {
    // Create mask: n consecutive 1s
    uint32_t mask = (1 << n) - 1;
    // Shift value right by pos, then apply mask
    return (value >> pos) & mask;
}

// Example: extractBits(0b11010110, 2, 3)
// Step 1: Shift right by 2: 0b00110101
// Step 2: Mask with 0b111: 0b00000101 = 5
```

---

## Q11. Set/Clear/Toggle Bits - SOLUTION

```cpp
// Set bit n to 1
uint32_t setBit(uint32_t value, int n) {
    return value | (1 << n);
}
// Use case: Enable interrupt in control register

// Clear bit n to 0
uint32_t clearBit(uint32_t value, int n) {
    return value & ~(1 << n);
}
// Use case: Disable specific peripheral

// Toggle bit n
uint32_t toggleBit(uint32_t value, int n) {
    return value ^ (1 << n);
}
// Use case: Toggle LED state, flip configuration flag
```

---

## Q12. Rightmost Set Bit Position - SOLUTION

```cpp
int rightmostSetBit(int n) {
    if (n == 0) return -1;
    
    // Method 1: Using log
    // n & -n isolates rightmost set bit
    int isolated = n & -n;
    return log2(isolated);
}

int rightmostSetBit2(int n) {
    if (n == 0) return -1;
    
    // Method 2: Count trailing zeros
    int pos = 0;
    while ((n & 1) == 0) {
        n >>= 1;
        pos++;
    }
    return pos;
}

// Method 3: Built-in
int rightmostSetBit3(int n) {
    if (n == 0) return -1;
    return __builtin_ctz(n);  // Count Trailing Zeros
}
```

---

## Embedded Systems Solutions

## Q13. Simple Memory Allocator - SOLUTION

### Key Concepts:
- Free list with block headers containing size and free flag
- Coalescing: Merge adjacent free blocks to reduce fragmentation
- Alignment: Ensure addresses align to word boundaries (typically 4 or 8 bytes)

```cpp
struct BlockHeader {
    size_t size;      // Size of block (excluding header)
    bool isFree;      // Free flag
    BlockHeader* next; // Next block in list
};

class SimpleAllocator {
private:
    void* heapStart;
    size_t heapSize;
    BlockHeader* freeList;
    
    void coalesce() {
        BlockHeader* current = freeList;
        while (current && current->next) {
            if (current->isFree && current->next->isFree) {
                // Merge blocks
                current->size += sizeof(BlockHeader) + current->next->size;
                current->next = current->next->next;
            } else {
                current = current->next;
            }
        }
    }
    
public:
    void* malloc(size_t size) {
        // Align size to 8 bytes
        size = (size + 7) & ~7;
        
        BlockHeader* current = freeList;
        while (current) {
            if (current->isFree && current->size >= size) {
                current->isFree = false;
                
                // Split block if large enough
                if (current->size >= size + sizeof(BlockHeader) + 8) {
                    BlockHeader* newBlock = (BlockHeader*)(
                        (char*)current + sizeof(BlockHeader) + size);
                    newBlock->size = current->size - size - sizeof(BlockHeader);
                    newBlock->isFree = true;
                    newBlock->next = current->next;
                    current->next = newBlock;
                    current->size = size;
                }
                
                return (void*)((char*)current + sizeof(BlockHeader));
            }
            current = current->next;
        }
        return nullptr; // Out of memory
    }
    
    void free(void* ptr) {
        if (!ptr) return;
        BlockHeader* block = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
        block->isFree = true;
        coalesce();
    }
};
```

---

## Q14. Circular Buffer - SOLUTION

```cpp
template<typename T, size_t SIZE>
class CircularBuffer {
private:
    T buffer[SIZE];
    size_t head;  // Write position
    size_t tail;  // Read position
    size_t count; // Number of items
    
public:
    CircularBuffer() : head(0), tail(0), count(0) {}
    
    bool enqueue(const T& item) {
        if (isFull()) return false;
        
        buffer[head] = item;
        head = (head + 1) % SIZE;
        count++;
        return true;
    }
    
    bool dequeue(T& item) {
        if (isEmpty()) return false;
        
        item = buffer[tail];
        tail = (tail + 1) % SIZE;
        count--;
        return true;
    }
    
    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == SIZE; }
    size_t size() const { return count; }
};

// Thread-safe version (add mutex):
template<typename T, size_t SIZE>
class ThreadSafeCircularBuffer {
private:
    CircularBuffer<T, SIZE> buffer;
    std::mutex mtx;
    std::condition_variable cvNotFull;
    std::condition_variable cvNotEmpty;
    
public:
    void enqueue(const T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        cvNotFull.wait(lock, [this] { return !buffer.isFull(); });
        buffer.enqueue(item);
        cvNotEmpty.notify_one();
    }
    
    T dequeue() {
        std::unique_lock<std::mutex> lock(mtx);
        cvNotEmpty.wait(lock, [this] { return !buffer.isEmpty(); });
        T item;
        buffer.dequeue(item);
        cvNotFull.notify_one();
        return item;
    }
};
```

---

## Q15. Memory-Mapped Register Access - SOLUTION

```cpp
#define REG_ADDR 0x40021000

// CRITICAL: Use volatile to prevent compiler optimization
// The hardware can change register values at any time
volatile uint32_t* const reg = (volatile uint32_t*)REG_ADDR;

// 1. Read entire 32-bit register
uint32_t readRegister() {
    return *reg;
}

// 2. Set bit 5 without affecting other bits
void setBit5() {
    *reg |= (1 << 5);
}

// 3. Clear bits 8-11 (4 bits) without affecting others
void clearBits8_11() {
    uint32_t mask = 0b1111 << 8;  // Create mask for bits 8-11
    *reg &= ~mask;                // Clear those bits
}

// Alternative: Bit-field approach (less portable but readable)
struct ControlRegister {
    uint32_t bit0_4  : 5;
    uint32_t bit5    : 1;  // The bit we want to set
    uint32_t bit6_7  : 2;
    uint32_t bit8_11 : 4;  // The bits we want to clear
    uint32_t bit12_31: 20;
};
volatile ControlRegister* const ctrlReg = 
    (volatile ControlRegister*)REG_ADDR;

// Why volatile is necessary:
// 1. Prevents compiler from caching register value in CPU register
// 2. Ensures every read/write goes to actual memory address
// 3. Prevents reordering of memory accesses by compiler
```

---

## Q16. Aligned Memory Allocation - SOLUTION

### Approach:
To return an aligned pointer while still being able to free the original allocation:
1. Allocate extra space: `size + alignment - 1 + sizeof(void*)`
2. Find the aligned address within that block
3. Store the original pointer just before the aligned address
4. On free, retrieve the original pointer and free it

### Code:
```cpp
void* aligned_malloc(size_t size, size_t alignment) {
    // Validate inputs
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        return nullptr;  // alignment must be power of 2
    }

    // Allocate extra space for alignment and storing original pointer
    size_t total = size + alignment - 1 + sizeof(void*);
    void* raw = malloc(total);
    if (!raw) return nullptr;

    // Calculate aligned address, leaving room for the stored pointer
    uintptr_t rawAddr = reinterpret_cast<uintptr_t>(raw);
    uintptr_t alignedAddr = (rawAddr + sizeof(void*) + alignment - 1) & ~(alignment - 1);

    // Store the original pointer just before the aligned address
    void** storedPtr = reinterpret_cast<void**>(alignedAddr - sizeof(void*));
    *storedPtr = raw;

    return reinterpret_cast<void*>(alignedAddr);
}

void aligned_free(void* ptr) {
    if (!ptr) return;

    // Retrieve the original pointer stored just before ptr
    void* raw = *(reinterpret_cast<void**>(ptr) - 1);
    free(raw);
}
```

### Visual Explanation:
```
malloc returns:     [----raw memory block--------------------------]
                    ^
                    raw (original pointer)

After alignment:    [padding][stored_ptr][====aligned user memory===]
                             ^           ^
                             |           aligned_ptr (returned to user)
                             points to raw
```

### Example Walkthrough:
```
aligned_malloc(100, 16):
1. malloc(100 + 16 - 1 + 8) = malloc(123)
2. Say malloc returns 0x1000
3. Aligned address = (0x1000 + 8 + 15) & ~15 = 0x1010
4. Store 0x1000 at address 0x1008 (just before 0x1010)
5. Return 0x1010 (16-byte aligned)

aligned_free(0x1010):
1. Read pointer at 0x1008 → gets 0x1000
2. free(0x1000)
```

### Key Points:
- The `& ~(alignment - 1)` trick clears lower bits to align (works because alignment is power of 2)
- We need `sizeof(void*)` extra bytes to store the original pointer
- The aligned address must leave room for the stored pointer

### Complexity:
Time: O(1) for both functions, Space: O(alignment) overhead per allocation

---

## Synchronization & Concurrency Solutions

## Q17. Producer-Consumer with Mutex - SOLUTION

```cpp
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ProducerConsumerQueue {
private:
    std::queue<T> buffer;
    std::mutex mtx;
    std::condition_variable cvNotFull;
    std::condition_variable cvNotEmpty;
    size_t maxSize;
    bool done;
    
public:
    ProducerConsumerQueue(size_t max) : maxSize(max), done(false) {}
    
    void produce(const T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Wait until buffer not full
        cvNotFull.wait(lock, [this] { 
            return buffer.size() < maxSize || done; 
        });
        
        if (done) return;
        
        buffer.push(item);
        cvNotEmpty.notify_one();  // Wake up one consumer
    }
    
    bool consume(T& item) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // Wait until buffer has items or done
        cvNotEmpty.wait(lock, [this] { 
            return !buffer.empty() || done; 
        });
        
        if (buffer.empty()) return false;  // Queue closed
        
        item = buffer.front();
        buffer.pop();
        cvNotFull.notify_one();  // Wake up one producer
        return true;
    }
    
    void close() {
        std::unique_lock<std::mutex> lock(mtx);
        done = true;
        cvNotFull.notify_all();
        cvNotEmpty.notify_all();
    }
};
```

---

## Q18. Reader-Writer Lock - SOLUTION

### Design Concepts:
- Multiple readers can hold lock simultaneously
- Writers need exclusive access
- Prevent writer starvation with queue or priority

```cpp
class ReaderWriterLock {
private:
    std::mutex mtx;
    std::condition_variable cvRead;
    std::condition_variable cvWrite;
    int readers;        // Active readers
    bool writer;        // Writer active
    int waitingWriters; // Writers waiting
    
public:
    ReaderWriterLock() : readers(0), writer(false), waitingWriters(0) {}
    
    void lockRead() {
        std::unique_lock<std::mutex> lock(mtx);
        // Wait if writer active or writers waiting (prevents starvation)
        cvRead.wait(lock, [this] { 
            return !writer && waitingWriters == 0; 
        });
        readers++;
    }
    
    void unlockRead() {
        std::unique_lock<std::mutex> lock(mtx);
        readers--;
        if (readers == 0) {
            cvWrite.notify_one();  // Wake up waiting writer
        }
    }
    
    void lockWrite() {
        std::unique_lock<std::mutex> lock(mtx);
        waitingWriters++;
        // Wait until no readers and no writer
        cvWrite.wait(lock, [this] { 
            return readers == 0 && !writer; 
        });
        waitingWriters--;
        writer = true;
    }
    
    void unlockWrite() {
        std::unique_lock<std::mutex> lock(mtx);
        writer = false;
        if (waitingWriters > 0) {
            cvWrite.notify_one();   // Prefer waiting writers
        } else {
            cvRead.notify_all();    // Wake all readers
        }
    }
};

// RAII wrappers
class ReadGuard {
    ReaderWriterLock& lock;
public:
    ReadGuard(ReaderWriterLock& l) : lock(l) { lock.lockRead(); }
    ~ReadGuard() { lock.unlockRead(); }
};

class WriteGuard {
    ReaderWriterLock& lock;
public:
    WriteGuard(ReaderWriterLock& l) : lock(l) { lock.lockWrite(); }
    ~WriteGuard() { lock.unlockWrite(); }
};
```

---

## Q19. Race Condition Detection - SOLUTION

### Problem:
counter++ is NOT atomic. It involves: (1) Read counter, (2) Increment, (3) Write back. Two threads can interleave, causing lost updates.

### Solutions:
```cpp
// Solution 1: Mutex
std::mutex mtx;
int counter = 0;

void increment() {
    std::lock_guard<std::mutex> lock(mtx);
    counter++;
}

// Solution 2: Atomic operations (preferred for simple counters)
std::atomic<int> counter(0);

void increment() {
    counter.fetch_add(1, std::memory_order_relaxed);
    // Or simply: counter++;
}

// Solution 3: Spinlock (for very short critical sections)
std::atomic_flag lock = ATOMIC_FLAG_INIT;
int counter = 0;

void increment() {
    while (lock.test_and_set(std::memory_order_acquire));
    counter++;
    lock.clear(std::memory_order_release);
}
```

---

## Q20. Spinlock vs Mutex - SOLUTION

### Key Differences:

**SPINLOCK:**
- Busy-waits (keeps CPU occupied)
- No context switch overhead
- Best for very short critical sections (nanoseconds to microseconds)
- Common in ISRs and kernel code
- Wastes CPU if lock held for long

**MUTEX:**
- Thread sleeps when waiting (yields CPU)
- Context switch overhead
- Best for longer critical sections
- More efficient for multi-millisecond waits

### When to Use in Embedded Systems:
- **Spinlock:** Protecting hardware registers, quick flag checks, ISR-safe locks
- **Mutex:** File I/O, network operations, complex data structure operations

---

## Q21. Priority Inversion - SOLUTION

### Problem Scenario:
High-priority task H needs resource held by low-priority task L. Medium-priority task M preempts L, indirectly blocking H (priority inversion).

### Solution 1: Priority Inheritance
When H blocks on L's mutex, temporarily boost L's priority to H's level. L completes faster, releases mutex, H resumes.

### Solution 2: Priority Ceiling
Mutex has a ceiling priority (highest priority of any task that uses it). When L locks mutex, it immediately inherits ceiling priority, preventing M from preempting.

```cpp
// Most RTOS support priority inheritance in mutex creation
// FreeRTOS example:
xSemaphoreCreateMutex();  // Automatically uses priority inheritance

// POSIX threads example:
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT);
pthread_mutex_init(&mutex, &attr);
```

---

## Embedded Domain Solutions

## Q22. ISR Design Principles - SOLUTION

### Key Principles:
- Keep ISRs SHORT (microseconds, not milliseconds)
- Minimize work - defer processing to main loop or task
- Never call blocking functions (malloc, printf, mutex_lock)
- Clear interrupt flag to avoid re-triggering
- Use volatile for shared variables
- Disable interrupts only when absolutely necessary

```cpp
// Example: UART RX ISR
volatile CircularBuffer<uint8_t, 256> rxBuffer;
volatile bool dataReady = false;

void UART_RX_IRQHandler() {
    // 1. Read data from hardware register (fast)
    uint8_t data = UART->DR;
    
    // 2. Buffer it (fast)
    rxBuffer.enqueue(data);
    
    // 3. Set flag for main loop (fast)
    if (data == '\n') {
        dataReady = true;
    }
    
    // 4. Clear interrupt flag (critical)
    UART->SR &= ~UART_SR_RXNE;
    
    // NO: Don't do heavy processing here
    // NO: Don't call printf, malloc, or blocking functions
    // NO: Don't take too long (< 10us ideal)
}

// Main loop handles processing
void mainLoop() {
    while (true) {
        if (dataReady) {
            dataReady = false;
            // Process received data (can take longer)
            processReceivedData();
        }
    }
}
```

---

## Q23. Volatile Keyword - SOLUTION

### When to Use:
- Memory-mapped hardware registers
- Variables modified by ISRs
- Multi-threaded shared variables (though atomics are preferred now)

```cpp
// Example 1: Memory-mapped register
#define GPIO_ODR ((volatile uint32_t*)0x40020014)

void setLED() {
    *GPIO_ODR |= (1 << 5);  // Compiler won't optimize away
}

// Example 2: ISR-modified variable
volatile bool buttonPressed = false;

void EXTI_IRQHandler() {
    buttonPressed = true;
    // Clear interrupt flag
}

void mainLoop() {
    while (!buttonPressed);  // volatile prevents optimization
    // Handle button press
}

// WITHOUT volatile, compiler might optimize to:
// if (!buttonPressed) while(true);  // Infinite loop!

// Example 3: Polling hardware status
volatile uint32_t* statusReg = (volatile uint32_t*)0x40000000;

while (!(*statusReg & STATUS_READY)) {
    // Wait for hardware to set ready bit
    // volatile ensures we read from hardware each iteration
}
```

---

## Q24. Endianness Conversion - SOLUTION

```cpp
uint32_t swapEndian32(uint32_t value) {
    return ((value & 0x000000FF) << 24) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

uint16_t swapEndian16(uint16_t value) {
    return (value << 8) | (value >> 8);
}

// Built-in functions (GCC/Clang):
uint32_t swap32 = __builtin_bswap32(value);
uint16_t swap16 = __builtin_bswap16(value);

// Use cases:
// 1. Network protocols (TCP/IP uses big-endian)
// 2. File formats (JPEG, PNG headers)
// 3. Cross-platform data exchange
// 4. Reading data from different architecture devices

// Example: Reading 32-bit value from I2C sensor (big-endian)
uint32_t readSensorBigEndian() {
    uint8_t bytes[4];
    i2c_read(bytes, 4);
    uint32_t value = (bytes[0] << 24) | (bytes[1] << 16) | 
                     (bytes[2] << 8) | bytes[3];
    return value;
}
```

---

## Q25-Q29: Additional Solutions

### Q25. Watchdog Timer:
- Hardware timer that resets system if not periodically refreshed
- Prevents system from hanging indefinitely
- Typical pattern: Feed watchdog in main loop, timeout if loop stalls
- Trade-off: Too short = false resets; too long = slow recovery

### Q26. DMA:
- Direct Memory Access allows peripherals to transfer data without CPU
- Benefits: Frees CPU for other work, higher throughput
- Cache coherency: Must flush cache before DMA read, invalidate after DMA write
- Use memory barriers and cache management functions

### Q27. Low-Power Sensor Design:
- Use sleep modes between readings (deep sleep, standby)
- Wake on timer interrupt or external trigger
- Batch data locally, transmit periodically
- Use low-power peripherals (LPUART, RTC)

### Q28. Device Driver Design:
- Initialization: Configure I2C, set sensor mode, verify device ID
- Read: Send command, wait for data ready, read bytes via I2C
- Error handling: Timeouts, CRC checks, retry logic
- Interrupt vs polling: Interrupt for low latency, polling for simplicity

### Q29. Firmware Update:
- Dual-bank flash: Bank A (active), Bank B (update)
- Write new firmware to Bank B
- Verify with CRC/signature
- Set boot flag, reboot to Bank B
- If boot fails, rollback to Bank A
- Prevent bricking: Never erase both banks simultaneously
