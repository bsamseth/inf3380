#ifndef DEBUG_FUNCTIONS_H
#define DEBUG_FUNCTIONS_H
/** @file */

/*
 * These functions serve to part of the application
 * and were only added for debugging purposes.s
 */
void print_matrix(double* A, int m, int n);
void rand_mat(double* A, int m, int n, int MAX_VALUE);
void std_mat(double* A, int m, int n);
int is_near(double a, double b, double eps);
int check_equal_content(char *file1, char *file2);

#endif
