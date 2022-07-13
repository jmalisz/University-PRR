#pragma once
#include <string>
#include <vector>

// Matrix class to simplify operations
template <class T>
class Matrix2D
{
public:
	Matrix2D(size_t rows, size_t cols) : matrix_(std::vector<std::vector<T>>(rows, std::vector<T>(cols))) {}
	Matrix2D(size_t size) : matrix_(std::vector<std::vector<T>>(size, std::vector<T>(size))) {}
	Matrix2D() {}
	std::vector<T> &operator[](size_t index) const { return matrix_[index]; }
	// Augment matrix_1 with matrix_2 values
	void augment(Matrix2D<T> &matrix_1, Matrix2D<T> matrix_2)
	{
		for (int i = 0; i < matrix_2.rows(); i++)
		{
			for (int j = 0; j < matrix_2.cols(); j++)
			{
				matrix_1(i).push_back(matrix_2(i, j));
			}
		}
	}
	// Return number of cols in first row
	int cols() { return matrix_[0].size(); }

	// Return number of rows
	int rows() { return matrix_.size(); }

	// Return rows beggining iterator
	std::vector<std::vector<double>>::iterator begin() { return matrix_.begin(); }

	// Return number of elements
	int size()
	{
		int elements_number = 0;

		for (auto &row : matrix_)
		{
			for (auto &element : row)
			{
				elements_number++;
			}
		}

		return elements_number;
	}
	T &operator()(unsigned int x, unsigned int y)
	{
		return matrix_[x][y];
	}
	std::vector<T> &operator()(unsigned int x)
	{
		return matrix_[x];
	}
	void operator=(std::vector<std::vector<T>> new_matrix)
	{
		matrix_ = new_matrix;
	}

private:
	std::vector<std::vector<T>> matrix_;
};

// Read matrix from file in current directory. Expects matrix size as first number.
Matrix2D<double> ReadMatrixFromFile(std::string file_name);

// Write matrix solution to provided stream
void WriteMatrixToStream(Matrix2D<double> &matrix, std::ostream &stream);

enum MatrixType
{
	kRandom,
	kIdentity
};

// Function that generates a matrix - random or identity
Matrix2D<double> GenerateMatrix(MatrixType matrix_type, int matrix_size);

// Swap first_row with the second_row in provided matrix and return it
void SwapRows(Matrix2D<double> &matrix, int first_row, int second_row);
