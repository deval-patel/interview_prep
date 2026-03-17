/*
 * Q18 — VLIW Instruction Packing [Hard]
 *
 * Topic: Hardware Architecture — VLIW
 *
 * Problem:
 *   Implement a VLIW (Very Long Instruction Word) instruction packer.
 *   A VLIW processor has multiple functional units that execute in parallel.
 *   The compiler/scheduler must pack independent operations into single
 *   VLIW words, filling unused slots with NOPs.
 *
 *   Functional units:
 *   - ALU: arithmetic/logic operations
 *   - MUL: multiplication operations
 *   - MEM: memory load/store operations
 *   - BRANCH: control flow operations
 *
 *   Each VLIW word can contain at most one operation per functional unit.
 *   Operations have data dependencies that must be respected.
 *
 * Constraints:
 *   - 4 functional unit slots per VLIW word
 *   - Operations have read/write register sets for dependency tracking
 *   - WAR, RAW, WAW hazards must be detected
 *   - Greedy packing: try to fill each VLIW word as much as possible
 *
 * Key Concepts:
 *   - VLIW architecture
 *   - Instruction-level parallelism (ILP)
 *   - Data hazard detection (RAW, WAR, WAW)
 *   - NOP insertion for unfilled slots
 */

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <set>
#include <algorithm>
#include <cstring>

using namespace std;

enum FuncUnit { ALU = 0, MUL = 1, MEM = 2, BRANCH = 3, NUM_UNITS = 4 };

const char* unit_names[] = {"ALU", "MUL", "MEM", "BR "};

struct Operation {
    int id;
    const char* name;
    FuncUnit unit;           // which functional unit this op uses
    vector<int> reads;       // registers read by this op
    vector<int> writes;      // registers written by this op
    vector<int> deps;        // operation IDs this depends on (explicit)
};

struct VLIWWord {
    int ops[4];  // operation ID for each slot (-1 = NOP)
    VLIWWord() { ops[0] = ops[1] = ops[2] = ops[3] = -1; }
};

// ============ YOUR IMPLEMENTATION ============

struct VLIWScheduler {
    vector<Operation> operations;

    /*
     * add_op: Add an operation to the schedule.
     * Returns the operation ID.
     */
    int add_op(const char* name, FuncUnit unit,
               vector<int> reads, vector<int> writes,
               vector<int> deps = {}) {
        // TODO: Create operation and add to list
        return -1;
    }

    /*
     * has_data_hazard: Check if op_b has a data dependency on op_a.
     *
     * RAW (Read After Write): op_b reads a register that op_a writes
     * WAW (Write After Write): op_b writes a register that op_a writes
     * WAR (Write After Read): op_b writes a register that op_a reads
     *
     * Returns true if scheduling op_b in the same VLIW word as op_a
     * would violate a dependency.
     */
    bool has_data_hazard(int op_a_id, int op_b_id) {
        // TODO: Check for RAW, WAW, WAR hazards
        return false;
    }

    /*
     * can_pack_together: Check if an operation can be added to a VLIW word.
     *
     * Conditions:
     *   1. The functional unit slot is not already taken
     *   2. No data hazards with any operation already in the word
     *   3. All explicit dependencies are satisfied (scheduled in earlier words)
     */
    bool can_pack_together(const VLIWWord& word, int op_id,
                           const set<int>& scheduled_ops) {
        // TODO: Implement
        return false;
    }

    /*
     * schedule: Pack all operations into VLIW words.
     *
     * Greedy algorithm:
     *   1. Maintain a set of "ready" operations (all deps satisfied)
     *   2. For each VLIW word:
     *      a. Try to pack as many ready operations as possible
     *      b. For each functional unit, find a ready op that fits
     *   3. Mark packed operations as scheduled
     *   4. Update ready set
     *   5. Repeat until all operations scheduled
     */
    vector<VLIWWord> schedule() {
        // TODO: Implement greedy VLIW packing
        return {};
    }

    /*
     * compute_ilp: Compute instruction-level parallelism.
     * ILP = total_operations / num_vliw_words
     * Higher is better (max = NUM_UNITS = 4)
     */
    float compute_ilp(const vector<VLIWWord>& words) {
        // TODO: Implement
        return 0.0f;
    }

    /*
     * count_nops: Count total NOP slots across all VLIW words.
     */
    int count_nops(const vector<VLIWWord>& words) {
        // TODO: Count slots with -1
        return 0;
    }

