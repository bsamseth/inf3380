#include "binaryformat.h"

void read_matrix_binaryformat (char* filename, double*** matrix, int* num_rows, int* num_cols) {

    FILE* fp = fopen (filename,"rb");
    fread (num_rows, sizeof(int), 1, fp);
    fread (num_cols, sizeof(int), 1, fp);

    alloc_matrix(matrix, (*num_rows), (*num_cols));
    for (size_t i = 0; i < (*num_rows); i++) {
        fread((*matrix)[i], sizeof(double), (*num_cols), fp);
    }
    fclose(fp);
}


void write_matrix_binaryformat (char* filename, double** matrix, int num_rows, int num_cols) {
    FILE *fp = fopen (filename,"wb");
    fwrite (&num_rows, sizeof(int), 1, fp);
    fwrite (&num_cols, sizeof(int), 1, fp);
    for (size_t i = 0; i < num_rows; i++) {
        fwrite (matrix[i], sizeof(double), num_cols, fp);
    }
    fclose (fp);
}