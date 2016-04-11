#include "debug.h"
#include "binaryformat.h"
#include "matrixalloc.h"
#include "matrixmultiply.h"


int main(int argc, char *argv[]) {

    if (argc < 4) {
        printf("Usage: %s m n outfile.bin\n", argv[0]);
        return 0;
    }

    double *A;
    int m = atoi(argv[1]);
    int n = atoi(argv[2]);
    char *outfile = argv[3];

    printf("Producing a std (%d x %d) matrix.\n", m, n);

    alloc_matrix(&A, m, n);
    std_mat(A, m, n);
    write_matrix_binaryformat(outfile, A, m, n);

    print_matrix(A, m, n);
    dealloc_matrix(A);
    return 0;
}