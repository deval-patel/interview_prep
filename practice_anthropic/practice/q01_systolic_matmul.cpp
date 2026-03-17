/*
 * Q01 — Systolic Array Matrix Multiply Simulator [Medium]
 *
 * Topic: Hardware Architecture — Systolic Arrays
 *
 * Problem:
 *   Implement a weight-stationary systolic array simulation that performs
 *   matrix multiplication C = A * B.
 *
 *   In a weight-stationary systolic array:
 *   - Each Processing Element (PE) holds one weight from matrix B
 *   - Input activations (from A) flow left-to-right with skew
 *   - Partial sums accumulate and flow top-to-bottom
 *   - After all cycles, the accumulated values form the output matrix C
 *
 * Constraints:
 *   - Support arbitrary MxK * KxN matrix multiplication
 *   - Implement cycle-by-cycle simulation (not just naive matmul)
 *   - Handle input skewing (row i of A enters i cycles late)
 *   - The PE grid should be KxN (one PE per weight element)
 *
 * Approach:
 *   1. Load weights from B into PE grid
 *   2. Create skewed input schedule for rows of A
 *   3. Step cycle by cycle: each PE multiplies its weight by input,
 *      adds to accumulated partial sum flowing from above
 *   4. After M + K - 1 + N - 1 cycles, drain outputs
 *
 * Complexity: O(M * K * N) arithmetic ops spread across cycles
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>

using namespace std;

// Processing Element for systolic array
struct PE {
    float weight;       // stationary weight from B
    float acc;          // accumulated partial sum
    float in_left;      // input from left neighbor (activation)
    float in_top;       // input from top neighbor (partial sum)
    float out_right;    // output to right neighbor
    float out_bottom;   // output to bottom neighbor
    bool has_input;     // whether this PE received valid input this cycle
    bool has_top;       // whether this PE received valid top input
};

// ============ YOUR IMPLEMENTATION ============

/*
 * naive_matmul: Reference implementation for verification.
 * C[M][N] = A[M][K] * B[K][N]
 */
void naive_matmul(const vector<vector<float>>& A,
                  const vector<vector<float>>& B,
                  vector<vector<float>>& C,
                  int M, int K, int N) {
    // TODO: Implement standard triple-loop matrix multiply
}

/*
 * SystolicArray: Simulates a weight-stationary systolic array.
 *
 * The PE grid is K rows x N columns.
 * Weight B[k][n] is loaded into PE[k][n].
 *
 * Inputs (rows of A) are fed from the left with skewing:
 *   - Row m of A enters starting at cycle m (skewed by row index)
 *   - Within each row, element k enters PE column 0 at cycle m + k
 *
 * Partial sums flow top to bottom:
 *   - PE[0][n] starts with 0 from above
 *   - PE[k][n] receives partial sum from PE[k-1][n]
 *   - After all K rows, the bottom PE[K-1][n] holds the final result for C[m][n]
 */
struct SystolicArray {
    int K, N;  // grid dimensions
    vector<vector<PE>> grid;  // K x N PE grid

    SystolicArray(int K, int N) : K(K), N(N), grid(K, vector<PE>(N)) {}

    // Load weights from matrix B into PE grid
    void load_weights(const vector<vector<float>>& B) {
        // TODO: Load B[k][n] into grid[k][n].weight
        // Also initialize all accumulators and signals to 0
    }

    // Execute one cycle of the systolic array
    // cycle_inputs[k] = activation value entering PE[k][0] from the left this cycle
    //                    (NaN or 0 if no valid input)
    // cycle_top[n] = partial sum entering PE[0][n] from the top (always 0)
    void step(const vector<float>& cycle_inputs, const vector<bool>& input_valid) {
        // TODO: For each PE in the grid:
        //   1. Read inputs (from left neighbor's out_right, or from cycle_inputs for column 0)
        //   2. Read top input (from top neighbor's out_bottom, or 0 for row 0)
        //   3. Compute: out_bottom = in_top + weight * in_left (if input valid)
        //               out_right = in_left (pass through)
        //   4. Store outputs for next cycle's neighbors
        //
        // HINT: Process from right-to-left, bottom-to-top to avoid overwriting
        //       inputs before they're read, OR use a double-buffer approach
    }

    // Get output from bottom row of PE grid for a given column
    float get_output(int col) {
        // TODO: Return the partial sum output from PE[K-1][col]
        return 0.0f;
    }
};

/*
 * systolic_matmul: Perform C = A * B using systolic array simulation.
 *
 * Steps:
 *   1. Create a K x N systolic array and load weights from B
 *   2. For each of (M + K - 1) cycles:
 *      - Determine which activation values to feed into the left column
 *      - Apply skewing: row m of A enters starting at cycle m
 *      - Step the systolic array
 *      - Check if any output row has completed (after K cycles of accumulation)
 *   3. Collect results into C
 */
