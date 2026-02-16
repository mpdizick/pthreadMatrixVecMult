/**
 * @file print_matrix.c
 * @author Mason Dizick (mpdizick@coastal.edu)
 * @brief Display binary matrix files in formatted output.
 * 
 * This program reads a binary matrix file and displays its contents
 * to the screen in a nicely formatted manner with 2 decimal places.
 * 
 * Binary file format:
 *   - First 4 bytes: number of rows (int)
 *   - Next 4 bytes: number of columns (int)
 *   - Remaining bytes: matrix data (doubles in row-major order)
 * 
 * Output format: XX.XX with 2 places before and after decimal
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

int main(int argc, char* argv[]) {
    FILE* fp;
    int rows, cols;
    int i, j;
    double* matrix;
    
    /* Check command line arguments */
    if (argc != 2) {
        Usage(argv[0]);
        exit(1);
    }
    
    /* Open file for reading */
    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open file %s for reading\n", argv[1]);
        exit(1);
    }
    
    /* Read dimensions */
    if (fread(&rows, sizeof(int), 1, fp) != 1) {
        fprintf(stderr, "Error: Failed to read rows from file\n");
        fclose(fp);
        exit(1);
    }
    if (fread(&cols, sizeof(int), 1, fp) != 1) {
        fprintf(stderr, "Error: Failed to read cols from file\n");
        fclose(fp);
        exit(1);
    }
    
    /* Validate dimensions */
    if (rows <= 0 || cols <= 0) {
        fprintf(stderr, "Error: Invalid dimensions in file (rows=%d, cols=%d)\n", rows, cols);
        fclose(fp);
        exit(1);
    }
    
    /* Allocate matrix */
    matrix = (double*)malloc(rows * cols * sizeof(double));
    if (matrix == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory for matrix\n");
        fclose(fp);
        exit(1);
    }
    
    /* Read matrix data */
    if (fread(matrix, sizeof(double), rows * cols, fp) != rows * cols) {
        fprintf(stderr, "Error: Failed to read matrix data from file\n");
        free(matrix);
        fclose(fp);
        exit(1);
    }
    
    fclose(fp);
    
    /* Print matrix dimensions */
    printf("Matrix: %d x %d\n", rows, cols);
    
    /* Print matrix with formatted output (XX.XX) */
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            printf("%05.2f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
    
    /* Clean up */
    free(matrix);
    
    return 0;
}

/*-------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print usage message
 */
void Usage(char* prog_name) {
    fprintf(stderr, "Usage: %s <file_name>\n", prog_name);
    fprintf(stderr, "  Prints a binary matrix file to the screen\n");
    fprintf(stderr, "  Example: %s A.mat\n", prog_name);
}
