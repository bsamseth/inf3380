#include "binaryformat.h"

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