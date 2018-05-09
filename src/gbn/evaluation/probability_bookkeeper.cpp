#include "probability_bookkeeper.h"
#include "../../helpers.hpp"

ProbabilityBookkeeper::ProbabilityBookkeeper(std::size_t n_vertices, std::vector<Vertex> relevant_vertices)
	:lookup(n_vertices), p_vec(relevant_vertices.size(), 0.0), n_zeros(0)
{
	for(std::size_t i = 0; i < relevant_vertices.size(); i++)
		lookup[relevant_vertices[i]] = i;	
}

// can be optimized by dividing by previous etc.
double ProbabilityBookkeeper::get_product()
{
	if(n_zeros == 0)
	{
		double val = 1;
		for(const auto& p : p_vec)
			val *= p;

		return val;
	}
	else
		return 0.0;
}

void ProbabilityBookkeeper::update_one_node(Vertex v, double p)
{
	auto& p_v = p_vec.at(lookup[v]);
	bool is_p_zero = prob_is_zero(p);
	if(n_zeros > 0)
	{
		bool is_p_v_zero = prob_is_zero(p_v);
		if(is_p_zero && !is_p_v_zero)
		{
			p_v = 0.0;
			n_zeros++;
		}
		if(!is_p_zero && is_p_v_zero)
		{
			p_v = p;
			n_zeros--;
		}
		if(!is_p_zero && !is_p_v_zero)
		{
			p_v = p;
		}
	}
	else
	{
		if(is_p_zero) {
			p_v = 0.0;
			n_zeros++;
		}
		else
		{
			p_v = p;
		}
	}
}