void systolic_matmul(const vector<vector<float>>& A,
                     const vector<vector<float>>& B,
                     vector<vector<float>>& C,
                     int M, int K, int N) {
    // TODO: Implement systolic array matrix multiplication
    //
    // Key insight: Total cycles needed = M + K - 1
    // At cycle t, row m feeds A[m][t-m] into the array (if 0 <= t-m < K)
    // Output for row m appears at the bottom after cycle m + K - 1
}

// ============ TEST FRAMEWORK ============

bool matrices_equal(const vector<vector<float>>& A,
                    const vector<vector<float>>& B,
                    int M, int N, float eps = 1e-5) {
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (fabs(A[i][j] - B[i][j]) > eps) return false;
    return true;
}

void print_matrix(const char* name, const vector<vector<float>>& M_mat, int rows, int cols) {
    printf("  %s:\n", name);
    for (int i = 0; i < rows; i++) {
        printf("    ");
        for (int j = 0; j < cols; j++) printf("%.1f ", M_mat[i][j]);
        printf("\n");
    }
}

int main() {
    printf("=== Q01: Systolic Array Matrix Multiply ===\n\n");
    int passed = 0, total = 0;

    // Test 1: 2x2 * 2x2
    {
        total++;
        vector<vector<float>> A = {{1, 2}, {3, 4}};
        vector<vector<float>> B = {{5, 6}, {7, 8}};
        vector<vector<float>> C_naive(2, vector<float>(2, 0));
        vector<vector<float>> C_systolic(2, vector<float>(2, 0));

        naive_matmul(A, B, C_naive, 2, 2, 2);
        systolic_matmul(A, B, C_systolic, 2, 2, 2);

        // Expected: C = [[19, 22], [43, 50]]
        vector<vector<float>> expected = {{19, 22}, {43, 50}};
        bool naive_ok = matrices_equal(C_naive, expected, 2, 2);
        bool systolic_ok = matrices_equal(C_systolic, expected, 2, 2);

        if (naive_ok && systolic_ok) {
            printf("PASS [2x2 matmul]\n");
            passed++;
        } else {
            printf("FAIL [2x2 matmul]\n");
            if (!naive_ok) printf("  Naive result incorrect\n");
            if (!systolic_ok) {
                printf("  Systolic result incorrect\n");
                print_matrix("Expected", expected, 2, 2);
                print_matrix("Got", C_systolic, 2, 2);
            }
        }
    }

    // Test 2: 2x3 * 3x4
    {
        total++;
        vector<vector<float>> A = {{1, 2, 3}, {4, 5, 6}};
        vector<vector<float>> B = {{7, 8, 9, 10}, {11, 12, 13, 14}, {15, 16, 17, 18}};
        vector<vector<float>> C_naive(2, vector<float>(4, 0));
        vector<vector<float>> C_systolic(2, vector<float>(4, 0));

        naive_matmul(A, B, C_naive, 2, 3, 4);
        systolic_matmul(A, B, C_systolic, 2, 3, 4);

        bool match = matrices_equal(C_naive, C_systolic, 2, 4);
        // Verify naive is correct: C[0][0] = 1*7+2*11+3*15 = 7+22+45 = 74
        bool naive_ok = (fabs(C_naive[0][0] - 74.0f) < 1e-5);

        if (match && naive_ok) {
            printf("PASS [2x3 * 3x4 matmul]\n");
            passed++;
        } else {
            printf("FAIL [2x3 * 3x4 matmul]\n");
            if (!naive_ok) printf("  Naive C[0][0] expected 74, got %.1f\n", C_naive[0][0]);
            if (!match) {
                printf("  Systolic doesn't match naive\n");
                print_matrix("Naive", C_naive, 2, 4);
                print_matrix("Systolic", C_systolic, 2, 4);
            }
        }
    }

    // Test 3: 4x4 identity
    {
        total++;
        vector<vector<float>> A = {{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
        vector<vector<float>> I = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
        vector<vector<float>> C(4, vector<float>(4, 0));

        systolic_matmul(A, I, C, 4, 4, 4);
        bool ok = matrices_equal(A, C, 4, 4);
        printf("%s [4x4 * Identity]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 4: 1x1
    {
        total++;
        vector<vector<float>> A = {{3.0f}};
        vector<vector<float>> B = {{7.0f}};
        vector<vector<float>> C(1, vector<float>(1, 0));
        systolic_matmul(A, B, C, 1, 1, 1);
        bool ok = fabs(C[0][0] - 21.0f) < 1e-5;
        printf("%s [1x1 scalar multiply]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
