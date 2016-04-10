#include "stdio.h"

#include "debug.h"
#include "binaryformat.h"
#include "matrixalloc.h"
#include "matrixmultiply.h"

int integration_sanity_check();

int main(int argc, char const *argv[]) {
    int error = 0;
    error += integration_sanity_check();
    return error;
}


int integration_sanity_check() {
    printf("Running: integration sanity check");
    const int m = 3;
    const int l = 3;
    const int n = 3;
    //const int MAX_VALUE = 10;

    double *A, *B, *C;
    alloc_matrix(&A, m, l);
    alloc_matrix(&B, l, n);
    alloc_matrix(&C, m, n);


    /* Generating Random Values for A & B Array*/
    std_mat(A, m, l);
    std_mat(B, l, n);

    /* Do C = A*B */
    multiply(A, B, C, m, l, n);

    double expected [9] = {30, 36, 42, 66, 81, 96, 102, 126, 150};
    for (int i = 0; i < m*n; i++) {
        if (is_near(expected[i], C[i], 0.0001) == 0){
            printf("\nError: multiply gives wrong result at element %d\n", i);
            printf("Expected: %f\n", expected[i]);
            printf("Acutal: %f\n", C[i]);
            return 1;
        }
    }

    dealloc_matrix(A, m);
    dealloc_matrix(B, l);
    dealloc_matrix(C, m);

    printf(" -> OK\n");
    return 0;
}