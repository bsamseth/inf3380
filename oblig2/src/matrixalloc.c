#include "matrixalloc.h"

void alloc_matrix(double** A, int rows, int cols) {
    (*A) = (double*) malloc(sizeof(double)* rows * cols);
}

void dealloc_matrix(double* A, int rows) {
    free(A);
}