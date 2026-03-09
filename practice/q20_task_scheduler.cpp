/**
 * Q20: Priority-Based Task Scheduler for Bare-Metal Systems
 *
 * PROBLEM:
 * Design a simple priority-based task scheduler for a bare-metal embedded
 * system (no OS). This is a cooperative scheduler where tasks must
 * voluntarily yield.
 *
 * REQUIREMENTS:
 * - Fixed number of tasks
 * - Priority-based scheduling (higher priority runs first)
 * - Periodic tasks with configurable intervals
 * - One-shot tasks
 * - Task enable/disable
 * - Minimal overhead
 *
 * SCHEDULER TYPES:
 *
 * 1. Cooperative: Tasks run to completion or yield voluntarily
 *    - Simple, no context switching
 *    - One task can block all others
 *
 * 2. Preemptive: Higher priority tasks interrupt lower priority
 *    - Requires context switching
 *    - More complex, needs stack management
 *
 * 3. Time-sliced: Each task gets a time quantum
 *    - Fairness between same-priority tasks
 *    - Requires timer interrupt
 *
 * This implementation: Cooperative priority scheduler
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>

// ============ SYSTEM SIMULATION ============

static uint32_t g_system_ticks = 0;

uint32_t get_tick_count() {
    return g_system_ticks;
}

void advance_ticks(uint32_t count) {
    g_system_ticks += count;
}

void reset_ticks() {
    g_system_ticks = 0;
}

// ============ SCHEDULER DESIGN ============

// Maximum number of tasks
const size_t MAX_TASKS = 16;

// Task priority levels (0 = highest, 255 = lowest)
using Priority = uint8_t;
[[maybe_unused]] const Priority PRIORITY_HIGHEST = 0;
                 const Priority PRIORITY_HIGH    = 64;
                 const Priority PRIORITY_NORMAL  = 128;
                 const Priority PRIORITY_LOW     = 192;
[[maybe_unused]] const Priority PRIORITY_IDLE    = 255;

// Task states
enum TaskState : uint8_t {
    TASK_DISABLED,      // Task is not running
    TASK_READY,         // Task is ready to run
    TASK_RUNNING,       // Task is currently running
    TASK_BLOCKED,       // Task is waiting for event
    TASK_SUSPENDED      // Task is suspended by user
};

// Task function type
using TaskFunction = void (*)(void* context);

// Task control block
struct TaskControlBlock {
    const char* name;           // Task name (for debugging)
    TaskFunction function;      // Task entry point
    void* context;              // User context pointer
    Priority priority;          // Task priority
    TaskState state;            // Current state
    uint32_t period_ms;         // Period for periodic tasks (0 = one-shot)
    uint32_t last_run_tick;     // Last execution tick
    uint32_t next_run_tick;     // Next scheduled run
    uint32_t run_count;         // Number of times executed
    uint32_t max_runtime_us;    // Maximum observed runtime
    bool pending;               // Task has work to do
};

// ============ YOUR IMPLEMENTATION ============

class Scheduler {
private:
    TaskControlBlock tasks[MAX_TASKS];
    size_t task_count;
    [[maybe_unused]] size_t current_task;
    bool running;

    // TODO: Add any additional tracking variables

public:
    Scheduler() : task_count(0), current_task(0), running(false) {
        memset(tasks, 0, sizeof(tasks));
    }

    /**
     * Create a new task.
     *
     * @param name      Task name for debugging
     * @param function  Task function to execute
     * @param context   User context passed to function
     * @param priority  Task priority (lower = higher priority)
     * @param period_ms Period in milliseconds (0 for one-shot)
     * @return Task ID, or -1 on failure
     */
    int create_task(const char* name, TaskFunction function, void* context,
                    Priority priority, uint32_t period_ms = 0) {
        // TODO: Implement
        // 1. Find empty slot
        // 2. Initialize task control block
        // 3. Set state to READY
        // 4. Return task ID

        return -1;  // Placeholder
    }

    /**
     * Enable a task.
     */
    bool enable_task(int task_id) {
        // TODO: Implement
        return false;
    }

    /**
     * Disable a task.
     */
    bool disable_task(int task_id) {
        // TODO: Implement
        return false;
    }

    /**
     * Suspend a task (temporary disable).
     */
    bool suspend_task(int task_id) {
        // TODO: Implement
        return false;
    }

    /**
     * Resume a suspended task.
     */
    bool resume_task(int task_id) {
        // TODO: Implement
        return false;
    }

    /**
     * Set task priority dynamically.
     */
    bool set_priority(int task_id, Priority priority) {
        // TODO: Implement
        return false;
    }

    /**
     * Signal a task to run immediately (set pending flag).
     * Useful for event-driven tasks.
     */
    bool signal_task(int task_id) {
        // TODO: Implement
        return false;
    }

    /**
     * Run the scheduler.
     * Call this in your main loop.
     *
     * For cooperative scheduling, this runs one task and returns.
     * Call repeatedly in a while(1) loop.
     */
    void run() {
        // TODO: Implement
        //
        // Algorithm:
        // 1. Find highest priority task that is:
        //    - READY state
        //    - Either pending OR next_run_tick <= current_tick
        // 2. Mark task as RUNNING
        // 3. Execute task function
        // 4. Mark task as READY
        // 5. Update next_run_tick for periodic tasks
        // 6. Clear pending flag

    }

    /**
     * Start the scheduler (set running flag).
     */
    void start() {
        running = true;
    }

    /**
     * Stop the scheduler.
     */
    void stop() {
        running = false;
    }

    /**
     * Check if scheduler is running.
     */
    bool is_running() const {
        return running;
    }

    /**
     * Get task info for debugging.
     */
    const TaskControlBlock* get_task_info(int task_id) const {
        if (task_id >= 0 && (size_t)task_id < task_count) {
            return &tasks[task_id];
        }
        return nullptr;
    }

    /**
     * Get number of tasks.
     */
    size_t get_task_count() const {
        return task_count;
    }

    /**
     * Print scheduler status.
     */
    void print_status() {
        printf("Scheduler Status:\n");
        printf("  Running: %s\n", running ? "yes" : "no");
        printf("  Task count: %zu\n", task_count);
        printf("\nTasks:\n");

        for (size_t i = 0; i < task_count; i++) {
            const char* state_str;
            switch (tasks[i].state) {
                case TASK_DISABLED: state_str = "DISABLED"; break;
                case TASK_READY: state_str = "READY"; break;
                case TASK_RUNNING: state_str = "RUNNING"; break;
                case TASK_BLOCKED: state_str = "BLOCKED"; break;
                case TASK_SUSPENDED: state_str = "SUSPENDED"; break;
                default: state_str = "UNKNOWN"; break;
            }

            printf("  [%zu] %s\n", i, tasks[i].name);
            printf("      State: %s, Priority: %u\n", state_str, tasks[i].priority);
            printf("      Period: %u ms, Run count: %u\n",
                   tasks[i].period_ms, tasks[i].run_count);
        }
    }

