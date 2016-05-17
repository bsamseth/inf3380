#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"

#include "debug.h"
#include "binaryformat.h"

void print_matrix(double* A, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            printf("%3.2f ", A[i*n+j]);
        printf("\n");
    }
}

void rand_mat(double* A, int m, int n, int MAX_VALUE) {
    /* Generating Random Values for A array*/
    srand(time(NULL));
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            A[i*n+j] = rand() % MAX_VALUE;
    }
}

void std_mat(double* A, int m, int n) {
    int count = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            A[i*n+j] = ++count;
    }
}

int is_near(double a, double b, double eps) {
    return fabs(a-b) < fabs(eps);
}

int check_equal_content(char *file1, char *file2) {
    int m_A, n_A, m_B, n_B;
    double *A, *B;

    read_matrix_binaryformat(file1, &A, &m_A, &n_A);
    read_matrix_binaryformat(file2, &B, &m_B, &n_B);

    if (m_A != m_B || n_A != n_B) {
        return 0;
    }
    for (int i = 0; i < m_A*n_A; i++) {
        if (!is_near(A[i], B[i], 0.001*A[i]))
            return 0;
    }
    return 1;
}
