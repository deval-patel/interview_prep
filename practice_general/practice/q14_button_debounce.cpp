/**
 * Q14: Button Debounce for Embedded Systems
 *
 * PROBLEM:
 * Implement debouncing logic for a GPIO button input. Physical buttons
 * produce "bounces" - rapid on/off transitions when pressed or released.
 *
 * Without debouncing, a single button press might register as multiple presses.
 *
 * BOUNCE CHARACTERISTICS:
 * - Typically lasts 1-50ms (varies by button quality)
 * - Appears as rapid HIGH/LOW transitions
 * - Occurs on both press and release
 *
 * APPROACHES:
 *
 * 1. Delay-based: After detecting edge, wait debounce_time then read again
 *    - Simple but blocking
 *
 * 2. Timer-based: Start timer on edge, confirm state after timeout
 *    - Non-blocking, interrupt-driven
 *
 * 3. Counter-based: Require N consecutive same readings
 *    - Good for polling-based systems
 *
 * 4. Integrator: Increment/decrement counter based on state
 *    - Provides hysteresis, noise-immune
 *
 * REAL-WORLD CONSIDERATIONS:
 * - Multiple buttons with independent state
 * - Press, release, long-press, double-click detection
 * - Power consumption (polling vs interrupts)
 */

#include <cstdint>
#include <cstdio>
#include <vector>
#include <functional>

// ============ SIMULATED HARDWARE ============

// Simulates button GPIO with bouncing behavior
class SimulatedButton {
    std::vector<std::pair<uint32_t, bool>> transitions;  // (time, state)
    size_t current_idx;

public:
    SimulatedButton() : current_idx(0) {}

    void set_pattern(const std::vector<std::pair<uint32_t, bool>>& pattern) {
        transitions = pattern;
        current_idx = 0;
    }

    bool read(uint32_t current_time_ms) {
        while (current_idx + 1 < transitions.size() &&
               transitions[current_idx + 1].first <= current_time_ms) {
            current_idx++;
        }
        return transitions.empty() ? false : transitions[current_idx].second;
    }

    void reset() {
        current_idx = 0;
    }
};

// Global simulated button for tests
static SimulatedButton g_button;
static uint32_t g_current_time_ms = 0;

// Simulated GPIO read (call from your debouncer)
bool gpio_read_button() {
    return g_button.read(g_current_time_ms);
}

// Get current time in milliseconds
uint32_t get_time_ms() {
    return g_current_time_ms;
}

// ============ YOUR IMPLEMENTATION ============

/**
 * Simple delay-based debouncer.
 *
 * After detecting a state change, wait DEBOUNCE_MS and re-read.
 * This is blocking - acceptable for simple applications.
 */
class BlockingDebouncer {
private:
    static const uint32_t DEBOUNCE_MS = 20;
    [[maybe_unused]] bool last_stable_state;

public:
    BlockingDebouncer() : last_stable_state(false) {}

    /**
     * Read debounced button state (blocking).
     *
     * Note: In simulation, we can't actually block, so this
     * simulates the behavior.
     */
    bool read_blocking() {
        // TODO: Implement
        // 1. Read current state
        // 2. If different from last_stable_state, wait DEBOUNCE_MS
        // 3. Read again and update last_stable_state if still different

        return false;  // Placeholder
    }
};

/**
 * Counter-based debouncer (non-blocking, polling).
 *
 * Requires N consecutive same readings before accepting state change.
 * Call update() regularly from your main loop.
 */
class PollingDebouncer {
private:
    static const int REQUIRED_COUNT = 5;  // Consecutive readings needed

    // TODO: Add state variables
    // - current stable state
    // - count of consecutive readings
    // - last raw reading

public:
    PollingDebouncer() {
        // TODO: Initialize
    }

