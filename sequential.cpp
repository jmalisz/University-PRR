#include <sequential.h>
#include <stdexcept>

// Perform Gaussian elimination to find matrix solution with sequential algorithm
void SequentialGaussianElimination(Matrix2D<double> &matrix)
{
	int matrix_rows = matrix.rows();
	int matrix_cols = matrix.cols();

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

		// Reduce elements other than diagonal to 0
		for (int row = 0; row < matrix_rows; row++)
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
