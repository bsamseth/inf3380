#ifndef MATRIX_ALLOC_H
#define MATRIX_ALLOC_H

#include "stdio.h"
#include "stdlib.h"

void alloc_matrix(double** A, int rows, int cols);

void dealloc_matrix(double* A, int rows);

#endif