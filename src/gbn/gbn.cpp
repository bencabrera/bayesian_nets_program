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

int port_from(const GBNGraph::edge_descriptor& e, const GBNGraph& g)
{
	return get(edge_position, g, e).first;
}

int port_to(const GBNGraph::edge_descriptor& e, const GBNGraph& g)
{
	return get(edge_position, g, e).second;
}

std::pair<int,int> type(const GBNGraph::vertex_descriptor& v, const GBNGraph& g)
{
	return std::pair<int, int>(get(vertex_matrix, g, v)->n, get(vertex_matrix, g, v)->m);
}

std::string& name(const GBNGraph::vertex_descriptor& v, GBNGraph& g)
{
	return get(vertex_name, g, v);
}

const std::string& name(const GBNGraph::vertex_descriptor& v, const GBNGraph& g)
{
	return get(vertex_name, g, v);
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

	const auto& g = graph(gbn);

	// check number of nodes etc
	std::size_t n_nodes = 0, n_inputs = 0, n_outputs = 0;
	for(auto v : boost::make_iterator_range(boost::vertices(g)))
	{
		switch(get(vertex_type, g, v))
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
		throw std::logic_error(std::string("Number of input nodes(") + std::to_string(n_inputs) + ") is not consistent with n" + std::to_string(n) + ").");
	if(n_outputs != m)
		throw std::logic_error(std::string("Number of output nodes(") + std::to_string(n_outputs) + ") is not consistent with m" + std::to_string(m) + ").");

	// check if matrix dimensions fit with in-coming / out-going edges
	for(auto v : boost::make_iterator_range(boost::vertices(g)))
	{
		const auto v_name = name(v,g);
		if(get(vertex_type, g, v) == NODE)
		{
			std::set<int> used_input_ports;
			for(auto e : boost::make_iterator_range(boost::in_edges(v, g)))
			{
				auto res = used_input_ports.insert(port_to(e, g));	

				// same port was already used -> one input has multiple sources -> failure
				if(!res.second)
					throw std::logic_error(v_name + ": Same input port of a vertex was connected to multiple sources.");
			}

			// check if all input ports have some precessor
			for(int i = 0; i < type(v,g).first; i++)
				if(used_input_ports.find(i) == used_input_ports.end())
					throw std::logic_error(v_name + ": Input port " + std::to_string(i) + " of vertex was not connected to a source.");
		}


		// check that all output ports have exactly one precessor
		if(get(vertex_type, g, v) == OUTPUT)
		{
			auto tmp = boost::in_edges(v, g);
			if(std::distance(tmp.first, tmp.second) != 1)
				throw std::logic_error(v_name + ": Output port has no or more than one precessor.");
		}
	}
}

