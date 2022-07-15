#include <openmpi.h>
#include <iostream>
#include <mpi.h>
#include <stdexcept>
#include <thread>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <algorithm>

namespace
{
	const int ROOT = 0;
	int rank, size, row_col_num, chunk_size;

	void ReduceDiagonal(double matrix_slice[], double diagonal_row[], int diagonal_row_num)
	{
		// With augmented matrix
		int chunk_cols = row_col_num * 2;
		int chunk_rows = chunk_size / chunk_cols;

		// Reduce elements other than diagonal to 0
		for (int row = 0; row < chunk_rows; row++)
		{
			if (chunk_rows * rank + row == diagonal_row_num)
			{
				continue;
			}

			double ratio = matrix_slice[diagonal_row_num + row * chunk_cols];

			// Reduce row elements by pivot * factor
			for (int col = diagonal_row_num; col < chunk_cols; col++)
			{
				matrix_slice[col + row * chunk_cols] -= diagonal_row[col] * ratio;
			}
		}
	}
}

// Perform Gaussian elimination to find matrix solution with OpenMPI
void OpenMPIGaussianElimination(Matrix2D<double> &matrix)
{
	double full_array[matrix.size()];
	double chunk_array[chunk_size];

	for (int diagonal = 0; diagonal < row_col_num; diagonal++)
	{
		int pivot_row = diagonal;
		double pivot_value = matrix(pivot_row, diagonal);

		// Find better pivot if it exists
		for (int row = diagonal + 1; row < row_col_num; row++)
		{
			if (abs(matrix(row, diagonal)) > pivot_value)
			{
				pivot_row = row;
				pivot_value = matrix(row, diagonal);
			}
		}

		// If there's a 0 on a diagonal matrix is singular and doesn't have an inverse
		if (matrix(pivot_row, diagonal) == 0)
		{
			throw std::runtime_error("Error: provided matrix cannot be inverted");
		}

		// Swap the greatest value row with current row
		if (pivot_row != diagonal)
		{
			SwapRows(matrix, diagonal, pivot_row);
		}

		// Make pivot_value 1 and compute other cols
		for (int col = diagonal; col < row_col_num * 2; col++)
		{
			matrix(diagonal, col) /= pivot_value;
		}

		MPI_Bcast(matrix(diagonal).data(), row_col_num * 2, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

		matrix.populate_array(full_array);
		MPI_Scatter(full_array, chunk_size, MPI_DOUBLE,
					chunk_array, chunk_size, MPI_DOUBLE,
					ROOT, MPI_COMM_WORLD);

		ReduceDiagonal(chunk_array, matrix(diagonal).data(), diagonal);

		MPI_Gather(chunk_array, chunk_size, MPI_DOUBLE,
				   full_array, chunk_size, MPI_DOUBLE,
				   ROOT, MPI_COMM_WORLD);

		matrix.replace_by_array(full_array);
	}
}

// A simplified version of the handler program, accepts input only from file
int main(int argc, char **argv)
{
	using std::cout;
	using std::endl;

	Matrix2D<double> matrix;
	double chunk_array[chunk_size];

	MPI_Init(&argc, &argv); // initialize MPI environment

	try
	{
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		if (rank == 0)
		{
			cout << "Started root process" << endl;

			std::ofstream output_file("Output.txt", std::ios::trunc);

			matrix = ReadMatrixFromFile("Input.txt");
			matrix.augment(matrix, GenerateMatrix(kIdentity, matrix.rows()));
			chunk_size = matrix.size() / size;
			row_col_num = matrix.rows();

			// Send row_col_num and chunk_size info
			MPI_Bcast(&chunk_size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
			MPI_Bcast(&row_col_num, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

			auto start = std::chrono::system_clock::now();

			OpenMPIGaussianElimination(matrix);

			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> time_ms = end - start;

			WriteMatrixToStream(matrix, output_file);

			cout << std::setprecision(5);
			cout << "Time taken by algorithm: " << time_ms.count() << "ms" << endl;
		}
		else
		{
			cout << "Started worker process: rank " << rank << " out of " << size << endl;

			// Get chunk size from root for later
			MPI_Bcast(&chunk_size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
			// Get number of rows in matrix from root for iterations
			MPI_Bcast(&row_col_num, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

			double full_array[chunk_size * size];
			double chunk_array[chunk_size];
			double diagonal_row[row_col_num * 2];

			for (int i = 0; i < row_col_num; i++)
			{
				MPI_Bcast(diagonal_row, row_col_num * 2, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

				MPI_Scatter(full_array, chunk_size, MPI_DOUBLE,
							chunk_array, chunk_size, MPI_DOUBLE,
							ROOT, MPI_COMM_WORLD);

				ReduceDiagonal(chunk_array, diagonal_row, i);

				MPI_Gather(chunk_array, chunk_size, MPI_DOUBLE,
						   full_array, chunk_size, MPI_DOUBLE,
						   ROOT, MPI_COMM_WORLD);
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	MPI_Finalize();

	return 0;
}
