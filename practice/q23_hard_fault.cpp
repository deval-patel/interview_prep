/**
 * Q23: ARM Cortex-M Hard Fault Analysis
 *
 * PROBLEM:
 * Given a hard fault on ARM Cortex-M, what registers would you examine and why?
 *
 * BACKGROUND:
 * Hard faults occur when the processor encounters an unrecoverable error:
 * - Invalid memory access
 * - Division by zero (if trap enabled)
 * - Undefined instruction
 * - Stack overflow
 * - Bus errors
 *
 * KEY REGISTERS FOR DEBUGGING:
 *
 * 1. CFSR (Configurable Fault Status Register) - 0xE000ED28
 *    Contains UFSR, BFSR, and MMFSR
 *    - UFSR: Usage Fault Status (undefined instruction, etc.)
 *    - BFSR: Bus Fault Status (bus errors)
 *    - MMFSR: Memory Management Fault Status (MPU violations)
 *
 * 2. HFSR (Hard Fault Status Register) - 0xE000ED2C
 *    - VECTTBL: Fault on vector table read
 *    - FORCED: Fault escalated from other fault
 *    - DEBUGEVT: Debug event
 *
 * 3. MMFAR (MemManage Fault Address) - 0xE000ED34
 *    Address that caused MPU fault (if MMFSR.MMARVALID set)
 *
 * 4. BFAR (Bus Fault Address) - 0xE000ED38
 *    Address that caused bus fault (if BFSR.BFARVALID set)
 *
 * 5. Stacked registers (from exception frame):
 *    R0-R3, R12, LR, PC, xPSR
 *    PC is particularly important - shows instruction that faulted
 *
 * 6. SP (Stack Pointer)
 *    - MSP: Main Stack Pointer (used in handler mode)
 *    - PSP: Process Stack Pointer (used in thread mode if configured)
 *
 * DEBUGGING STEPS:
 * 1. Check HFSR to see if fault was escalated (FORCED bit)
 * 2. If FORCED, check CFSR for original fault type
 * 3. Check MMFAR/BFAR for faulting address (if valid)
 * 4. Examine stacked PC to find faulting instruction
 * 5. Check stack pointer for stack overflow
 */

#include <cstdint>
#include <cstdio>
#include <cstring>

// ============ SIMULATED ARM REGISTERS ============

// System Control Block register addresses (Cortex-M3/M4)
// In real embedded code you'd use: volatile uint32_t* reg = (volatile uint32_t*)SCB_CFSR;
[[maybe_unused]] const uint32_t SCB_BASE  = 0xE000ED00;
[[maybe_unused]] const uint32_t SCB_CFSR  = 0xE000ED28;  // Configurable Fault Status
[[maybe_unused]] const uint32_t SCB_HFSR  = 0xE000ED2C;  // Hard Fault Status
[[maybe_unused]] const uint32_t SCB_MMFAR = 0xE000ED34;  // MemManage Fault Address
[[maybe_unused]] const uint32_t SCB_BFAR  = 0xE000ED38;  // Bus Fault Address

// Simulated register values
static uint32_t g_cfsr = 0;
static uint32_t g_hfsr = 0;
static uint32_t g_mmfar = 0;
static uint32_t g_bfar = 0;

// Stacked exception frame
struct ExceptionFrame {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;    // Link Register (return address)
    uint32_t pc;    // Program Counter (faulting instruction)
    uint32_t xpsr;  // Program Status Register
};

static ExceptionFrame g_exception_frame = {};
static uint32_t g_sp_at_fault = 0;

// ============ FAULT STATUS BIT DEFINITIONS ============

// CFSR - Usage Fault Status Register (bits 16-25)
[[maybe_unused]] const uint32_t UFSR_DIVBYZERO  = (1 << 25); // Division by zero
[[maybe_unused]] const uint32_t UFSR_UNALIGNED  = (1 << 24); // Unaligned access
[[maybe_unused]] const uint32_t UFSR_NOCP       = (1 << 19); // No coprocessor
[[maybe_unused]] const uint32_t UFSR_INVPC      = (1 << 18); // Invalid PC load
[[maybe_unused]] const uint32_t UFSR_INVSTATE   = (1 << 17); // Invalid EPSR.T bit
[[maybe_unused]] const uint32_t UFSR_UNDEFINSTR = (1 << 16); // Undefined instruction

