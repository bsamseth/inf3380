#include "matrixmultiply.h"
#include "binaryformat.h"


void multiply(double* A, double* B, double* C, int m, int l, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < l; k++) {
                C[i*m+j] += A[i*m+k]*B[k*l+j];
            }
        }
    }
}
