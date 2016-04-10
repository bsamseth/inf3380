#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "debug.h"

void print_matrix(double* A, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            printf("%3f ", A[i*m+j]);
        printf("\n");
    }
}

void rand_mat(double* A, int m, int n, int MAX_VALUE) {
    /* Generating Random Values for A array*/
    srand(time(NULL));
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            A[i*m+j] = rand() % MAX_VALUE;
    }
}

void std_mat(double* A, int m, int n) {
    int count = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            A[i*m+j] = ++count;
    }
}
