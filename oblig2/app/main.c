/**
 * @brief Matrix-Matrix Multiplication Using Cannon's algorithm
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
    int my_rank, num_procs;


    int tag = 123; // tags not used, dummy value
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
    int sqp = sqrt(num_procs - 1); // square root of number of processes.



    /* TIME EXECUTION */
    double my_time;
    MPI_Barrier(MPI_COMM_WORLD);  /*synchronize all processes*/
    my_time = MPI_Wtime();



    /* MASTER CODE */
    if (my_rank == 0) {

        /* Handle input. Need sqp*sqp + 1 == num_procs for Cannon's */
        if (sqp * sqp != num_procs - 1) {
            printf("Number of processes must be a square number + 1 ");
            printf(" (e.g. 5=(2*2+1)).\n");
            MPI_Finalize();
            return 0;
        }
        if (argc < 4) {
            printf("Usage: %s inFile_A inFile_B outfile\n", argv[0]);
            MPI_Finalize();
            return 0;
        }

        char* infile_A = argv[1];
        char* infile_B = argv[2];
        char* outfile = argv[3];

        double* A, *B, *C;
        int m, l, n, tmp_l;
        read_matrix_binaryformat(infile_A, &A, &m, &l);
        read_matrix_binaryformat(infile_B, &B, &tmp_l, &n);

        if (tmp_l != l) {
            printf("Columns of A not equal to rows of B: m=%d, l=%d, n=%d\n",
                    m, l, n);
            MPI_Finalize();
            return 1;
        }

        alloc_matrix(&C, m, n);

        cannonMultiplyMaster(my_rank, num_procs, sqp, tag, &status, A, B, C, m, l, n);

        /* Result matrix C is now complete, write to file */
        write_matrix_binaryformat(outfile, C, m, n);
        dealloc_matrix(C);

    }
    /* Worker Code */
    else {
        cannonMultiplyWorker(my_rank, num_procs, sqp, tag, &status);
    }



    /* CALCULATE TIME DATA */
    double max_time, min_time, avg_time;
    my_time = MPI_Wtime() - my_time;
    /* compute max, min, and average timing statistics */
    MPI_Reduce(&my_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&my_time, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&my_time, &avg_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
      avg_time /= num_procs;
      printf("Min: %lf  Max: %lf  Avg:  %lf\n", min_time, max_time,avg_time);
    }



    MPI_Finalize ();
    return 0;
}
