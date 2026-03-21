"""
Q01 — Systolic Array Matrix Multiply Simulator [Medium]

Topic: Hardware Architecture — Systolic Arrays

Problem:
  Implement a weight-stationary systolic array simulation that performs
  matrix multiplication C = A * B.

  In a weight-stationary systolic array:
  - Each Processing Element (PE) holds one weight from matrix B
  - Input activations (from A) flow left-to-right with skew
  - Partial sums accumulate and flow top-to-bottom
  - After all cycles, the accumulated values form the output matrix C

Constraints:
  - Support arbitrary MxK * KxN matrix multiplication
  - Implement cycle-by-cycle simulation (not just naive matmul)
  - Handle input skewing (row i of A enters i cycles late)
  - The PE grid should be KxN (one PE per weight element)

Approach:
  1. Load weights from B into PE grid
  2. Create skewed input schedule for rows of A
  3. Step cycle by cycle: each PE multiplies its weight by input,
     adds to accumulated partial sum flowing from above
  4. After M + K - 1 + N - 1 cycles, drain outputs

Complexity: O(M * K * N) arithmetic ops spread across cycles
"""


class PE:
    """Processing Element for systolic array."""
    def __init__(self):
        self.weight = 0.0       # stationary weight from B
        self.acc = 0.0          # accumulated partial sum
        self.in_left = 0.0      # input from left neighbor (activation)
        self.in_top = 0.0       # input from top neighbor (partial sum)
        self.out_right = 0.0    # output to right neighbor
        self.out_bottom = 0.0   # output to bottom neighbor
        self.has_input = False
        self.has_top = False


# ============ YOUR IMPLEMENTATION ============

def naive_matmul(A, B, M, K, N, stats=None):
    """
    Reference implementation for verification.
    C[M][N] = A[M][K] * B[K][N]

    If stats is provided (a MatmulStats object), track:
      - stats.hbm_loads: +2 per multiply (one A element, one B element)
      - stats.hbm_stores: +1 per output element
      - stats.flops: +2 per multiply-add
    """
    C = [[0.0] * N for _ in range(M)]
    # TODO: Implement standard triple-loop matrix multiply
    # Track stats if provided
    return C


class SystolicArray:
    """
    Simulates a weight-stationary systolic array.

    The PE grid is K rows x N columns.
    Weight B[k][n] is loaded into PE[k][n].

    Inputs (rows of A) are fed from the left with skewing:
      - Row m of A enters starting at cycle m (skewed by row index)
      - Within each row, element k enters PE column 0 at cycle m + k

    Partial sums flow top to bottom:
      - PE[0][n] starts with 0 from above
      - PE[k][n] receives partial sum from PE[k-1][n]
      - After all K rows, the bottom PE[K-1][n] holds the final result
    """
    def __init__(self, K, N):
        self.K = K
        self.N = N
        self.grid = [[PE() for _ in range(N)] for _ in range(K)]

    def load_weights(self, B):
        """Load weights from matrix B into PE grid."""
        # TODO: Load B[k][n] into grid[k][n].weight
        pass

    def step(self, cycle_inputs, input_valid):
        """
        Execute one cycle of the systolic array.
        cycle_inputs[k] = activation value entering PE[k][0] from the left
        input_valid[k] = whether cycle_inputs[k] is valid
        """
        # TODO: For each PE in the grid:
        #   1. Read inputs (from left neighbor's out_right, or from cycle_inputs for column 0)
        #   2. Read top input (from top neighbor's out_bottom, or 0 for row 0)
        #   3. Compute: out_bottom = in_top + weight * in_left (if input valid)
        #               out_right = in_left (pass through)
        #   4. Store outputs for next cycle's neighbors
        #
        # HINT: Process from right-to-left, bottom-to-top to avoid overwriting
        #       inputs before they're read, OR use a double-buffer approach
        pass

    def get_output(self, col):
        """Get output from bottom row of PE grid for a given column."""
        # TODO: Return the partial sum output from PE[K-1][col]
        pass


