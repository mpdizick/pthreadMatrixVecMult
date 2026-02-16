/**
 * @file matrix_vector.c
 * @author Mason Dizick (mpdizick@coastal.edu)
 * @brief Serial matrix-vector multiplication using binary files.
 * 
 * This program performs matrix-vector multiplication: y = A * x
 * where A is an m×n matrix and x is an n×1 column vector.
 * 
 * The program reads both matrix A and vector x from binary files,
 * performs the multiplication serially (no parallelization), and
 * writes the result vector y to a binary file.
 * 
 * Includes error checking for dimension compatibility.
 * 
 * @version 1.0
 * @date 2026-02-16
 * 
 * @copyright Copyright (c) 2026
 * 
*/

#include <stdio.h>
#include <stdlib.h>

void Usage(char* prog_name);
int Read_matrix(char* filename, double** A_p, int* m_p, int* n_p);
void Mat_vect_mult(double A[], double x[], double y[], int m, int n);
int Write_vector(char* filename, double y[], int m);

int main(int argc, char* argv[]) {
    double *A = NULL, *x = NULL, *y = NULL;
    int m_A, n_A, m_x, n_x;
    
    /* Check command line arguments */
    if (argc != 4) {
        Usage(argv[0]);
        exit(1);
    }
    
    /* Read matrix A */
    if (Read_matrix(argv[1], &A, &m_A, &n_A) != 0) {
        fprintf(stderr, "Error: Failed to read matrix A from %s\n", argv[1]);
        exit(1);
    }
    
    /* Read vector x (must be a column vector) */
    if (Read_matrix(argv[2], &x, &m_x, &n_x) != 0) {
        fprintf(stderr, "Error: Failed to read vector x from %s\n", argv[2]);
        free(A);
        exit(1);
    }
    
    /* Check that x is a column vector */
    if (n_x != 1) {
        fprintf(stderr, "Error: x must be a column vector (n_x = %d, should be 1)\n", n_x);
        free(A);
        free(x);
        exit(1);
    }
    
    /* Check compatibility for matrix multiplication */
    if (n_A != m_x) {
        fprintf(stderr, "Error: Incompatible dimensions for multiplication\n");
        fprintf(stderr, "  Matrix A is %d x %d, Vector x is %d x 1\n", m_A, n_A, m_x);
        fprintf(stderr, "  Number of columns in A (%d) must equal number of rows in x (%d)\n", n_A, m_x);
        free(A);
        free(x);
        exit(1);
    }
    
    /* Allocate result vector y */
    y = (double*)malloc(m_A * sizeof(double));
    if (y == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory for result vector\n");
        free(A);
        free(x);
        exit(1);
    }
    
    /* Perform matrix-vector multiplication */
    Mat_vect_mult(A, x, y, m_A, n_A);
    
    /* Write result to file */
    if (Write_vector(argv[3], y, m_A) != 0) {
        fprintf(stderr, "Error: Failed to write result to %s\n", argv[3]);
        free(A);
        free(x);
        free(y);
        exit(1);
    }
    
    /* Clean up */
    free(A);
    free(x);
    free(y);
    
    return 0;
}

/*-------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print usage message
*/
void Usage(char* prog_name) {
    fprintf(stderr, "Usage: %s <file_A> <file_x> <file_y>\n", prog_name);
    fprintf(stderr, "  Multiplies matrix A by vector x and stores result in y\n");
    fprintf(stderr, "  All files are in binary matrix format\n");
    fprintf(stderr, "  Example: %s A.mat x.mat y.mat\n", prog_name);
}

/*-------------------------------------------------------------------
 * Function:  Read_matrix
 * Purpose:   Read a binary matrix file
 * In args:   filename
 * Out args:  A_p (pointer to matrix data), m_p (rows), n_p (cols)
 * Return:    0 on success, -1 on error
*/
int Read_matrix(char* filename, double** A_p, int* m_p, int* n_p) {
    FILE* fp;
    int rows, cols;
    double* A;
    
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1;
    }
    
    /* Read dimensions */
    if (fread(&rows, sizeof(int), 1, fp) != 1 ||
        fread(&cols, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    
    if (rows <= 0 || cols <= 0) {
        fclose(fp);
        return -1;
    }
    
    /* Allocate matrix */
    A = (double*)malloc(rows * cols * sizeof(double));
    if (A == NULL) {
        fclose(fp);
        return -1;
    }
    
    /* Read data */
    if (fread(A, sizeof(double), rows * cols, fp) != rows * cols) {
        free(A);
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    
    *A_p = A;
    *m_p = rows;
    *n_p = cols;
    
    return 0;
}

/*-------------------------------------------------------------------
 * Function:  Mat_vect_mult
 * Purpose:   Multiply matrix A by vector x to produce y
 * In args:   A (m x n matrix), x (n x 1 vector), m, n
 * Out arg:   y (m x 1 vector)
*/
void Mat_vect_mult(double A[], double x[], double y[], int m, int n) {
    int i, j;
    
    for (i = 0; i < m; i++) {
        y[i] = 0.0;
        for (j = 0; j < n; j++) {
            y[i] += A[i * n + j] * x[j];
        }
    }
}

/*-------------------------------------------------------------------
 * Function:  Write_vector
 * Purpose:   Write a vector to a binary file (as a column vector)
 * In args:   filename, y (vector data), m (length)
 * Return:    0 on success, -1 on error
*/
int Write_vector(char* filename, double y[], int m) {
    FILE* fp;
    int cols = 1;  /* Column vector */
    
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        return -1;
    }
    
    /* Write dimensions (m x 1) */
    if (fwrite(&m, sizeof(int), 1, fp) != 1 ||
        fwrite(&cols, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    
    /* Write data */
    if (fwrite(y, sizeof(double), m, fp) != m) {
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    return 0;
}