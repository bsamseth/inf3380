#include "debug.h"
#include "binaryformat.h"
#include "matrixalloc.h"
#include "matrixmultiply.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s infile.bin\n", argv[0]);
        return 0;
    }

    double *A;
    int m, n;
    char *input = argv[1];

    read_matrix_binaryformat(input, &A, &m, &n);
    print_matrix(A, m, n);
    dealloc_matrix(A);
    return 0;
}