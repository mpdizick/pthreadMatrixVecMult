/**
 * @file make_matrix.c
 * @author Mason Dizick (mpdizick@coastal.edu)
 * @brief Create binary matrix files with random double values.
 * 
 * This program generates a binary matrix file with specified dimensions.
 * Matrix values are random doubles between 0.0 and 10.0.
 * 
 * Binary file format:
 *   - First 4 bytes: number of rows (int)
 *   - Next 4 bytes: number of columns (int)
 *   - Remaining bytes: matrix data (doubles in row-major order)
 * 
 * @version 1.0
 * @date 2026-02-16
 * 
 * @copyright Copyright (c) 2026
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void Usage(char* prog_name);

int main(int argc, char* argv[]) {
    FILE* fp;
    int rows, cols;
    int i, total_elements;
    double* matrix;
    
    /* Check command line arguments */
    if (argc != 4) {
        Usage(argv[0]);
        exit(1);
    }
    
    /* Parse dimensions */
    rows = atoi(argv[2]);
    cols = atoi(argv[3]);
    
    /* Validate dimensions */
    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Error: rows and cols must be positive integers\n");
        Usage(argv[0]);
        exit(1);
    }
    
    /* Open file for writing */
    fp = fopen(argv[1], "wb");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", argv[1]);
        exit(1);
    }
    
    /* Write dimensions to file (as 4-byte integers) */
    if (fwrite(&rows, sizeof(int), 1, fp) != 1) {
        fprintf(stderr, "Error: Failed to write rows to file\n");
        fclose(fp);
        exit(1);
    }
    if (fwrite(&cols, sizeof(int), 1, fp) != 1) {
        fprintf(stderr, "Error: Failed to write cols to file\n");
        fclose(fp);
        exit(1);
    }
    
    /* Allocate matrix */
    total_elements = rows * cols;
    matrix = (double*)malloc(total_elements * sizeof(double));
    if (matrix == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory for matrix\n");
        fclose(fp);
        exit(1);
    }
    
    /* Initialize random seed */
    srand(time(NULL));
    
    /* Fill matrix with random values between 0.0 and 10.0 */
    for (i = 0; i < total_elements; i++) {
        matrix[i] = ((double)rand() / (double)RAND_MAX) * 10.0;
    }
    
    /* Write matrix data to file */
    if (fwrite(matrix, sizeof(double), total_elements, fp) != total_elements) {
        fprintf(stderr, "Error: Failed to write matrix data to file\n");
        free(matrix);
        fclose(fp);
        exit(1);
    }
    
    /* Clean up */
    free(matrix);
    fclose(fp);
    
    return 0;
}

/*-------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print usage message and exit
 */
void Usage(char* prog_name) {
    fprintf(stderr, "Usage: %s <file_name> <rows> <cols>\n", prog_name);
    fprintf(stderr, "  Creates a binary matrix file with random double values\n");
    fprintf(stderr, "  Example: %s A.mat 100 50\n", prog_name);
}