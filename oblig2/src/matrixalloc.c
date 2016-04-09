#include "matrixalloc.h"

void alloc_matrix(double*** A, int rows, int cols) {
    (*A) = (double**) malloc(rows * sizeof(double*));
    for (int i = 0; i < rows; i++)
        (*A)[i] = (double*) malloc(cols * sizeof(double));
}

void dealloc_matrix(double** A, int rows) {
    for (int i = 0; i < rows; i++) {
        free(A[i]);
    }
    free(A);
}