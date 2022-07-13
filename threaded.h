#pragma once
#include <utils.h>

// Perform Gaussian elimination to find matrix solution with threads
void ThreadedGaussianElimination(Matrix2D<double> &matrix, int thread_number);