// CFSR - Bus Fault Status Register (bits 8-15)
[[maybe_unused]] const uint32_t BFSR_BFARVALID   = (1 << 15); // BFAR has valid address
[[maybe_unused]] const uint32_t BFSR_STKERR      = (1 << 12); // Stack push error
[[maybe_unused]] const uint32_t BFSR_UNSTKERR    = (1 << 11); // Stack pop error
[[maybe_unused]] const uint32_t BFSR_IMPRECISERR = (1 << 10); // Imprecise bus error
[[maybe_unused]] const uint32_t BFSR_PRECISERR   = (1 << 9);  // Precise bus error
[[maybe_unused]] const uint32_t BFSR_IBUSERR     = (1 << 8);  // Instruction bus error

// CFSR - MemManage Fault Status Register (bits 0-7)
[[maybe_unused]] const uint32_t MMFSR_MMARVALID  = (1 << 7); // MMFAR has valid address
[[maybe_unused]] const uint32_t MMFSR_MSTKERR    = (1 << 4); // Stack push MPU error
[[maybe_unused]] const uint32_t MMFSR_MUNSTKERR  = (1 << 3); // Stack pop MPU error
[[maybe_unused]] const uint32_t MMFSR_DACCVIOL   = (1 << 1); // Data access violation
[[maybe_unused]] const uint32_t MMFSR_IACCVIOL   = (1 << 0); // Instruction access violation

// HFSR - Hard Fault Status Register
[[maybe_unused]] const uint32_t HFSR_DEBUGEVT = (1u << 31); // Debug event
                 const uint32_t HFSR_FORCED   = (1 << 30);  // Forced hard fault (escalated)
[[maybe_unused]] const uint32_t HFSR_VECTTBL  = (1 << 1);   // Vector table read error

// ============ YOUR IMPLEMENTATION ============

/**
 * Analyze hard fault and print diagnostic information.
 *
 * This is what you'd call from your HardFault_Handler.
 */
void analyze_hard_fault() {
    // TODO: Implement fault analysis
    //
    // Steps:
    // 1. Print HFSR analysis
    // 2. If FORCED bit set, analyze CFSR
    // 3. Print MMFAR/BFAR if valid
    // 4. Print exception frame (especially PC)
    // 5. Suggest likely cause

    printf("\n");
    printf("========================================\n");
    printf("        HARD FAULT ANALYSIS\n");
    printf("========================================\n\n");

    // TODO: Add your analysis here

}

/**
 * Determine fault type from register values.
 *
 * @return String description of fault type
 */
const char* get_fault_type() {
    // TODO: Implement
    // Check CFSR bits to determine fault type

    return "Unknown";  // Placeholder
}

/**
 * Get faulting address (if available).
 *
 * @param address Output: the faulting address
 * @return true if address is valid
 */
bool get_fault_address(uint32_t* address) {
    // TODO: Implement
    // Check if BFAR or MMFAR is valid and return the address

    return false;  // Placeholder
}

/**
 * Get the PC (program counter) at time of fault.
 */
uint32_t get_fault_pc() {
    return g_exception_frame.pc;
}

/**
 * Check if fault might be stack overflow.
 */
bool is_likely_stack_overflow() {
    // TODO: Implement
    // Check for:
    // - Stack error bits in CFSR
    // - SP value near stack bottom
    // - PC in typical stack overflow patterns

    return false;  // Placeholder
}

/**
 * Print register dump in standard format.
 */
void print_register_dump() {
    printf("Exception Frame:\n");
    printf("  R0  = 0x%08X\n", g_exception_frame.r0);
    printf("  R1  = 0x%08X\n", g_exception_frame.r1);
    printf("  R2  = 0x%08X\n", g_exception_frame.r2);
    printf("  R3  = 0x%08X\n", g_exception_frame.r3);
    printf("  R12 = 0x%08X\n", g_exception_frame.r12);
    printf("  LR  = 0x%08X\n", g_exception_frame.lr);
    printf("  PC  = 0x%08X  <- Faulting instruction\n", g_exception_frame.pc);
    printf("  xPSR= 0x%08X\n", g_exception_frame.xpsr);
    printf("\n");
    printf("  SP  = 0x%08X\n", g_sp_at_fault);
}

/**
 * Print fault status registers.
 */
void print_fault_registers() {
    printf("Fault Status Registers:\n");
    printf("  HFSR  = 0x%08X\n", g_hfsr);
    printf("  CFSR  = 0x%08X\n", g_cfsr);
    printf("  MMFAR = 0x%08X\n", g_mmfar);
    printf("  BFAR  = 0x%08X\n", g_bfar);
    printf("\n");
}

// ============ FAULT SIMULATION ============

// Simulate different fault scenarios

