#include <threaded.h>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

namespace
{
	void ReduceDiagonal(Matrix2D<double> &matrix, int start_row, int end_row, int diagonal)
	{
		int matrix_cols = matrix.cols();

		// Reduce elements other than diagonal to 0
		for (int row = start_row; row < end_row; row++)
		{
			if (row == diagonal)
			{
				continue;
			}

			double ratio = matrix(row, diagonal);

			// Reduce row elements by pivot * factor
			for (int col = diagonal; col < matrix_cols; col++)
			{
				matrix(row, col) -= matrix(diagonal, col) * ratio;
			}
		}
	}
}

// Perform Gaussian elimination to find matrix solution with threads
void ThreadedGaussianElimination(Matrix2D<double> &matrix, int thread_number)
{
	int matrix_rows = matrix.rows();
	int matrix_cols = matrix.cols();

	std::vector<std::thread> threads;
	int rows_per_thread = matrix_rows / thread_number;

	for (int diagonal = 0; diagonal < matrix_rows; diagonal++)
	{
		int pivot_row = diagonal;
		double pivot_value = matrix(pivot_row, diagonal);

		// Find better pivot if it exists
		for (int row = diagonal + 1; row < matrix_rows; row++)
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
		for (int col = diagonal; col < matrix_cols; col++)
		{
			matrix(diagonal, col) /= pivot_value;
		}

		// TODO: Add thread pooling to improve performance
		for (int start_row = 0, end_row = rows_per_thread; start_row < matrix_rows;)
		{
			threads.push_back(std::thread(ReduceDiagonal, std::ref(matrix), start_row, end_row, diagonal));
			start_row += rows_per_thread;
			end_row += rows_per_thread;
		}
		for (std::thread &finished_thread : threads)
		{
			finished_thread.join();
		}

		threads.clear();
	}
}
