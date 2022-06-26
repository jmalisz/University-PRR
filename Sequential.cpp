#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

using namespace std;

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
Matrix2D<double> ReadMatrixFromFile(string file_name)
{
	ifstream input(file_name);

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
void WriteMatrixToStream(Matrix2D<double> &matrix, ostream &stream)
{
	stream << setprecision(2);

	for (int i = 0; i < matrix.rows(); ++i)
	{
		for (int j = 0; j < matrix.cols(); ++j)
		{
			stream << setw(10) << matrix(i, j);
		}

		stream << endl;
	}

	stream << endl;
}

enum MatrixType
{
	kRandom,
	kIdentity
};

// Function that generates a matrix - random or identity
Matrix2D<double> GenerateMatrix(MatrixType matrix_type, int matrix_size)
{
	Matrix2D<double> new_matrix(matrix_size);

	// Obtain a random number from hardware
	random_device rd;
	// Seed the generator
	mt19937 gen(rd());
	// Define the range
	uniform_int_distribution<> dis(0, 100);

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
	for (int i = 0; i <= matrix.rows(); i++)
	{
		double temp = matrix(first_row, i);
		matrix(first_row, i) = matrix(second_row, i);
		matrix(second_row, i) = temp;
	}
}

// Perform Gaussian elimination to find matrix solution
void GaussianElimination(Matrix2D<double> &matrix)
{
	for (int diagonal = 0; diagonal < matrix.rows(); diagonal++)
	{
		int pivot_row = diagonal;
		double pivot_value = matrix(pivot_row, diagonal);

		// Find better pivot if it exists
		for (int row = diagonal + 1; row < matrix.rows(); row++)
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
		for (int col = diagonal; col < matrix.cols(); col++)
		{
			matrix(diagonal, col) /= pivot_value;
		}

		// Reduce elements other than diagonal to 0
		for (int row = 0; row < matrix.rows(); row++)
		{
			if (row == diagonal)
			{
				continue;
			}

			double ratio = matrix(row, diagonal);

			// Reduce row elements by pivot * factor
			for (int col = 0; col < matrix.cols(); col++)
			{
				matrix(row, col) -= matrix(diagonal, col) * ratio;
			}
		}
	}
}

int main()
{
	Matrix2D<double> matrix;
	int user_input = 0;

	try
	{
		// Choose input type
		cout << "Matrix inversion program. Choose input type: 1 - file (Input.txt), 2 - random: ";

		while (!(cin >> user_input) || (user_input != 1 && user_input != 2))
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Wrong input type, choose input type: 1 - file (Input.txt), 2 - random: ";
		}

		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (user_input == 1)
		{
			matrix = ReadMatrixFromFile("Input.txt");
		}
		else
		{
			// Choose random matrix size
			cout << "Choose matrix size: ";

			while (!(cin >> user_input) || user_input <= 0)
			{
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Wrong matrix size, choose a number bigger than 0: ";
			}

			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			matrix = GenerateMatrix(kRandom, user_input);
		}

		// Choose output type, file will be closed in destructor
		ofstream output_file;
		cout << "Choose output type: 1 - file (Output.txt), 2 - console: ";

		while (!(cin >> user_input) || user_input <= 0)
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Wrong output type, choose output type: 1 - file (Output.txt), 2 - console: ";
		}

		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		if (user_input == 1)
		{
			output_file.open("Output.txt", ios::trunc);
		}

		matrix.augment(matrix, GenerateMatrix(kIdentity, matrix.rows()));

		if (user_input == 2)
		{
			cout << "Matrix before inversion:" << endl;
		}

		WriteMatrixToStream(matrix, user_input == 1 ? output_file : cout);

		GaussianElimination(matrix);

		if (user_input == 2)
		{
			cout << "Matrix after inversion:" << endl;
		}

		WriteMatrixToStream(matrix, user_input == 1 ? output_file : cout);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}
