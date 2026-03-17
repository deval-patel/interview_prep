/*
 * Q17 — 2D Convolution with im2col [Medium-Hard]
 *
 * Topic: ML Context / Kernel Optimization
 *
 * Problem:
 *   Implement 2D convolution in two ways:
 *   1. Naive: Direct nested-loop convolution
 *   2. im2col: Transform input into a column matrix, then use matrix multiplication
 *
 *   The im2col approach converts convolution into a GEMM (General Matrix Multiply)
 *   operation, which maps perfectly to systolic arrays and is the standard approach
 *   on TPUs.
 *
 * Constraints:
 *   - Input: (H, W) single-channel image
 *   - Kernel: (KH, KW) filter
 *   - Support stride and padding
 *   - Output dimensions: ((H + 2*pad - KH) / stride + 1, (W + 2*pad - KW) / stride + 1)
 *
 * Key Concepts:
 *   - im2col transformation
 *   - Converting convolution to matrix multiplication (GEMM)
 *   - Why this is efficient on systolic arrays
 *   - Memory-compute tradeoff (im2col uses more memory but enables fast GEMM)
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cstring>

using namespace std;

// ============ YOUR IMPLEMENTATION ============

/*
 * compute_output_dim: Calculate output dimension for one axis.
 * output = (input + 2*padding - kernel) / stride + 1
 */
int compute_output_dim(int input_dim, int kernel_dim, int stride, int padding) {
    // TODO: Implement
    return 0;
}

/*
 * naive_conv2d: Direct nested-loop 2D convolution.
 *
 * For each output position (oh, ow):
 *   output[oh][ow] = sum over (kh, kw) of:
 *     input[oh*stride + kh - pad][ow*stride + kw - pad] * kernel[kh][kw]
 *   (treat out-of-bounds input as 0 — zero padding)
 */
void naive_conv2d(const vector<vector<float>>& input,    // [H][W]
                  const vector<vector<float>>& kernel,   // [KH][KW]
                  vector<vector<float>>& output,          // [OH][OW]
                  int H, int W, int KH, int KW,
                  int stride, int padding) {
    // TODO: Implement naive convolution
}

/*
 * im2col: Transform input into column matrix for GEMM-based convolution.
 *
 * For each output position (oh, ow), extract the KH*KW input patch
 * and lay it out as a row in the column matrix.
 *
 * col_matrix shape: [OH*OW, KH*KW]
 * col_matrix[oh*OW + ow][kh*KW + kw] = input[oh*stride + kh - pad][ow*stride + kw - pad]
 *
 * Then: output_flat = col_matrix * kernel_flat
 *   where kernel_flat is kernel reshaped to [KH*KW, 1]
 */
void im2col(const vector<vector<float>>& input,
            int H, int W, int KH, int KW,
            int stride, int padding,
            vector<vector<float>>& col_matrix,    // [OH*OW][KH*KW]
            int OH, int OW) {
    // TODO: Implement im2col transformation
    // For each output position, extract the input patch
    // Use 0 for out-of-bounds positions (padding)
}

/*
 * im2col_conv2d: Convolution using im2col + matrix multiplication.
 *
 * Steps:
 *   1. Create col_matrix using im2col
 *   2. Flatten kernel to 1D vector
 *   3. output_flat[i] = dot(col_matrix[i], kernel_flat)
 *   4. Reshape output_flat into output[OH][OW]
 */
void im2col_conv2d(const vector<vector<float>>& input,
                   const vector<vector<float>>& kernel,
                   vector<vector<float>>& output,
                   int H, int W, int KH, int KW,
                   int stride, int padding) {
    // TODO: Implement conv2d using im2col approach
}

// ============ TEST FRAMEWORK ============

bool matrices_close(const vector<vector<float>>& A,
                    const vector<vector<float>>& B,
                    int M, int N, float tol = 1e-4) {
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            if (fabs(A[i][j] - B[i][j]) > tol) return false;
    return true;
}

