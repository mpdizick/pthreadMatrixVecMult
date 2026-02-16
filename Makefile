# Default target: build all programs
all: $(TARGETS)

# Utility programs (serial)
make_matrix: make_matrix.c
	$(CC) $(CFLAGS) -o make_matrix make_matrix.c

print_matrix: print_matrix.c
	$(CC) $(CFLAGS) -o print_matrix print_matrix.c

matrix_vector: matrix_vector.c
	$(CC) $(CFLAGS) -o matrix_vector matrix_vector.c

# Parallel program
pth_matrix_vector: pth_matrix_vector.c quinn.h timer.h
	$(CC) $(CFLAGS) -o pth_matrix_vector pth_matrix_vector.c $(LDFLAGS)

# Clean up compiled files
clean:
	rm -f $(TARGETS) *.o

# Clean data files
clean_data:
	rm -f *.mat

# Clean everything
clean_all: clean clean_data
	rm -f *.out

# Test with small matrices
test: all
	@echo "Creating test matrices..."
	./make_matrix A_test.mat 5 10
	./make_matrix X_test.mat 10 1
	@echo "\nMatrix A:"
	./print_matrix A_test.mat
	@echo "\nVector X:"
	./print_matrix X_test.mat
	@echo "\nSerial multiplication:"
	./matrix_vector A_test.mat X_test.mat Y1_test.mat
	./print_matrix Y1_test.mat
	@echo "\nParallel multiplication (2 threads):"
	./pth_matrix_vector A_test.mat X_test.mat Y2_test.mat 2
	./print_matrix Y2_test.mat

.PHONY: all clean clean_data clean_all test