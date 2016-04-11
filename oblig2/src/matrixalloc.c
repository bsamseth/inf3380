#include "matrixalloc.h"

void alloc_matrix(double** A, int rows, int cols) {
    (*A) = malloc(rows * cols * sizeof **A );
    for (int i = 0; i < rows*cols; i++) {
        (*A)[i] = 0;
    }
}

void dealloc_matrix(double* A) {
    free(A);
}