    /*
     * print_schedule: Pretty-print the VLIW schedule.
     */
    void print_schedule(const vector<VLIWWord>& words) {
        printf("  VLIW Schedule:\n");
        printf("  %4s | %8s | %8s | %8s | %8s\n", "Word", "ALU", "MUL", "MEM", "BRANCH");
        printf("  %s\n", "-----+----------+----------+----------+---------");
        for (int i = 0; i < (int)words.size(); i++) {
            printf("  %4d |", i);
            for (int u = 0; u < NUM_UNITS; u++) {
                if (words[i].ops[u] >= 0)
                    printf(" %8s |", operations[words[i].ops[u]].name);
                else
                    printf(" %8s |", "NOP");
            }
            printf("\n");
        }
    }
};

// ============ TEST FRAMEWORK ============

int main() {
    printf("=== Q18: VLIW Instruction Packing ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Four independent ops on different units -> 1 VLIW word
    {
        total++;
        VLIWScheduler sched;
        sched.add_op("add", ALU, {1, 2}, {3});
        sched.add_op("mul", MUL, {4, 5}, {6});
        sched.add_op("ld",  MEM, {7}, {8});
        sched.add_op("br",  BRANCH, {}, {});

        auto words = sched.schedule();
        bool ok = (words.size() == 1);
        float ilp = sched.compute_ilp(words);
        printf("%s [4 independent ops -> %zu VLIW words, ILP=%.1f]\n",
               ok ? "PASS" : "FAIL", words.size(), ilp);
        if (ok) passed++;
    }

    // Test 2: RAW dependency forces serialization
    {
        total++;
        VLIWScheduler sched;
        sched.add_op("add", ALU, {1, 2}, {3});       // writes r3
        sched.add_op("mul", MUL, {3, 4}, {5}, {0});  // reads r3, depends on add

        auto words = sched.schedule();
        bool ok = (words.size() == 2);  // must be in different words
        printf("%s [RAW dependency -> %zu words (expected 2)]\n",
               ok ? "PASS" : "FAIL", words.size());
        if (ok) passed++;
    }

    // Test 3: Same functional unit forces serialization
    {
        total++;
        VLIWScheduler sched;
        sched.add_op("add1", ALU, {1}, {3});
        sched.add_op("add2", ALU, {2}, {4});  // same unit, no data dep

        auto words = sched.schedule();
        bool ok = (words.size() == 2);
        printf("%s [Same unit -> %zu words (expected 2)]\n",
               ok ? "PASS" : "FAIL", words.size());
        if (ok) passed++;
    }

    // Test 4: Mixed dependencies
    {
        total++;
        VLIWScheduler sched;
        sched.add_op("ld1",  MEM, {10}, {1});              // load r1
        sched.add_op("ld2",  MEM, {11}, {2});              // load r2 (same unit as ld1)
        sched.add_op("add",  ALU, {1, 2}, {3}, {0, 1});    // r3 = r1 + r2
        sched.add_op("mul",  MUL, {3, 3}, {4}, {2});       // r4 = r3 * r3
        sched.add_op("st",   MEM, {4}, {}, {3});            // store r4

        auto words = sched.schedule();
        // ld1 -> ld2 (same unit) -> add (needs ld1,ld2) -> mul (needs add) -> st (needs mul, same unit as ld)
        // Minimum words: ld1 | ld2+? | add | mul | st = at least 4 words
        bool ok = (words.size() >= 4 && words.size() <= 5);
        printf("%s [Mixed deps -> %zu VLIW words]\n", ok ? "PASS" : "FAIL", words.size());
        if (ok) passed++;
        sched.print_schedule(words);
    }

    // Test 5: NOP counting
    {
        total++;
        VLIWScheduler sched;
        sched.add_op("add", ALU, {1}, {2});  // only 1 op

        auto words = sched.schedule();
        int nops = sched.count_nops(words);
        bool ok = (nops == 3);  // 1 word, 3 empty slots
        printf("%s [NOP count = %d, expected 3]\n", ok ? "PASS" : "FAIL", nops);
        if (ok) passed++;
    }

    // Test 6: ILP calculation
    {
        total++;
        VLIWScheduler sched;
        sched.add_op("a1", ALU, {1}, {2});
        sched.add_op("m1", MUL, {3}, {4});
        sched.add_op("a2", ALU, {5}, {6});
        sched.add_op("m2", MUL, {7}, {8});

        auto words = sched.schedule();
        float ilp = sched.compute_ilp(words);
        // 4 ops in 2 words = ILP of 2.0
        bool ok = (fabs(ilp - 2.0f) < 0.1f);
        printf("%s [ILP = %.1f, expected 2.0]\n", ok ? "PASS" : "FAIL", ilp);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
