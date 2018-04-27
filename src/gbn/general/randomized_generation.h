#pragma once

#include "../matrix/randomized_generation.h"
#include "gbn.h"

#include <random>

struct RandomGBNParams {
	std::size_t vertex_window_size = 3;
	std::size_t n_min_precessors = 0;
	std::size_t n_max_precessors = 3;
	std::size_t n_min_successors = 0;
	std::size_t n_max_successors = 3;
	RandomMatrixParams matrix_params;
};

GBN generate_random_gbn(std::size_t n, std::size_t m, std::size_t n_inside_vertices, std::mt19937& mt, RandomGBNParams params);
