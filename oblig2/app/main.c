#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#include "debug.h"
#include "binaryformat.h"
#include "matrixalloc.h"
#include "matrixmultiply.h"

void print_matrix(double** A, int m, int n);

int main() {

    const int m = 3;
    const int l = 3;
    const int n = 3;
    //const int MAX_VALUE = 10;

    double **A, **B, **C;
    alloc_matrix(&A, m, l);
    alloc_matrix(&B, l, n);
    alloc_matrix(&C, m, n);


    /* Generating Random Values for A & B Array*/
    // rand_mat(A, m, l, MAX_VALUE);
    // rand_mat(B, l, n, MAX_VALUE);
    std_mat(A, m, l);
    std_mat(B, l, n);

    /* Printing the matrices*/
    printf("Matrix A :\n");
    print_matrix(A, m, l);
    printf("Matrix B :\n");
    print_matrix(B, l, n);
    printf("Matrix C = A*B :\n");
    multiply(A, B, C, m, l, n);
    print_matrix(C, m, n);


    dealloc_matrix(A, m);
    dealloc_matrix(B, l);
    dealloc_matrix(C, m);
    return 0;
}