void simulate_null_pointer_dereference() {
    printf("=== Simulating NULL Pointer Dereference ===\n");

    // Set up registers as if we dereferenced NULL
    g_hfsr = HFSR_FORCED;
    g_cfsr = BFSR_PRECISERR | BFSR_BFARVALID;
    g_bfar = 0x00000000;  // NULL address
    g_mmfar = 0;

    g_exception_frame.pc = 0x08001234;  // Where fault occurred
    g_exception_frame.lr = 0x08001100;  // Caller
    g_exception_frame.r0 = 0x00000000;  // NULL pointer
    g_sp_at_fault = 0x20007F00;

    analyze_hard_fault();
}

void simulate_stack_overflow() {
    printf("=== Simulating Stack Overflow ===\n");

    g_hfsr = HFSR_FORCED;
    g_cfsr = BFSR_STKERR;  // Stack push error
    g_bfar = 0x20000000;   // Stack bottom
    g_mmfar = 0;

    g_exception_frame.pc = 0x08002000;
    g_exception_frame.lr = 0x08002000;  // Recursive function
    g_sp_at_fault = 0x20000010;         // Very low SP

    analyze_hard_fault();
}

void simulate_undefined_instruction() {
    printf("=== Simulating Undefined Instruction ===\n");

    g_hfsr = HFSR_FORCED;
    g_cfsr = UFSR_UNDEFINSTR;
    g_bfar = 0;
    g_mmfar = 0;

    g_exception_frame.pc = 0x08003456;
    g_exception_frame.lr = 0x08003400;
    g_sp_at_fault = 0x20007F00;

    analyze_hard_fault();
}

void simulate_division_by_zero() {
    printf("=== Simulating Division by Zero ===\n");

    g_hfsr = HFSR_FORCED;
    g_cfsr = UFSR_DIVBYZERO;
    g_bfar = 0;
    g_mmfar = 0;

    g_exception_frame.pc = 0x08004000;
    g_exception_frame.lr = 0x08003F00;
    g_exception_frame.r1 = 0;  // Divisor was zero
    g_sp_at_fault = 0x20007F00;

    analyze_hard_fault();
}

void simulate_unaligned_access() {
    printf("=== Simulating Unaligned Access ===\n");

    g_hfsr = HFSR_FORCED;
    g_cfsr = UFSR_UNALIGNED;
    g_bfar = 0;
    g_mmfar = 0;

    g_exception_frame.pc = 0x08005000;
    g_exception_frame.lr = 0x08004F00;
    g_exception_frame.r0 = 0x20001001;  // Unaligned address
    g_sp_at_fault = 0x20007F00;

    analyze_hard_fault();
}

// ============ TEST FRAMEWORK ============

void print_debugging_guide() {
    printf("\n=== Hard Fault Debugging Guide ===\n\n");

    printf("1. Key Registers to Check:\n");
    printf("   HFSR  (0xE000ED2C) - Why hard fault occurred\n");
    printf("   CFSR  (0xE000ED28) - Detailed fault status\n");
    printf("   MMFAR (0xE000ED34) - Memory fault address\n");
    printf("   BFAR  (0xE000ED38) - Bus fault address\n");
    printf("   PC    (stacked)    - Faulting instruction\n\n");

    printf("2. Common Fault Causes:\n");
    printf("   - NULL pointer dereference -> BFSR.PRECISERR, BFAR=0\n");
    printf("   - Stack overflow -> BFSR.STKERR, SP near stack bottom\n");
    printf("   - Invalid function pointer -> UFSR.INVPC or UNDEFINSTR\n");
    printf("   - Unaligned access -> UFSR.UNALIGNED\n");
    printf("   - MPU violation -> MMFSR.DACCVIOL/IACCVIOL\n\n");

    printf("3. Getting More Info:\n");
    printf("   - Use PC to find faulting function in .map file\n");
    printf("   - Use LR to find caller\n");
    printf("   - Check if SP is valid (within stack bounds)\n");
    printf("   - Look at R0-R3 for function arguments\n\n");

    printf("4. Prevention:\n");
    printf("   - Enable div-by-zero and unaligned traps\n");
    printf("   - Use stack canaries\n");
    printf("   - Enable MPU protection\n");
    printf("   - Use static analysis\n\n");
}

int main() {
    printf("Running Q23: ARM Hard Fault Analysis\n");
    printf("=====================================\n\n");

    // Run simulations
    simulate_null_pointer_dereference();
    printf("\n");

    simulate_stack_overflow();
    printf("\n");

    simulate_undefined_instruction();
    printf("\n");

    simulate_division_by_zero();
    printf("\n");

    simulate_unaligned_access();
    printf("\n");

    print_debugging_guide();

    printf("=====================================\n");
    printf("Review the fault analysis output above.\n");
    printf("Implement analyze_hard_fault() to provide useful diagnostics.\n");

    return 0;
}