private:
    /**
     * Find the highest priority ready task.
     */
    int find_next_task() {
        // TODO: Implement
        // Return task ID of highest priority ready task, or -1 if none

        return -1;
    }
};

// ============ TEST FRAMEWORK ============

// Test task execution tracking
static int g_task_execution_order[100];
static int g_execution_count = 0;

void reset_tracking() {
    g_execution_count = 0;
    memset(g_task_execution_order, -1, sizeof(g_task_execution_order));
}

void task_high_priority(void* ctx) {
    int id = (int)(intptr_t)ctx;
    if (g_execution_count < 100) {
        g_task_execution_order[g_execution_count++] = id;
    }
    printf("  Task HIGH (id=%d) executed at tick %u\n", id, get_tick_count());
}

void task_normal_priority(void* ctx) {
    int id = (int)(intptr_t)ctx;
    if (g_execution_count < 100) {
        g_task_execution_order[g_execution_count++] = id;
    }
    printf("  Task NORMAL (id=%d) executed at tick %u\n", id, get_tick_count());
}

void task_low_priority(void* ctx) {
    int id = (int)(intptr_t)ctx;
    if (g_execution_count < 100) {
        g_task_execution_order[g_execution_count++] = id;
    }
    printf("  Task LOW (id=%d) executed at tick %u\n", id, get_tick_count());
}