int main() {
    printf("=== Q17: 2D Convolution with im2col ===\n\n");
    int passed = 0, total = 0;

    // Test 1: Output dimension calculation
    {
        total++;
        int od = compute_output_dim(5, 3, 1, 0);  // 5x5 input, 3x3 kernel, stride 1, no pad
        bool ok = (od == 3);
        printf("%s [Output dim: 5,k=3,s=1,p=0 -> %d, expected 3]\n",
               ok ? "PASS" : "FAIL", od);
        if (ok) passed++;
    }

    // Test 2: Output dim with padding
    {
        total++;
        int od = compute_output_dim(5, 3, 1, 1);  // same padding
        bool ok = (od == 5);
        printf("%s [Output dim: 5,k=3,s=1,p=1 -> %d, expected 5]\n",
               ok ? "PASS" : "FAIL", od);
        if (ok) passed++;
    }

    // Test 3: Naive conv2d basic
    {
        total++;
        // 3x3 input, 2x2 kernel, stride 1, no padding -> 2x2 output
        vector<vector<float>> input = {{1,2,3},{4,5,6},{7,8,9}};
        vector<vector<float>> kernel = {{1,0},{0,1}};
        int OH = 2, OW = 2;
        vector<vector<float>> output(OH, vector<float>(OW, 0));
        naive_conv2d(input, kernel, output, 3, 3, 2, 2, 1, 0);
        // output[0][0] = 1*1 + 2*0 + 4*0 + 5*1 = 6
        // output[0][1] = 2*1 + 3*0 + 5*0 + 6*1 = 8
        // output[1][0] = 4*1 + 5*0 + 7*0 + 8*1 = 12
        // output[1][1] = 5*1 + 6*0 + 8*0 + 9*1 = 14
        bool ok = fabs(output[0][0]-6)<1e-4 && fabs(output[0][1]-8)<1e-4 &&
                  fabs(output[1][0]-12)<1e-4 && fabs(output[1][1]-14)<1e-4;
        printf("%s [Naive conv2d 3x3 input, 2x2 kernel]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 4: im2col matches naive
    {
        total++;
        int H = 6, W = 6, KH = 3, KW = 3;
        vector<vector<float>> input(H, vector<float>(W));
        vector<vector<float>> kernel(KH, vector<float>(KW));

        srand(42);
        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
                input[i][j] = (float)(rand() % 10);
        for (int i = 0; i < KH; i++)
            for (int j = 0; j < KW; j++)
                kernel[i][j] = (float)(rand() % 5);

        int OH = compute_output_dim(H, KH, 1, 0);
        int OW = compute_output_dim(W, KW, 1, 0);
        vector<vector<float>> out_naive(OH, vector<float>(OW, 0));
        vector<vector<float>> out_im2col(OH, vector<float>(OW, 0));

        naive_conv2d(input, kernel, out_naive, H, W, KH, KW, 1, 0);
        im2col_conv2d(input, kernel, out_im2col, H, W, KH, KW, 1, 0);

        bool ok = matrices_close(out_naive, out_im2col, OH, OW);
        printf("%s [im2col matches naive (6x6, 3x3 kernel)]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 5: With stride > 1
    {
        total++;
        int H = 6, W = 6, KH = 3, KW = 3, stride = 2;
        vector<vector<float>> input(H, vector<float>(W));
        vector<vector<float>> kernel(KH, vector<float>(KW, 1.0f));
        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
                input[i][j] = (float)(i * W + j + 1);

        int OH = compute_output_dim(H, KH, stride, 0);
        int OW = compute_output_dim(W, KW, stride, 0);
        vector<vector<float>> out_naive(OH, vector<float>(OW, 0));
        vector<vector<float>> out_im2col(OH, vector<float>(OW, 0));

        naive_conv2d(input, kernel, out_naive, H, W, KH, KW, stride, 0);
        im2col_conv2d(input, kernel, out_im2col, H, W, KH, KW, stride, 0);

        bool ok = matrices_close(out_naive, out_im2col, OH, OW);
        printf("%s [Stride=2, im2col matches naive]\n", ok ? "PASS" : "FAIL");
        if (ok) passed++;
    }

    // Test 6: With padding
    {
        total++;
        int H = 4, W = 4, KH = 3, KW = 3, stride = 1, pad = 1;
        vector<vector<float>> input(H, vector<float>(W));
        vector<vector<float>> kernel(KH, vector<float>(KW, 1.0f));
        for (int i = 0; i < H; i++)
            for (int j = 0; j < W; j++)
                input[i][j] = 1.0f;

        int OH = compute_output_dim(H, KH, stride, pad);
        int OW = compute_output_dim(W, KW, stride, pad);
        vector<vector<float>> out_naive(OH, vector<float>(OW, 0));
        vector<vector<float>> out_im2col(OH, vector<float>(OW, 0));

        naive_conv2d(input, kernel, out_naive, H, W, KH, KW, stride, pad);
        im2col_conv2d(input, kernel, out_im2col, H, W, KH, KW, stride, pad);

        bool ok = matrices_close(out_naive, out_im2col, OH, OW);
        // Corner should sum fewer 1s: output[0][0] = 4 (2x2 valid region)
        ok = ok && fabs(out_naive[0][0] - 4.0f) < 1e-4;
        printf("%s [Padding=1, im2col matches naive, corner=%.0f]\n",
               ok ? "PASS" : "FAIL", out_naive[0][0]);
        if (ok) passed++;
    }

    printf("\nPassed %d/%d tests\n", passed, total);
    return (passed == total) ? 0 : 1;
}
