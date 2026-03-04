# Practice Set 2: Solutions & Deep Dives

## 1. Modern C++: The "C Engineer's" Perspective

### A1. RAII and Move Semantics
**Concept:** In C, you manually call `CloseHandle()`. In C++, we use the **Destructor** to do it for us.

**The "Standard" Way (Using `std::unique_ptr`):**
For an interview, the best answer is to use `std::unique_ptr` with a custom deleter. It’s zero-overhead and highly readable.
```cpp
struct HandleDeleter {
    void operator()(HANDLE h) { if (h) CloseDevice(h); }
};

// Usage:
std::unique_ptr<void, HandleDeleter> myHandle(OpenDevice(1));
// CloseDevice is called automatically when myHandle goes out of scope.