bool test_priority_order() {
    printf("--- Priority Order Tests ---\n\n");
    int passed = 0;
    int total = 0;

    reset_ticks();
    reset_tracking();

    Scheduler sched;

    // Create tasks with different priorities
    int low_id = sched.create_task("low", task_low_priority,
                                    (void*)0, PRIORITY_LOW, 0);
    int normal_id = sched.create_task("normal", task_normal_priority,
                                       (void*)1, PRIORITY_NORMAL, 0);
    int high_id = sched.create_task("high", task_high_priority,
                                     (void*)2, PRIORITY_HIGH, 0);

    // Signal all tasks to run
    sched.signal_task(low_id);
    sched.signal_task(normal_id);
    sched.signal_task(high_id);

    sched.start();

    // Run scheduler 3 times - should execute in priority order
    for (int i = 0; i < 3; i++) {
        sched.run();
    }

    // Verify execution order: high, normal, low
    if (g_execution_count == 3 &&
        g_task_execution_order[0] == 2 &&  // high
        g_task_execution_order[1] == 1 &&  // normal
        g_task_execution_order[2] == 0) {  // low
        printf("PASS: Tasks executed in priority order\n");
        passed++;
    } else {
        printf("FAIL: Wrong execution order\n");
        printf("  Got: ");
        for (int i = 0; i < g_execution_count; i++) {
            printf("%d ", g_task_execution_order[i]);
        }
        printf("\n");
    }
    total++;

    printf("\nPriority order tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_periodic_tasks() {
    printf("--- Periodic Task Tests ---\n\n");
    int passed = 0;
    int total = 0;

    reset_ticks();
    reset_tracking();

    Scheduler sched;

    // Create periodic task with 100ms period
    int task_id = sched.create_task("periodic", task_normal_priority,
                                     (void*)0, PRIORITY_NORMAL, 100);

    sched.start();

    // Run for simulated 500ms
    for (int i = 0; i < 50; i++) {
        sched.run();
        advance_ticks(10);  // 10ms per iteration
    }

    // Should have executed 5 times (at 0, 100, 200, 300, 400)
    const TaskControlBlock* info = sched.get_task_info(task_id);
    if (info && info->run_count == 5) {
        printf("PASS: Periodic task ran 5 times in 500ms\n");
        passed++;
    } else {
        printf("FAIL: Periodic task ran %u times (expected 5)\n",
               info ? info->run_count : 0);
    }
    total++;

    printf("\nPeriodic task tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_task_control() {
    printf("--- Task Control Tests ---\n\n");
    int passed = 0;
    int total = 0;

    reset_ticks();

    Scheduler sched;

    int task_id = sched.create_task("test", task_normal_priority,
                                     (void*)0, PRIORITY_NORMAL, 50);

    sched.start();

    // Run once
    sched.run();
    advance_ticks(100);

    // Suspend task
    sched.suspend_task(task_id);
    sched.run();  // Should not execute

    const TaskControlBlock* info = sched.get_task_info(task_id);
    uint32_t count_after_suspend = info ? info->run_count : 0;

    advance_ticks(100);
    sched.run();  // Still suspended

    if (info && info->run_count == count_after_suspend) {
        printf("PASS: Suspended task did not run\n");
        passed++;
    } else {
        printf("FAIL: Suspended task ran\n");
    }
    total++;

    // Resume task
    sched.resume_task(task_id);
    sched.run();

    if (info && info->run_count > count_after_suspend) {
        printf("PASS: Resumed task ran\n");
        passed++;
    } else {
        printf("FAIL: Resumed task did not run\n");
    }
    total++;

    printf("\nTask control tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

int main() {
    printf("Running Q20: Task Scheduler\n");
    printf("===========================\n\n");

    bool all_passed = true;

    if (!test_priority_order()) all_passed = false;
    if (!test_periodic_tasks()) all_passed = false;
    if (!test_task_control()) all_passed = false;

    printf("===========================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
