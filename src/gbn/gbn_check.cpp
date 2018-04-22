#include "gbn_check.h"

// point to check:
// - does every node have a matrix?
// - are the dimensions of the matrix correct
// - number of output / input nodes correct?
// - 
void check_gbn_integrity(const GBN& gbn)
{
	// auto g = graph(gbn);
	const auto& g = gbn.graph;
	const auto gbn_vertices = vertices(gbn);

	// check number of nodes etc
	std::size_t n_nodes = 0, n_inputs = 0, n_outputs = 0;
	for(auto v : gbn_vertices)
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
	if(n_inputs != gbn.n)
		throw std::logic_error(std::string("Number of input nodes(") + std::to_string(n_inputs) + ") is not consistent with n" + std::to_string(gbn.n) + ").");
	if(n_outputs != gbn.m)
		throw std::logic_error(std::string("Number of output nodes(") + std::to_string(n_outputs) + ") is not consistent with m" + std::to_string(gbn.m) + ").");

	// check if matrix dimensions fit with in-coming / out-going edges
	for(auto v : gbn_vertices)
	{
		if(type(v,g) == NODE)
		{
			std::set<int> used_output_ports;
			for(auto e : boost::make_iterator_range(boost::out_edges(v, g)))
			{
				used_output_ports.insert(port_from(e,g));
			}

			if(boost::in_degree(v,g) != matrix(v,g)->n)
				throw std::logic_error(std::string("Input dimension of node '") + name(v,g) + "' (" + std::to_string(gbn.n) + ") does not fit matrix input dimension (" + std::to_string(matrix(v,g)->n));
			if(used_output_ports.size() != matrix(v,g)->m)
				throw std::logic_error(std::string("Output dimension of node '") + name(v,g) + "' (" + std::to_string(gbn.m) + ") does not fit matrix output dimension (" + std::to_string(matrix(v,g)->m));
		}
	}

	// check structural integrity of network
	for(auto v : gbn_vertices)
	{
		const auto v_name = name(v,g);
		if(type(v,g) == NODE)
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
			for(Index i = 0; i < n(v,g); i++)
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
