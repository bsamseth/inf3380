#ifndef CANNON_MULTIPLY_H
#define CANNON_MULTIPLY_H

#include "mpi.h"

void cannonMultiply(int my_m, int my_l, int my_n, double *a, double *b, double *c, MPI_Comm comm);

void cannonDelegateWork(int num_procs, int m, int l, int n, double *A, double *B);

void cannonRecieveResults(int num_procs, int m, int l, int n, double *C);

#endif
