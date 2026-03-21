"""
Q18 — VLIW Instruction Packing [Hard]

Topic: Hardware Architecture — VLIW

Problem:
  Implement a VLIW (Very Long Instruction Word) instruction packer.
  A VLIW processor has multiple functional units that execute in parallel.
  The compiler/scheduler must pack independent operations into single
  VLIW words, filling unused slots with NOPs.

  Functional units:
  - ALU (0): arithmetic/logic operations
  - MUL (1): multiplication operations
  - MEM (2): memory load/store operations
  - BRANCH (3): control flow operations

  Each VLIW word can contain at most one operation per functional unit.
  Operations have data dependencies that must be respected.

Constraints:
  - 4 functional unit slots per VLIW word
  - Operations have read/write register sets for dependency tracking
  - WAR, RAW, WAW hazards must be detected
  - Greedy packing: try to fill each VLIW word as much as possible

Key Concepts:
  - VLIW architecture
  - Instruction-level parallelism (ILP)
  - Data hazard detection (RAW, WAR, WAW)
  - NOP insertion for unfilled slots
"""

ALU = 0
MUL = 1
MEM = 2
BRANCH = 3
NUM_UNITS = 4
UNIT_NAMES = ["ALU", "MUL", "MEM", "BR "]


class Operation:
    def __init__(self, op_id, name, unit, reads, writes, deps=None):
        self.id = op_id
        self.name = name
        self.unit = unit
        self.reads = reads      # registers read
        self.writes = writes    # registers written
        self.deps = deps or []  # explicit dependencies


class VLIWWord:
    def __init__(self):
        self.ops = [-1] * NUM_UNITS  # -1 = NOP


# ============ YOUR IMPLEMENTATION ============

class VLIWScheduler:
    def __init__(self):
        self.operations = []

    def add_op(self, name, unit, reads, writes, deps=None):
        """Add an operation. Returns the operation ID."""
        # TODO: Create operation and add to list
        return -1

    def has_data_hazard(self, op_a_id, op_b_id):
        """
        Check if scheduling op_b in same VLIW word as op_a would cause a hazard.
        RAW: op_b reads a register that op_a writes
        WAW: op_b writes a register that op_a writes
        WAR: op_b writes a register that op_a reads
        """
        # TODO: Check for RAW, WAW, WAR hazards
        return False

    def can_pack_together(self, word, op_id, scheduled_ops):
        """
        Check if an operation can be added to a VLIW word.
        1. Functional unit slot not taken
        2. No data hazards with ops already in word
        3. All explicit deps satisfied (in scheduled_ops)
        """
        # TODO: Implement
        return False

    def schedule(self):
        """
        Pack all operations into VLIW words using greedy algorithm.
        """
        # TODO: Implement greedy VLIW packing
        return []

    def compute_ilp(self, words):
        """ILP = total_operations / num_vliw_words"""
        # TODO: Implement
        return 0.0

    def count_nops(self, words):
        """Count total NOP slots across all VLIW words."""
        # TODO: Count slots with -1
        return 0

    def print_schedule(self, words):
        print("  VLIW Schedule:")
        print(f"  {'Word':>4} | {'ALU':>8} | {'MUL':>8} | {'MEM':>8} | {'BRANCH':>8}")
        print(f"  {'-----':>4}-+-{'-'*8}-+-{'-'*8}-+-{'-'*8}-+-{'-'*8}")
        for i, w in enumerate(words):
            row = f"  {i:4d} |"
            for u in range(NUM_UNITS):
                if w.ops[u] >= 0:
                    row += f" {self.operations[w.ops[u]].name:>8} |"
                else:
                    row += f" {'NOP':>8} |"
            print(row)


# ============ TEST FRAMEWORK ============

if __name__ == "__main__":
    print("=== Q18: VLIW Instruction Packing ===\n")
    passed = 0
    total = 0

    # Test 1: Four independent ops on different units -> 1 VLIW word
    total += 1
    sched = VLIWScheduler()
    sched.add_op("add", ALU, [1, 2], [3])
    sched.add_op("mul", MUL, [4, 5], [6])
    sched.add_op("ld",  MEM, [7], [8])
    sched.add_op("br",  BRANCH, [], [])
    words = sched.schedule()
    ok = len(words) == 1
    ilp = sched.compute_ilp(words)
    print(f"{'PASS' if ok else 'FAIL'} [4 independent ops -> {len(words)} VLIW words, ILP={ilp:.1f}]")
    if ok: passed += 1

    # Test 2: RAW dependency forces serialization
    total += 1
    sched = VLIWScheduler()
    sched.add_op("add", ALU, [1, 2], [3])
    sched.add_op("mul", MUL, [3, 4], [5], [0])
    words = sched.schedule()
    ok = len(words) == 2
    print(f"{'PASS' if ok else 'FAIL'} [RAW dependency -> {len(words)} words (expected 2)]")
    if ok: passed += 1

    # Test 3: Same functional unit forces serialization
    total += 1
    sched = VLIWScheduler()
    sched.add_op("add1", ALU, [1], [3])
    sched.add_op("add2", ALU, [2], [4])
    words = sched.schedule()
    ok = len(words) == 2
    print(f"{'PASS' if ok else 'FAIL'} [Same unit -> {len(words)} words (expected 2)]")
    if ok: passed += 1

    # Test 4: Mixed dependencies
    total += 1
    sched = VLIWScheduler()
    sched.add_op("ld1",  MEM, [10], [1])
    sched.add_op("ld2",  MEM, [11], [2])
    sched.add_op("add",  ALU, [1, 2], [3], [0, 1])
    sched.add_op("mul",  MUL, [3, 3], [4], [2])
    sched.add_op("st",   MEM, [4], [], [3])
    words = sched.schedule()
    ok = 4 <= len(words) <= 5
    print(f"{'PASS' if ok else 'FAIL'} [Mixed deps -> {len(words)} VLIW words]")
    if ok: passed += 1
    sched.print_schedule(words)

    # Test 5: NOP counting
    total += 1
    sched = VLIWScheduler()
    sched.add_op("add", ALU, [1], [2])
    words = sched.schedule()
    nops = sched.count_nops(words)
    ok = nops == 3
    print(f"{'PASS' if ok else 'FAIL'} [NOP count = {nops}, expected 3]")
    if ok: passed += 1

    # Test 6: ILP calculation
    total += 1
    sched = VLIWScheduler()
    sched.add_op("a1", ALU, [1], [2])
    sched.add_op("m1", MUL, [3], [4])
    sched.add_op("a2", ALU, [5], [6])
    sched.add_op("m2", MUL, [7], [8])
    words = sched.schedule()
    ilp = sched.compute_ilp(words)
    ok = abs(ilp - 2.0) < 0.1
    print(f"{'PASS' if ok else 'FAIL'} [ILP = {ilp:.1f}, expected 2.0]")
    if ok: passed += 1

    print(f"\nPassed {passed}/{total} tests")
    exit(0 if passed == total else 1)
