#ifndef MATRIX_MULTIPLY_H
#define MATRIX_MULTIPLY_H
/** @file */


/**
 * Compute C = A * B.
 * Uses OpenMP to optimize the tripple loop.
 * In addition, B is transposed locally in order to
 * have more cache friendly multiplication.
 *
 * Results are added to each element of C, so
 * C is not set to zero before multiplication.
 *
 * @param A Left hand side matrix
 * @param B Right ahnd side matrix
 * @param C Resulting matrix
 * @param m Rows in matrix A
 * @param l Columns in A / Rows in B
 * @param n Columns in B
 */
void multiply(double* A, double* B, double* C, int m, int l, int n);

/**
 * Transpose matrix A, and store in B.
 * A is unchanged, and B = A^T.
 * A and B assumed to be preallocated.
 * @param A Matrix to transpose
 * @param B Matrix to store the transpose result
 * @param m Rows of A
 * @param n Columns of A
 */
void transpose(double *A, double *B, int m, int n);

#endif
