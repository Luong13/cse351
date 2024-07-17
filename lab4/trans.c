/*
 * Luong Dinh, 2066624
CSE 351 Lab 4, Part 2 - Matrix Transposes
*/

/* 
 * trans.c - Natrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[M][N], int B[N][M]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1 KiB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "support/cachelab.h"

int is_transpose(int M, int N, int A[M][N], int B[N][M]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part I of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[M][N], int B[N][M]) {
	int a,b,c,d,e,f,g,h;
	if (M == 32 && N == 32) {
		for(int i = 0; i < M; i += 8){
			for(int j = 0; j < N; j += 8){
				for(int k = i; k < i + 8; k++){
					a = A[k][j];
					b = A[k][j+1];
					c = A[k][j+2];
					d = A[k][j+3];
					e = A[k][j+4];
					f = A[k][j+5];
					g = A[k][j+6];
					h = A[k][j+7];
                                        B[j][k] = a;
                                        B[j+1][k] = b;
                                        B[j+2][k] = c;
                                        B[j+3][k] = d;
                                        B[j+4][k] = e;
                                        B[j+5][k] = f;
                                        B[j+6][k] = g;
                                        B[j+7][k] = h;
				}
			}
		}
	} else if (M == 64 && N == 64) {
		for(int i = 0; i < M; i += 4){
                        for(int j = 0; j < N; j += 4){
                                for(int k = i; k < i + 4; k++){
                                        a = A[k][j];
                                        b = A[k][j+1];
                                        c = A[k][j+2];
                                        d = A[k][j+3];
                                        B[j][k] = a;
                                        B[j+1][k] = b;
                                        B[j+2][k] = c;
                                        B[j+3][k] = d;
                                }
                        }
                }

	}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[M][N], int B[N][M]) {
    int i, j, tmp;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[M][N], int B[N][M]) {
    int i, j;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

