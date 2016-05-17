/**
 * @file
 * Matrix-Matrix Multiplication Using Cannon's Algorithm
 *
 * @author Bendik Samseth
 */

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "mpi.h"

#include "debug.h"
#include "binaryformat.h"
#include "matrixalloc.h"
#include "matrixmultiply.h"
#include "cannonMultiply.h"


int main(int argc, char* argv[]) {
    int tag = 1; // tags not used, dummy value
    int my_rank, num_procs;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
    int sqp = sqrt(num_procs - 1); // square root of number of processes.

    /* Need a sperate comm for just the workers
     * A value of MPI_UNDEFINED will cause the process
     * to not be part of the new comm.
     */
    int is_worker = my_rank != 0 ? 1 : MPI_UNDEFINED;
    MPI_Comm worker_comm;
    MPI_Comm_split(MPI_COMM_WORLD, is_worker, my_rank, &worker_comm);


    double *A, *B, *C;
    int m, l, n, tmp_l;
    /* MASTER CODE SETUP */
    if (my_rank == 0) {
        /* Handle input. Need sqp*sqp + 1 == num_procs for Cannon's */
        if (sqp * sqp != num_procs - 1) {
            printf("Number of processes must be a square number + 1 ");
            printf(" (e.g. 5=(2*2+1)).\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        if (argc < 4) {
            printf("Usage: mpirun -n p %s inFile_A inFile_B outfile_C\n", argv[0]);
            printf("Where number of processes p = n^2 + 1 for integer n.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        char* infile_A = argv[1];
        char* infile_B = argv[2];
        read_matrix_binaryformat(infile_A, &A, &m, &l);
        read_matrix_binaryformat(infile_B, &B, &tmp_l, &n);


        if (tmp_l != l) {
            printf("Columns of A not equal to rows of B\n");
            MPI_Abort(MPI_COMM_WORLD, 2);
            return 2;
        }
        alloc_matrix(&C, m, n);
    }

    /* Master code */
    if (my_rank == 0) {
        cannonDelegateWork(num_procs, m, l, n, A, B);
        cannonRecieveResults(num_procs, m, l, n, C);
    }
    /* Worker Code */
    else {
        int my_m, my_l, my_n;
        MPI_Recv(&my_m, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&my_l, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&my_n, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        double* a, *b, *c;
        alloc_matrix(&a, my_m, my_l);
        alloc_matrix(&b, my_l, my_n);
        alloc_matrix(&c, my_m, my_n);
        MPI_Recv(a, my_m*my_l, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(b, my_l*my_n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);

        cannonMultiply(my_m, my_l, my_n, a, b, c, worker_comm);

        MPI_Send(c, my_m*my_n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        dealloc_matrix(a);
        dealloc_matrix(b);
        dealloc_matrix(c);
    }

    if (my_rank == 0) {
        char* outfile = argv[3];
        write_matrix_binaryformat(outfile, C, m, n);
        dealloc_matrix(A);
        dealloc_matrix(B);
        dealloc_matrix(C);
    }

    MPI_Finalize ();
    return 0;
}
