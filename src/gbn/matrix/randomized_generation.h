#pragma once

#include "matrix.h"

#include <random>

struct RandomMatrixParams {
	double F_matrix_prob = 1;
	double OneB_matrix_prob = 1;
	double Terminator_matrix_prob = 1;
};

MatrixPtr generate_random_matrix(std::size_t n, std::size_t m, std::mt19937& mt, RandomMatrixParams params);
