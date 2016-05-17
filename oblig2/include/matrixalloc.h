#ifndef MATRIX_ALLOC_H
#define MATRIX_ALLOC_H
/** @file */

#include "stdio.h"
#include "stdlib.h"

/**
 * Allocate and zero-initialize matrix of given size.
 * Returns a 1D pointer (matrix is flat in memory)
 * @param A    Return a double* to first element of matrix
 * @param rows Number of rows to be allocated
 * @param cols Number of column to be allocated
 */
void alloc_matrix(double** A, int rows, int cols);

/**
 * Deallocate a matrix.
 *
 * The current implementation of this is trivial,
 * but the method was used as a wrapper for any other
 * things that might be neccessary to do.
 * @param A Pointer to the matrix to be deallocated
 */
void dealloc_matrix(double* A);

#endif
