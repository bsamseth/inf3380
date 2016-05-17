#ifndef CANNON_MULTIPLY_H
#define CANNON_MULTIPLY_H
/** @file */

#include "mpi.h"

/**
 * Compute the block-result of c = a x b using Cannon's algorithm.
 *
 * This is done by using MPI's Cartesian topology, where all processes
 * are though of beeing in a virtual grid of (sqp x sqp) processes,
 * with sqp = sqrt(number of workers).
 *
 * First step:
 * 	Process P(i,j) sends matrix a i columns to the left (with wraparound)
 * 	and matrix b j rows up (with wraparound).
 *
 * Second step:
 * 	1:  Compute partial result c = a x b
 * 	2:  Process P(i,j) sends matrix a to P(i-1,j) and matrix b
 * 		to P(i,j-1) (with wraparound)
 *  3: While total multiplications are less than sqp, go to (second step, 1)
 *
 * The final result is then a block of total matrix C, stored in c.
 *
 * This algorithm assumes that all processes has equaly sized blocks.
 * If C is not evenly divided, then master will have padded out blocks
 * with zeros in order to ensure that this is the case.
 *
 * @param my_m Number of rows in matrix a/c
 * @param my_l Number of columns/rows in matrix a/b
 * @param my_n Number of columns in matrix b/c
 * @param a    Matrix a - left side matrix
 * @param b    Matrix b - right side matrix
 * @param c    Matrix c - result matrix
 * @param comm Communicator containing all the workers (not the master)
 */
void cannonMultiply(int my_m, int my_l, int my_n, double *a, double *b, double *c, MPI_Comm comm);


/**
 * Delegate blocks of matrix A and B to workers, so that they can compute
 * the block results (see cannonMultiply).
 *
 * This will give each worker a block of A and B.
 * In the case that not all blocks can be of equal size, all
 * blocks are padded with zeros so that all blocks of A are equaly sized, and
 * the same for blocks of B. This will at most mean 1 row and column of
 * zeros.
 *
 * @param num_procs Number of processes (including master)
 * @param m         Number of rows in A
 * @param l         Number of columns/rows in A/B
 * @param n         Number of columns of B
 * @param A         Matrix A to be partitioned
 * @param B         Matrix B to be partitioned
 */
void cannonDelegateWork(int num_procs, int m, int l, int n, double *A, double *B);


/**
 * Recive block results from all workers, and put the combined
 * result into matrix C
 *
 * In the case that some of the blocks have been padded with zeros,
 * only the parts of the result blocks that correspond to the parts of A- and B-blocks
 * that were not added as padding, are put into C.
 *
 * @param num_procs Number of processes (including master)
 * @param m         Number of rows in C
 * @param n         Number of columns in C
 * @param C         Matrix C to store results
 */
void cannonRecieveResults(int num_procs, int m, int n, double *C);

#endif
