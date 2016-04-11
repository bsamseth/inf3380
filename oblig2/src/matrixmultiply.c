#include "matrixmultiply.h"
#include "binaryformat.h"
#include "omp.h"

/*
 * Compute C = A * B.
 * Uses OpenMP to optimize the tripple loop.

void multiply(double* A, double* B, double* C, int m, int l, int n) {
    #pragma omp parallel
    {
        int i, j, k;
        #pragma omp for
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                double dot = 0;
                for (k = 0; k < l; k++) {
                    dot += A[i * l + k] * B[k * n + j];
                }
                C[i * n + j] += dot;
            }
        }
    }
}
*/

void multiply(double* A, double* B, double* C, int m, int l, int n) {
    double *B2;
    B2 = malloc(l*n * sizeof *B2);
    transpose(B, B2, l, m);
    #pragma omp parallel
    {
        int i, j, k;
        #pragma omp for
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                double dot = 0;
                for (k = 0; k < l; k++) {
                    dot += A[i * l + k] * B2[j * l + k];
                }
                C[i * n + j] += dot;
            }
        }
    }
    free(B2);
}

void transpose(double *A, double *B, int m, int n) {
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            B[j * m + i] = A[i * n + j];
        }
    }
}