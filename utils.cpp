#include "utils.h"
#include <fstream>
#include <iomanip>
#include <random>
#include <stdexcept>
#include <string>

// Read matrix from file in current directory. Expects matrix size as first number.
Matrix2D<double> ReadMatrixFromFile(std::string file_name)
{
	std::ifstream input(file_name);

	double matrix_size = 0;
	input >> matrix_size;

	Matrix2D<double> matrix(matrix_size, matrix_size);

	for (int i = 0; i < matrix_size; i++)
	{
		for (int j = 0; j < matrix_size; j++)
		{
			input >> matrix(i, j);
		}
	}

	if (matrix.size() != pow(matrix_size, 2))
	{
		throw std::runtime_error("Error: provided matrix isn't square and cannot be inverted");
	}

	return matrix;
}

// Write matrix solution to provided stream
void WriteMatrixToStream(Matrix2D<double> &matrix, std::ostream &stream)
{
	stream << std::setprecision(2);

	for (int i = 0; i < matrix.rows(); ++i)
	{
		for (int j = 0; j < matrix.cols(); ++j)
		{
			stream << std::setw(10) << matrix(i, j);
		}

		stream << std::endl;
	}

	stream << std::endl;
}

// Function that generates a matrix - random or identity
Matrix2D<double> GenerateMatrix(MatrixType matrix_type, int matrix_size)
{
	Matrix2D<double> new_matrix(matrix_size);

	// Obtain a random number from hardware
	std::random_device rd;
	// Seed the generator
	std::mt19937 gen(rd());
	// Define the range
	std::uniform_int_distribution<> dis(0, 100);

	for (int i = 0; i < matrix_size; ++i)
	{
		for (int j = 0; j < matrix_size; ++j)
		{
			if (matrix_type == kRandom)
			{
				new_matrix(i, j) = dis(gen);
			}
			else if (matrix_type == kIdentity)
			{
				new_matrix(i, j) = 0;
			}
		}

		if (matrix_type == kIdentity)
		{
			new_matrix(i, i) = 1;
		}
	}

	return new_matrix;
}

// Swap first_row with the second_row in provided matrix and return it
void SwapRows(Matrix2D<double> &matrix, int first_row, int second_row)
{
	for (int i = 0; i <= matrix.cols(); i++)
	{
		double temp = matrix(first_row, i);
		matrix(first_row, i) = matrix(second_row, i);
		matrix(second_row, i) = temp;
	}
}
