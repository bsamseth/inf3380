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

void MatrixMatrixMultiply(int n, double *a, double *b, double *c, MPI_Comm comm);
void MatrixMultiply(int n, double *a, double *b, double *c);
void print_matrix(double *A, int m, int n);
void read_matrix_binaryformat (char* filename, double** matrix, int* num_rows, int* num_cols);
void alloc_matrix(double** A, int rows, int cols);
void dealloc_matrix(double* A);

int main(int argc, char* argv[]) {
    int my_rank, num_procs;


    int tag = 1; // tags not used, dummy value
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
    int sqp = sqrt(num_procs - 1); // square root of number of processes.


    /* Need a sperate comm for just the workers */
    int is_worker = my_rank != 0 ? 1 : MPI_UNDEFINED;
    MPI_Comm worker_comm;
    MPI_Comm_split(MPI_COMM_WORLD, is_worker, my_rank, &worker_comm);



    double* A, *B, *C;
    int m, l, n, tmp_l;
    /* MASTER CODE SETUP */
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
        // char* outfile = argv[3];

        read_matrix_binaryformat(infile_A, &A, &m, &l);
        read_matrix_binaryformat(infile_B, &B, &tmp_l, &n);

        if (tmp_l != l) {
            printf("Columns of A not equal to rows of B\n");
            MPI_Finalize();
            return 1;
        }
        alloc_matrix(&C, m, n);
    }

    MPI_Barrier(MPI_COMM_WORLD);  /*synchronize all processes*/


    /* Master code */
    if (my_rank == 0) {

        /* Delegate submatrices to workers */
        int ioffset = 0, joffset = 0;
        for (int worker = 1; worker < num_procs; worker++) {
            double* subA, *subB;
            alloc_matrix(&subA, n, n);
            alloc_matrix(&subB, n, n);

            int my_n = n / sqp;
            for (int i = 0; i < my_n; i++) {
                for (int j = 0; j < my_n; j++) {
                    subA[i*my_n + j] = A[(i+ioffset) * n + (j + joffset)];
                    subB[i*my_n + j] = B[(i+ioffset) * n + (j + joffset)];
                }
            }

            if (worker % sqp == 0) {
                ioffset += my_n;
                joffset = 0;
            } else {
                joffset += my_n;
            }

            MPI_Send(&my_n, 1, MPI_INT, worker, tag, MPI_COMM_WORLD);
            MPI_Send(subA, my_n*my_n, MPI_DOUBLE, worker, tag, MPI_COMM_WORLD);
            MPI_Send(subB, my_n*my_n, MPI_DOUBLE, worker, tag, MPI_COMM_WORLD);

            dealloc_matrix(subA);
            dealloc_matrix(subB);
        }

        /* Recive results */
        ioffset = joffset = 0;
        for (int worker = 1; worker < num_procs; worker++) {
            int my_n;
            MPI_Recv(&my_n, 1, MPI_INT, worker, tag, MPI_COMM_WORLD, &status);
            double* subC;
            alloc_matrix(&subC, my_n, my_n);
            MPI_Recv(subC, my_n*my_n, MPI_DOUBLE, worker, tag, MPI_COMM_WORLD, &status);

            for (int i = 0; i < my_n; i++) {
                for (int j = 0; j < my_n; j++) {
                    C[(i+ioffset) * n + (j + joffset)] += subC[i*my_n + j];
                }
            }
            dealloc_matrix(subC);

            if (worker % sqp == 0) {
                ioffset += my_n;
                joffset = 0;
            } else {
                joffset += my_n;
            }
        }

    }
    /* Worker Code */
    else {
        int my_n;
        MPI_Recv(&my_n, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        double* a, *b, *c;
        alloc_matrix(&a, my_n, my_n);
        alloc_matrix(&b, my_n, my_n);
        alloc_matrix(&c, my_n, my_n);
        MPI_Recv(a, my_n*my_n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(b, my_n*my_n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);

        MatrixMatrixMultiply(my_n, a, b, c, worker_comm);

        printf("Worker #%d calculated matrices:\n", my_rank);
        print_matrix(c, my_n, my_n);


        MPI_Send(&my_n, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
        MPI_Send(c, my_n*my_n, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);  /*synchronize all processes*/

    if (my_rank == 0) {
        printf("Result C = \n");
        print_matrix(C, n, n);
        dealloc_matrix(C);
    }


    MPI_Finalize ();
    return 0;
}





void MatrixMatrixMultiply(int n, double *a, double *b, double *c, MPI_Comm comm) {
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
    MPI_Sendrecv_replace(a, n*n, MPI_DOUBLE, shiftdest,
        1, shiftsource, 1, comm_2d, &status);

    MPI_Cart_shift(comm_2d, 0, -mycoords[1], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(b, n*n, MPI_DOUBLE,
        shiftdest, 1, shiftsource, 1, comm_2d, &status);


	/* Get into the main computation loop */
    for (i=0; i<dims[0]; i++) {
        MatrixMultiply(n, a, b, c); /*c=c+a*b*/

	    /* Shift matrix a left by one */
        MPI_Sendrecv_replace(a, n*n, MPI_DOUBLE,
            leftrank, 1, rightrank, 1, comm_2d, &status);

	    /* Shift matrix b up by one */
        MPI_Sendrecv_replace(b, n*n, MPI_DOUBLE,
            uprank, 1, downrank, 1, comm_2d, &status);
    }

	/* Restore the original distribution of a and b */
    MPI_Cart_shift(comm_2d, 1, +mycoords[0], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(a, n*n, MPI_DOUBLE,
        shiftdest, 1, shiftsource, 1, comm_2d, &status);
    MPI_Cart_shift(comm_2d, 0, +mycoords[1], &shiftsource, &shiftdest);
    MPI_Sendrecv_replace(b, n*n, MPI_DOUBLE,
        shiftdest, 1, shiftsource, 1, comm_2d, &status);
    MPI_Comm_free(&comm_2d); /* Free up communicator */
}


/* This function performs a serial matrix-matrix multiplication c = a*b */
void MatrixMultiply(int n, double *a, double *b, double *c)
{
    int i, j, k;
    for (i=0; i<n; i++)
        for (j=0; j<n; j++)
            for (k=0; k<n; k++)
                c[i*n+j] += a[i*n+k]*b[k*n+j];
}

void print_matrix(double* A, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            printf("%3f ", A[i*n+j]);
        printf("\n");
    }
}

void read_matrix_binaryformat (char* filename, double** matrix, int* num_rows, int* num_cols) {
    FILE* fp = fopen (filename,"rb");
    fread (num_rows, sizeof(int), 1, fp);
    fread (num_cols, sizeof(int), 1, fp);
    /* storage allocation of the matrix */
    alloc_matrix(matrix, *num_rows, *num_cols);
    /* read in the entire matrix */
    fread ((*matrix), sizeof(double), (*num_rows) * (*num_cols), fp);
    fclose (fp);
}

void write_matrix_binaryformat (char* filename, double* matrix, int num_rows, int num_cols) {
    FILE *fp = fopen (filename,"wb");
    fwrite (&num_rows, sizeof(int), 1, fp);
    fwrite (&num_cols, sizeof(int), 1, fp);
    fwrite (matrix, sizeof(double), num_rows*num_cols, fp);
    fclose (fp);
}

void alloc_matrix(double** A, int rows, int cols) {
    (*A) = malloc(rows * cols * sizeof **A );
    for (int i = 0; i < rows*cols; i++) {
        (*A)[i] = 0;
    }
}

void dealloc_matrix(double* A) {
    free(A);
}
