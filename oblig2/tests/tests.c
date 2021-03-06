#include "stdio.h"

#include "debug.h"
#include "binaryformat.h"
#include "matrixalloc.h"
#include "matrixmultiply.h"

int alloc_multiply_test();
int fullTest();

int main(int argc, char *argv[]) {
    int error = 0;
    error += alloc_multiply_test();
    error += fullTest();
    return error;
}


int alloc_multiply_test() {
    printf("Running: allocation + multiplication test");
    const int m = 7;
    const int l = 8;
    const int n = 6;

    double *A, *B, *C;
    alloc_matrix(&A, m, l);
    alloc_matrix(&B, l, n);
    alloc_matrix(&C, m, n);

    /* Generating Standard Values for A & B Array */
    std_mat(A, m, l);
    std_mat(B, l, n);

    /* Do C = A*B */
    multiply(A, B, C, m, l, n);

    double expected [42] = {1044, 1080, 1116, 1152, 1188, 1224,
        2452, 2552, 2652, 2752, 2852, 2952,
        3860, 4024, 4188, 4352, 4516, 4680,
        5268, 5496, 5724, 5952, 6180, 6408,
        6676, 6968, 7260, 7552, 7844, 8136,
        8084, 8440, 8796, 9152, 9508, 9864,
        9492, 9912, 10332, 10752, 11172, 11592};

    for (int i = 0; i < m*n; i++) {
        if (is_near(expected[i], C[i], 0.0001) == 0){
            printf("\nError: multiply gives wrong result at element %d\n", i);
            printf("Expected: %f\n", expected[i]);
            printf("Acutal: %f\n", C[i]);
            printf("A = \n");
            print_matrix(A, m, l);
            printf("B = \n");
            print_matrix(B, l, n);
            printf("C = \n");
            print_matrix(C, m, n);
            return 1;
        }
    }

    dealloc_matrix(A);
    dealloc_matrix(B);
    dealloc_matrix(C);

    printf(" -> OK\n");
    return 0;
}


int fullTest() {
    printf("Running: full test (large_a x large_b = large_c)");
    system("mpirun -n 10 main.x ../input/large_matrix_a.bin ../input/large_matrix_b.bin out_from_unit_test.bin");

    char answer [50] = "out_from_unit_test.bin";
    char correct [50] = "../input/large_matrix_c.bin";

    if (!check_equal_content(answer, correct)) {
        printf("\nError: produced file is different from fasit\n");
        return 1;
    }
    printf(" -> OK\n");
    return 0;
}
