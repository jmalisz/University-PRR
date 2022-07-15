#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>

#include <openmp.h>
#include <openmpi.h>
#include <sequential.h>
#include <threaded.h>
#include <utils.h>

int main()
{
	using std::cin;
	using std::cout;
	using std::endl;
	using std::numeric_limits;
	using std::streamsize;

	Matrix2D<double> matrix;
	int user_input = 0;

	try
	{
		// Choose input type
		cout << "Matrix inversion program. Choose input type: 1 - file (Input.txt), 2 - random: ";

		while (!(cin >> user_input) || user_input < 1 || user_input > 2)
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

			while (!(cin >> user_input) || user_input < 1)
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
		int output_type = 0;
		std::ofstream output_file;
		cout << "Choose output type: 1 - file (Output.txt), 2 - console: ";

		while (!(cin >> output_type) || output_type < 1 || output_type > 2)
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Wrong output type, choose output type: 1 - file (Output.txt), 2 - console: ";
		}

		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		matrix.augment(matrix, GenerateMatrix(kIdentity, matrix.rows()));

		if (output_type == 1)
		{
			output_file.open("Output.txt", std::ios::trunc);
		}
		else
		{
			cout << "Matrix before inversion:" << endl;
		}

		WriteMatrixToStream(matrix, output_type == 1 ? output_file : cout);

		// Choose algorithm
		cout << "Choose algorithm type (its execution will be timed):" << endl;

		auto print_algorithms = []()
		{
			cout << "1 - Sequential" << endl;
			cout << "2 - OpenMP" << endl;
			cout << "3 - OpenMPI" << endl;
			cout << "4 - Threads" << endl;
		};

		print_algorithms();

		while (!(cin >> user_input) || user_input < 1 || user_input > 4)
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Wrong algorithm type, chose algorithm type:" << endl;
			print_algorithms();
		}

		auto start = std::chrono::system_clock::now();

		switch (user_input)
		{
		case 1:
			SequentialGaussianElimination(matrix);
			break;
		case 2:
			OpenMPGaussianElimination(matrix);
			break;
		case 3:
			// Interfacing this handler program with OpenMPI isn't trivial
			// It's easier to just call it as another program here
			// TODO: Add external program execution there
			throw std::runtime_error("Error: Not implemented yet, see README.md for more info");
		case 4:
			ThreadedGaussianElimination(matrix, 4);
			break;
		default:
			break;
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double, std::milli> time_ms = end - start;

		if (output_type == 2)
		{
			cout << "Matrix after inversion:" << endl;
		}
		WriteMatrixToStream(matrix, output_type == 1 ? output_file : cout);

		cout << std::setprecision(5);
		cout << "Time taken by algorithm: " << time_ms.count() << "ms" << endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}