    /**
     * Call this regularly (e.g., every 5ms from main loop).
     * Updates internal state based on current GPIO reading.
     */
    void update() {
        // TODO: Implement
        // 1. Read GPIO
        // 2. If same as current raw state, increment counter
        // 3. If different, reset counter and update raw state
        // 4. If counter >= REQUIRED_COUNT, update stable state

    }

    /**
     * Get the debounced button state.
     */
    bool get_state() const {
        // TODO: Implement
        return false;  // Placeholder
    }

    /**
     * Check if button was just pressed (rising edge).
     * Clears the flag after reading.
     */
    bool was_pressed() {
        // TODO: Implement
        return false;  // Placeholder
    }

    /**
     * Check if button was just released (falling edge).
     * Clears the flag after reading.
     */
    bool was_released() {
        // TODO: Implement
        return false;  // Placeholder
    }
};

/**
 * Integrator-based debouncer (robust, non-blocking).
 *
 * Uses a counter that integrates the input:
 * - Increment when HIGH, decrement when LOW
 * - Clamp to [0, MAX]
 * - Output HIGH when counter reaches MAX, LOW when reaches 0
 *
 * Provides hysteresis and is very noise-immune.
 */
class IntegratorDebouncer {
private:
    static const int MAX_COUNT = 10;
    static const int THRESHOLD_HIGH = 8;  // Go HIGH above this
    static const int THRESHOLD_LOW = 2;   // Go LOW below this

    // TODO: Add state variables

public:
    IntegratorDebouncer() {
        // TODO: Initialize
    }

    /**
     * Call regularly to update integrator.
     */
    void update() {
        // TODO: Implement integrator logic

    }

    bool get_state() const {
        // TODO: Implement
        return false;  // Placeholder
    }
};

/**
 * BONUS: Multi-button manager with event callbacks.
 *
 * Handles multiple buttons, detects press/release/long-press/double-click.
 */
class ButtonManager {
public:
    enum Event {
        NONE,
        PRESSED,
        RELEASED,
        LONG_PRESS,
        DOUBLE_CLICK
    };

    using EventCallback = std::function<void(int button_id, Event event)>;

private:
    // TODO: Add per-button state tracking
    static const uint32_t LONG_PRESS_MS = 1000;
    static const uint32_t DOUBLE_CLICK_MS = 300;

public:
    void register_button(int button_id, bool (*gpio_read_func)()) {
        // TODO: Register button with its GPIO read function
    }

    void set_callback(EventCallback cb) {
        // TODO: Set event callback
    }

    void update() {
        // TODO: Update all buttons, call callbacks on events
    }
};

// ============ TEST FRAMEWORK ============

// Create a typical bouncy button press pattern
std::vector<std::pair<uint32_t, bool>> make_bouncy_press(uint32_t press_time) {
    std::vector<std::pair<uint32_t, bool>> pattern;
    pattern.push_back({0, false});  // Initial released state

    // Bouncy press starting at press_time
    pattern.push_back({press_time, true});
    pattern.push_back({press_time + 1, false});  // Bounce
    pattern.push_back({press_time + 2, true});
    pattern.push_back({press_time + 3, false});  // Bounce
    pattern.push_back({press_time + 5, true});
    pattern.push_back({press_time + 6, false});  // Bounce
    pattern.push_back({press_time + 10, true});  // Stable pressed

    return pattern;
}

std::vector<std::pair<uint32_t, bool>> make_bouncy_press_release(
    uint32_t press_time, uint32_t release_time) {

    auto pattern = make_bouncy_press(press_time);

    // Bouncy release
    pattern.push_back({release_time, false});
    pattern.push_back({release_time + 1, true});   // Bounce
    pattern.push_back({release_time + 2, false});
    pattern.push_back({release_time + 4, true});   // Bounce
    pattern.push_back({release_time + 8, false});  // Stable released

    return pattern;
}