def systolic_matmul(A, B, M, K, N):
    """
    Perform C = A * B using systolic array simulation.

    Steps:
      1. Create a K x N systolic array and load weights from B
      2. For each of (M + K - 1) cycles:
         - Determine which activation values to feed into the left column
         - Apply skewing: row m of A enters starting at cycle m
         - Step the systolic array
         - Check if any output row has completed (after K cycles of accumulation)
      3. Collect results into C
    """
    C = [[0.0] * N for _ in range(M)]
    # TODO: Implement systolic array matrix multiplication
    #
    # Key insight: Total cycles needed = M + K - 1
    # At cycle t, row m feeds A[m][t-m] into the array (if 0 <= t-m < K)
    # Output for row m appears at the bottom after cycle m + K - 1
    return C


# ============ TEST FRAMEWORK ============

def matrices_equal(A, B, M, N, eps=1e-5):
    for i in range(M):
        for j in range(N):
            if abs(A[i][j] - B[i][j]) > eps:
                return False
    return True


def print_matrix(name, mat, rows, cols):
    print(f"  {name}:")
    for i in range(rows):
        print("    " + " ".join(f"{mat[i][j]:.1f}" for j in range(cols)))


if __name__ == "__main__":
    print("=== Q01: Systolic Array Matrix Multiply ===\n")
    passed = 0
    total = 0

    # Test 1: 2x2 * 2x2
    total += 1
    A = [[1, 2], [3, 4]]
    B = [[5, 6], [7, 8]]
    C_naive = naive_matmul(A, B, 2, 2, 2)
    C_systolic = systolic_matmul(A, B, 2, 2, 2)
    expected = [[19, 22], [43, 50]]
    naive_ok = matrices_equal(C_naive, expected, 2, 2)
    systolic_ok = matrices_equal(C_systolic, expected, 2, 2)
    if naive_ok and systolic_ok:
        print("PASS [2x2 matmul]")
        passed += 1
    else:
        print("FAIL [2x2 matmul]")
        if not naive_ok:
            print("  Naive result incorrect")
        if not systolic_ok:
            print("  Systolic result incorrect")
            print_matrix("Expected", expected, 2, 2)
            print_matrix("Got", C_systolic, 2, 2)

    # Test 2: 2x3 * 3x4
    total += 1
    A = [[1, 2, 3], [4, 5, 6]]
    B = [[7, 8, 9, 10], [11, 12, 13, 14], [15, 16, 17, 18]]
    C_naive = naive_matmul(A, B, 2, 3, 4)
    C_systolic = systolic_matmul(A, B, 2, 3, 4)
    match = matrices_equal(C_naive, C_systolic, 2, 4)
    naive_ok = abs(C_naive[0][0] - 74.0) < 1e-5
    if match and naive_ok:
        print("PASS [2x3 * 3x4 matmul]")
        passed += 1
    else:
        print("FAIL [2x3 * 3x4 matmul]")
        if not naive_ok:
            print(f"  Naive C[0][0] expected 74, got {C_naive[0][0]:.1f}")
        if not match:
            print("  Systolic doesn't match naive")

    # Test 3: 4x4 identity
    total += 1
    A = [[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]]
    I = [[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]
    C = systolic_matmul(A, I, 4, 4, 4)
    ok = matrices_equal(A, C, 4, 4)
    print(f"{'PASS' if ok else 'FAIL'} [4x4 * Identity]")
    if ok:
        passed += 1

    # Test 4: 1x1
    total += 1
    A = [[3.0]]
    B = [[7.0]]
    C = systolic_matmul(A, B, 1, 1, 1)
    ok = abs(C[0][0] - 21.0) < 1e-5
    print(f"{'PASS' if ok else 'FAIL'} [1x1 scalar multiply]")
    if ok:
        passed += 1

    print(f"\nPassed {passed}/{total} tests")
    exit(0 if passed == total else 1)
