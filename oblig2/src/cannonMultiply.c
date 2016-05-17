#include "math.h"

#include "matrixalloc.h"
#include "matrixmultiply.h"
#include "mpi.h"


void cannonMultiply(int my_m, int my_l, int my_n, double *a, double *b, double *c, MPI_Comm comm) {
    int i;
    int npes, dims[2], periods[2];
    int myrank, my2drank, mycoords[2];
    int uprank, downrank, leftrank, rightrank;//, coords[2];
    int shiftsource, shiftdest;
    MPI_Status status;
    MPI_Comm comm_2d;


	/* Get the communicator related information */
    MPI_Comm_size(comm, &npes);
    MPI_Comm_rank(comm, &myrank);


	/* Set up the Cartesian topology */
    dims[0] = dims[1] = sqrt(npes);


	/* Set the periods for wraparound connections, 1 == true*/
    periods[0] = periods[1] = 1;


	/* Create the Cartesian topology, with rank reordering */
    MPI_Cart_create(comm, 2, dims, periods, 1, &comm_2d);

	/* Get the rank and coordinates with respect to the new topology */
    MPI_Comm_rank(comm_2d, &my2drank);
    MPI_Cart_coords(comm_2d, my2drank, 2, mycoords);


	/* Compute ranks of the up and left shifts */
    MPI_Cart_shift(comm_2d, 1, -1, &rightrank, &leftrank);
    MPI_Cart_shift(comm_2d, 0, -1, &downrank, &uprank);


	/* Perform the initial matrix alignment. First for A and then for B */
    MPI_Cart_shift(comm_2d, 1, -mycoords[0], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(a, my_m*my_l, MPI_DOUBLE, shiftdest,
        1, shiftsource, 1, comm_2d, &status);

    MPI_Cart_shift(comm_2d, 0, -mycoords[1], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(b, my_l*my_n, MPI_DOUBLE,
        shiftdest, 1, shiftsource, 1, comm_2d, &status);


	/* Get into the main computation loop */
    for (i=0; i<dims[0]; i++) {
        multiply(a, b, c, my_m, my_l, my_n); /*c=c+a*b*/

	    /* Shift matrix a left by one */
        MPI_Sendrecv_replace(a, my_m*my_l, MPI_DOUBLE,
            leftrank, 1, rightrank, 1, comm_2d, &status);

	    /* Shift matrix b up by one */
        MPI_Sendrecv_replace(b, my_l*my_n, MPI_DOUBLE,
            uprank, 1, downrank, 1, comm_2d, &status);
    }

    MPI_Comm_free(&comm_2d); /* Free up communicator */
}


void cannonDelegateWork(int num_procs, int m, int l, int n, double *A, double *B) {
    int tag = 1;
    int sqp = sqrt(num_procs - 1);
    int ioffsetA = 0, joffsetA = 0;
    int ioffsetB = 0, joffsetB = 0;
    for (int worker = 1; worker < num_procs; worker++) {
        int Pi = (worker-1) / sqp;
        int Pj = (worker-1) % sqp;

        /* Find dimensions of submatrices,
         * both actual sizes and the maximum for all workers*/
        int my_m = Pi < m % sqp ? m/sqp+1 : m/sqp;
        int my_l_a = Pj < l % sqp ? l/sqp+1 : l/sqp;

        int my_l_b = Pi < l % sqp ? l/sqp+1 : l/sqp;
        int my_n = Pj < n % sqp ? n/sqp+1 : n/sqp;

        int my_m_max = 0 < m % sqp ? m/sqp+1 : m/sqp;
        int my_l_max = 0 < l % sqp ? l/sqp+1 : l/sqp;
        int my_n_max = 0 < n % sqp ? n/sqp+1 : n/sqp;

        /* Allocate the max size */
        double* subA, *subB;
        alloc_matrix(&subA, my_m_max, my_l_max);
        alloc_matrix(&subB, my_l_max, my_n_max);

        /* Fill in only the actual size, rest is zero */
        for (int i = 0; i < my_m; i++) {
            for (int j = 0; j < my_l_a; j++) {
                subA[i*my_l_max + j] = A[(i+ioffsetA) * l + (j + joffsetA)];
            }
        }
        for (int i = 0; i < my_l_b; i++) {
            for (int j = 0; j < my_n; j++) {
                subB[i*my_n_max + j] = B[(i+ioffsetB) * n + (j + joffsetB)];
            }
        }

        /* Update offsets */
        if (worker % sqp == 0) {
            ioffsetA += my_m;
            ioffsetB += my_l_b;
            joffsetA = joffsetB = 0;
        } else {
            joffsetA += my_l_a;
            joffsetB += my_n;
        }

        MPI_Send(&my_m_max, 1, MPI_INT, worker, tag, MPI_COMM_WORLD);
        MPI_Send(&my_l_max, 1, MPI_INT, worker, tag, MPI_COMM_WORLD);
        MPI_Send(&my_n_max, 1, MPI_INT, worker, tag, MPI_COMM_WORLD);
        MPI_Send(subA, my_m_max*my_l_max, MPI_DOUBLE, worker, tag, MPI_COMM_WORLD);
        MPI_Send(subB, my_l_max*my_n_max, MPI_DOUBLE, worker, tag, MPI_COMM_WORLD);

        dealloc_matrix(subA);
        dealloc_matrix(subB);
    }
}


void cannonRecieveResults(int num_procs, int m, int n, double *C) {
    MPI_Status status;
    int tag = 1;
    int sqp = sqrt(num_procs - 1);

    /* Recive results */
    int ioffsetC = 0, joffsetC = 0;
    for (int worker = 1; worker < num_procs; worker++) {

        int Pi = (worker-1) / sqp;
        int Pj = (worker-1) % sqp;

        /* Find dimensions of submatrices,
         * both actual sizes and the maximum for all workers */
        int my_m = Pi < m % sqp ? m/sqp+1 : m/sqp;
        int my_n = Pj < n % sqp ? n/sqp+1 : n/sqp;

        int my_m_max = 0 < m % sqp ? m/sqp+1 : m/sqp;
        int my_n_max = 0 < n % sqp ? n/sqp+1 : n/sqp;


        /* Alloc. and recive max size */
        double* subC;
        alloc_matrix(&subC, my_m_max, my_n_max);
        MPI_Recv(subC, my_m_max*my_n_max, MPI_DOUBLE, worker, tag, MPI_COMM_WORLD, &status);

        /* Place in C only actaul size elements */
        for (int i = 0; i < my_m; i++) {
            for (int j = 0; j < my_n; j++) {
                C[(i+ioffsetC) * n + (j + joffsetC)] += subC[i*my_n_max + j];
            }
        }
        dealloc_matrix(subC);

        /* Update offsets */
        if (worker % sqp == 0) {
            ioffsetC += my_m;
            joffsetC = 0;
        } else {
            joffsetC += my_n;
        }
    }
}