bool test_polling_debouncer() {
    printf("--- Polling Debouncer Tests ---\n");
    int passed = 0;
    int total = 0;

    PollingDebouncer debouncer;

    // Test 1: Initial state is released
    g_button.set_pattern({{0, false}});
    g_current_time_ms = 0;

    for (int i = 0; i < 10; i++) {
        debouncer.update();
        g_current_time_ms += 5;
    }

    if (!debouncer.get_state()) {
        printf("PASS: Initial state is released\n");
        passed++;
    } else {
        printf("FAIL: Initial state should be released\n");
    }
    total++;

    // Test 2: Clean press (no bounce)
    g_button.set_pattern({{0, false}, {100, true}});
    g_current_time_ms = 0;

    // Run until after press
    for (int i = 0; i < 50; i++) {
        debouncer.update();
        g_current_time_ms += 5;
    }

    if (debouncer.get_state()) {
        printf("PASS: Clean press detected\n");
        passed++;
    } else {
        printf("FAIL: Clean press not detected\n");
    }
    total++;

    // Test 3: Bouncy press should still work
    g_button.set_pattern(make_bouncy_press(100));
    g_current_time_ms = 0;
    PollingDebouncer debouncer2;

    // Run through the bouncy pattern
    for (int i = 0; i < 60; i++) {  // 300ms total
        debouncer2.update();
        g_current_time_ms += 5;
    }

    if (debouncer2.get_state()) {
        printf("PASS: Bouncy press debounced correctly\n");
        passed++;
    } else {
        printf("FAIL: Bouncy press not handled\n");
    }
    total++;

    // Test 4: was_pressed() edge detection
    g_button.set_pattern({{0, false}, {50, true}});
    g_current_time_ms = 0;
    PollingDebouncer debouncer3;

    bool press_detected = false;
    int press_count = 0;

    for (int i = 0; i < 40; i++) {
        debouncer3.update();
        if (debouncer3.was_pressed()) {
            press_detected = true;
            press_count++;
        }
        g_current_time_ms += 5;
    }

    if (press_detected && press_count == 1) {
        printf("PASS: was_pressed() triggered exactly once\n");
        passed++;
    } else {
        printf("FAIL: was_pressed() count = %d (expected 1)\n", press_count);
    }
    total++;

    printf("Polling debouncer tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_integrator_debouncer() {
    printf("--- Integrator Debouncer Tests ---\n");
    int passed = 0;
    int total = 0;

    IntegratorDebouncer debouncer;

    // Test: Very noisy signal should be filtered
    // Alternating 0/1 should not trigger a stable state change
    std::vector<std::pair<uint32_t, bool>> noisy_pattern;
    noisy_pattern.push_back({0, false});
    for (int i = 1; i < 100; i++) {
        noisy_pattern.push_back({(uint32_t)i, i % 2 == 0});
    }
    noisy_pattern.push_back({100, false});  // End with stable low

    g_button.set_pattern(noisy_pattern);
    g_current_time_ms = 0;

    bool ever_high = false;
    for (int i = 0; i < 100; i++) {
        debouncer.update();
        if (debouncer.get_state()) ever_high = true;
        g_current_time_ms += 1;
    }

    if (!ever_high) {
        printf("PASS: Noisy signal filtered out\n");
        passed++;
    } else {
        printf("FAIL: Integrator should filter 50%% duty noise\n");
    }
    total++;

    // Test: Sustained high should eventually trigger
    g_button.set_pattern({{0, false}, {10, true}});
    g_current_time_ms = 0;
    IntegratorDebouncer debouncer2;

    for (int i = 0; i < 50; i++) {
        debouncer2.update();
        g_current_time_ms += 1;
    }

    if (debouncer2.get_state()) {
        printf("PASS: Sustained high detected\n");
        passed++;
    } else {
        printf("FAIL: Sustained high not detected\n");
    }
    total++;

    printf("Integrator debouncer tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

int main() {
    printf("Running Q14: Button Debounce\n");
    printf("============================\n\n");

    bool all_passed = true;

    if (!test_polling_debouncer()) all_passed = false;
    if (!test_integrator_debouncer()) all_passed = false;

    printf("============================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
