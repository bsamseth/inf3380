#ifndef CANNON_MULTIPLY_H
#define CANNON_MULTIPLY_H

#include "mpi.h"

void cannonMultiplyMaster(int my_rank, int num_procs, int sqp, int tag,
                    MPI_Status* status,
                    double* A, double* B, double* C,
                    int m, int l, int n);

void cannonMultiplyWorker(int my_rank, int num_procs, int sqp, int tag,
                    MPI_Status* status);

#endif
