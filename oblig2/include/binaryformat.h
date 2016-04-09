#ifndef BINARY_FORMAT_H
#define BINARY_FORMAT_H

#include "stdio.h"
#include "stdlib.h"
#include "matrixalloc.h"

void read_matrix_binaryformat (char* filename, double*** matrix, int* num_rows, int* num_cols);

void write_matrix_binaryformat (char* filename, double** matrix, int num_rows, int num_cols);

#endif