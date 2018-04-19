#include "gbn.h"

GBNGraph& graph(GBN& gbn)
{
	return std::get<1>(gbn);
}

const GBNGraph& graph(const GBN& gbn)
{
	return std::get<1>(gbn);
}

std::pair<int,int>& type(GBN& gbn)
{
	return std::get<0>(gbn);
}
const std::pair<int,int>& type(const GBN& gbn)
{
	return std::get<0>(gbn);
}

// point to check:
// - does every node have a matrix?
// - are the dimensions of the matrix correct
// - number of output / input nodes correct?
// - 
void check_gbn_integrity(const GBN& gbn)
{
	std::size_t n = type(gbn).first;
	std::size_t m = type(gbn).second;

	std::size_t n_nodes = 0, n_inputs = 0, n_outputs = 0;
	for(auto v : boost::make_iterator_range(boost::vertices(graph(gbn))))
	{
		switch(get(vertex_type, graph(gbn), v))
		{
			case NODE:
				n_nodes++;
				break;
			case INPUT:
				n_inputs++;
				break;
			case OUTPUT:
				n_outputs++;
				break;
		}
	}
	if(n_inputs != n)
		throw std::logic_error("Number of input nodes is not consistent with n");
	if(n_inputs != m)
		throw std::logic_error("Number of output nodes is not consistent with m");
}
