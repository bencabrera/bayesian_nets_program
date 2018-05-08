#pragma once

#include "../general/gbn.h"

struct ProbabilityBookkeeper
{
	public:
		ProbabilityBookkeeper(std::size_t n_vertices, std::vector<Vertex> relevant_vertices);

		// can be optimized by dividing by previous etc.
		double get_product();

		void update_one_node(Vertex v, double p);

	private:
		std::vector<std::size_t> lookup;
		std::vector<double> p_vec;

		int n_zeros;
};

