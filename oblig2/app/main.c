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

        /* Divide matrices into blocks and send to workers */
        int ioffsetA = 0; /* These variables will track the current */
        int joffsetA = 0; /* position inside the main matrices */
        int ioffsetB = 0;
        int joffsetB = 0;
        for (int worker = 1; worker < num_procs; worker++) {
            /* Determine coordinates in the process grid */
            int Pi = (worker-1) / sqp;
            int Pj = (worker-1) % sqp;

            /**
             *  Determine dimensions of submatrices.
             *  If m % sqp = a != 0, then let the first a workers have
             *  one extra row. Do the same for columns.
             *  This will, however, give different sizes for the allocated
             *  blocks for some of the workers, which will be truble
             *  when workers send/recive data from others. So, allocate
             *  the maximum for everybody, and simply don't use the empty values.
             */
            int m_A  = (Pi < m % sqp) ? m/sqp + 1 : m/sqp;
            int n_A  = (Pj < l % sqp) ? l/sqp + 1 : l/sqp;
            int lmax = (0  < l % sqp) ? l/sqp + 1 : l/sqp;
            int m_B  = (Pi < l % sqp) ? l/sqp + 1 : l/sqp;
            int n_B  = (Pj < n % sqp) ? n/sqp + 1 : n/sqp;

            /* Allocate sub-matrices */
            double *subA, *subB;
            alloc_matrix(&subA, m_A, lmax);
            alloc_matrix(&subB, lmax, n_B);

            /* Fill sub-matrices with correct block of values */
            for (int i = 0; i < m_A; i++) {
                for (int j = 0; j < n_A; j++) {
                    subA[i*n_A + j] = A[(i+ioffsetA) * n_A + j + joffsetA];
                }
            }

            for (int i = 0; i < m_B; i++) {
                for (int j = 0; j < n_B; j++) {
                    subB[i*n_B + j] = B[(i+ioffsetB) * n_B + j + joffsetB];
                }
            }

            /* Update offsets */
            if (worker % sqp == 0) {
                ioffsetA += m_A;
                ioffsetB += m_B;
                joffsetA = 0;
                joffsetB = 0;
            } else {
                joffsetA += n_A;
                joffsetB += n_B;
            }

            /**
             * Send information to workers.
             * Send m_A, lmax and n_B first (3 integers).
             * Then send subA (m_A x lmax) and subB (lmax x n_B)
             */
            MPI_Send(&m_A, 1, MPI_INT, worker, tag, MPI_COMM_WORLD);
            MPI_Send(&lmax, 1, MPI_INT, worker, tag, MPI_COMM_WORLD);
            MPI_Send(&n_B, 1, MPI_INT, worker, tag, MPI_COMM_WORLD);

            MPI_Send(subA, m_A*lmax, MPI_DOUBLE, worker, tag, MPI_COMM_WORLD);
            MPI_Send(subB, lmax*n_B, MPI_DOUBLE, worker, tag, MPI_COMM_WORLD);

            dealloc_matrix(subA);
            dealloc_matrix(subB);
        }

        dealloc_matrix(A);
        dealloc_matrix(B);

        /* Recive results from worker processes */
        int ioffsetC = 0;
        int joffsetC = 0;
        for (int worker = 1; worker < num_procs; worker++) {
            int m_C, n_C;
            MPI_Recv(&m_C, 1, MPI_INT, worker, tag, MPI_COMM_WORLD, &status);
            MPI_Recv(&n_C, 1, MPI_INT, worker, tag, MPI_COMM_WORLD, &status);

            double* subC;
            alloc_matrix(&subC, m_C, n_C);

            MPI_Recv(subC, m_C*n_C, MPI_DOUBLE, worker, tag, MPI_COMM_WORLD, &status);

            for (int i = 0; i < m_C; i++) {
                for (int j = 0; j < n_C; j++) {
                    C[(i+ioffsetC) * n + j + joffsetC] = subC[i * n_C + j];
                }
            }

            if (worker % sqp == 0) {
                ioffsetC += m_C;
                joffsetC = 0;
            } else {
                joffsetC += n_C;
            }

            dealloc_matrix(subC);
        }

        /* Result matrix C is now complete, write to file */
        write_matrix_binaryformat(outfile, C, m, n);
        dealloc_matrix(C);

    }
    /* Worker Code */
    else {

        /* Recive matrix dimensions from master */
        int m, l, n;
        MPI_Recv(&m, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&l, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&n, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        double *A, *B, *C;
        alloc_matrix(&A, m, l);
        alloc_matrix(&B, l, n);
        alloc_matrix(&C, m, n);

        MPI_Recv(A, m*l, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(B, l*n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);

        /**
         * Perform initial alignment.
         * Shift A_ij to the left by i steps (with wraparound)
         * Shift B_ij upwards by k steps (with wraparound)
         */

        /* Determine coordinates in the process grid */
        int Pi = (my_rank-1) / sqp;
        int Pj = (my_rank-1) % sqp;

        /* Determine dest and source for the shift */
        int left  = (Pj - Pi + sqp) % sqp;
        int right = (Pi + Pj) % sqp;
        int up    = (Pi - Pj + sqp) % sqp;
        int down  = (Pi + Pj) % sqp;

        int A_to   = Pi * sqp + left + 1;
        int A_from = Pi * sqp + right + 1;
        int B_to   = up * sqp + Pj + 1;
        int B_from = down * sqp + Pj + 1;

        /* Preform the shift, only if not already in place */
        if (A_to != my_rank) {
            MPI_Sendrecv_replace(A, m*l, MPI_DOUBLE, A_to, tag, A_from, tag,
                MPI_COMM_WORLD, &status);
        }
        if (B_to != my_rank) {
            MPI_Sendrecv_replace(B, l*n, MPI_DOUBLE, B_to, tag, B_from, tag,
                MPI_COMM_WORLD, &status);
        }

        /**
         * Now do sqp single shifts and multiplications.
         * Shift A_ij left by 1 (with wraparound)
         * Shift B_ij up by 1 (with wraparound)
         *
         * For this, shift has to be recalculated:
         */
         /* Determine dest and source for the shift */
         left  = (Pj - 1 + sqp) % sqp;
         right = (Pj + 1) % sqp;
         up    = (Pi - 1 + sqp) % sqp;
         down  = (Pi + 1) % sqp;

         A_to   = Pi * sqp + left + 1;
         A_from = Pi * sqp + right + 1;
         B_to   = up * sqp + Pj + 1;
         B_from = down * sqp + Pj + 1;

         for (int i = 0; i < sqp; i++) {
            multiply(A, B, C, m, l, n);

            MPI_Sendrecv_replace(A, m*l, MPI_DOUBLE, A_to, tag, A_from, tag, MPI_COMM_WORLD, &status);
            MPI_Sendrecv_replace(B, l*n, MPI_DOUBLE, B_to, tag, B_from, tag, MPI_COMM_WORLD, &status);
         }


         /* Done! Now send results to master */
         MPI_Send(&m, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
         MPI_Send(&n, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
         MPI_Send(C, m*n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);

         /* Release memory */
         dealloc_matrix(A);
         dealloc_matrix(B);
         dealloc_matrix(C);
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
