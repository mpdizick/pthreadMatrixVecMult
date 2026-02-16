/**
 * @file pth_matrix_vector.c
 * @author Mason Dizick (mpdizick@coastal.edu)
 * @brief Parallel matrix-vector multiplication using POSIX threads.
 * 
 * This program performs matrix-vector multiplication: y = A * x
 * where A is an m×n matrix and x is an n×1 column vector.
 * 
 * Uses pthreads for parallelization with Quinn's macros to distribute
 * rows evenly among threads. Includes timing measurements for both
 * overall execution and computation-only sections.
 * 
 * Timing data is output to stderr in CSV format:
 *   N,P,Time_Overall,Time_Work
 * 
 * @version 1.0
 * @date 2026-02-16
 * 
 * @copyright Copyright (c) 2026
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "quinn.h"
#include "timer.h"

/* Global variables */
int thread_count;
double *A = NULL;
double *x = NULL;
double *y = NULL;
int m, n;

/* Function prototypes */
void Usage(char* prog_name);
int Read_matrix(char* filename, double** A_p, int* m_p, int* n_p);
int Write_vector(char* filename, double y[], int m);
void* Pth_mat_vect(void* rank);

int main(int argc, char* argv[]) {
    int m_x, n_x;
    long thread;
    pthread_t* thread_handles;
    double start_total, end_total, start_work, end_work;
    
    /* Start overall timing */
    GET_TIME(start_total);
    
    /* Check command line arguments */
    if (argc != 5) {
        Usage(argv[0]);
        exit(1);
    }
    
    /* Get number of threads */
    thread_count = atoi(argv[4]);
    if (thread_count <= 0) {
        fprintf(stderr, "Error: Number of threads must be positive\n");
        exit(1);
    }
    
    /* Read matrix A */
    if (Read_matrix(argv[1], &A, &m, &n) != 0) {
        fprintf(stderr, "Error: Failed to read matrix A from %s\n", argv[1]);
        exit(1);
    }
    
    /* Read vector x */
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
    
    /* Check compatibility */
    if (n != m_x) {
        fprintf(stderr, "Error: Incompatible dimensions for multiplication\n");
        fprintf(stderr, "  Matrix A is %d x %d, Vector x is %d x 1\n", m, n, m_x);
        free(A);
        free(x);
        exit(1);
    }
    
    /* Allocate result vector */
    y = (double*)malloc(m * sizeof(double));
    if (y == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory for result vector\n");
        free(A);
        free(x);
        exit(1);
    }
    
    /* Allocate thread handles */
    thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
    if (thread_handles == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory for thread handles\n");
        free(A);
        free(x);
        free(y);
        exit(1);
    }
    
    /* Start work timing */
    GET_TIME(start_work);
    
    /* Create threads */
    for (thread = 0; thread < thread_count; thread++) {
        pthread_create(&thread_handles[thread], NULL, Pth_mat_vect, (void*)thread);
    }
    
    /* Join threads */
    for (thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
    
    /* End work timing */
    GET_TIME(end_work);
    
    /* Write result */
    if (Write_vector(argv[3], y, m) != 0) {
        fprintf(stderr, "Error: Failed to write result to %s\n", argv[3]);
        free(A);
        free(x);
        free(y);
        free(thread_handles);
        exit(1);
    }
    
    /* End overall timing */
    GET_TIME(end_total);
    
    /* Print timing results to stderr */
    fprintf(stderr, "%d,%d,%e,%e\n", m, thread_count, end_total - start_total, end_work - start_work);
    
    /* Clean up */
    free(A);
    free(x);
    free(y);
    free(thread_handles);
    
    return 0;
}

/*-------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print usage message
*/
void Usage(char* prog_name) {
    fprintf(stderr, "Usage: %s <file_A> <file_x> <file_y> <num_threads>\n", prog_name);
    fprintf(stderr, "  Multiplies matrix A by vector x using pthreads\n");
    fprintf(stderr, "  Stores result in y and prints timing to stderr\n");
    fprintf(stderr, "  Example: %s A.mat x.mat y.mat 4\n", prog_name);
}

/*-------------------------------------------------------------------
 * Function:  Read_matrix
 * Purpose:   Read a binary matrix file
*/
int Read_matrix(char* filename, double** A_p, int* m_p, int* n_p) {
    FILE* fp;
    int rows, cols;
    double* A;
    
    fp = fopen(filename, "rb");
    if (fp == NULL) return -1;
    
    if (fread(&rows, sizeof(int), 1, fp) != 1 ||
        fread(&cols, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    
    if (rows <= 0 || cols <= 0) {
        fclose(fp);
        return -1;
    }
    
    A = (double*)malloc(rows * cols * sizeof(double));
    if (A == NULL) {
        fclose(fp);
        return -1;
    }
    
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
 * Function:  Write_vector
 * Purpose:   Write a vector to binary file
*/
int Write_vector(char* filename, double y[], int m) {
    FILE* fp;
    int cols = 1;
    
    fp = fopen(filename, "wb");
    if (fp == NULL) return -1;
    
    if (fwrite(&m, sizeof(int), 1, fp) != 1 ||
        fwrite(&cols, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }
    
    if (fwrite(y, sizeof(double), m, fp) != m) {
        fclose(fp);
        return -1;
    }
    
    fclose(fp);
    return 0;
}

/*-------------------------------------------------------------------
 * Function:  Pth_mat_vect
 * Purpose:   Thread function for parallel matrix-vector multiplication
 *            Uses Quinn macros to distribute rows among threads
*/
void* Pth_mat_vect(void* rank) {
    long my_rank = (long)rank;
    int local_first_row, local_last_row;
    int i, j;
    
    /* Calculate row distribution using Quinn macros */
    local_first_row = BLOCK_LOW(my_rank, thread_count, m);
    local_last_row = BLOCK_HIGH(my_rank, thread_count, m);
    
    /* Compute assigned rows */
    for (i = local_first_row; i <= local_last_row; i++) {
        y[i] = 0.0;
        for (j = 0; j < n; j++) {
            y[i] += A[i * n + j] * x[j];
        }
    }
    
    return NULL;
}
