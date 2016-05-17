#ifndef BINARY_FORMAT_H
#define BINARY_FORMAT_H

#include "stdio.h"
#include "stdlib.h"
#include "matrixalloc.h"

/**
 * Read matrix from binary file. Will allocate memory for the matrix.
 * All inputs, except filename, will be changed.
 * @param filename Filename of binary file
 * @param matrix   Returns a double*, where values will be stored
 * @param num_rows Returns number of rows in the matrix
 * @param num_cols Returns number of columns in the matrix
 */
void read_matrix_binaryformat (char* filename, double** matrix, int* num_rows, int* num_cols);

/**
 * Write a matrix to file in binar format
 * @param filename File name to store data
 * @param matrix   1D array of the matrix elements
 * @param num_rows Number of rows in the matrix
 * @param num_cols Number of columns in the matrix
 */
void write_matrix_binaryformat (char* filename, double* matrix, int num_rows, int num_cols);

#endif
