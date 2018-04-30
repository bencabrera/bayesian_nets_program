#include "check.h"

namespace {
	void check_matrix_dimension_matches_degree_for_node(const GBN& gbn, Vertex v)
	{
		auto& g = gbn.graph;
		if(type(v,g) == NODE)
		{
			std::set<int> used_output_ports;
			for(auto e : boost::make_iterator_range(boost::out_edges(v, g)))
				used_output_ports.insert(port_from(e,g));

			if(boost::in_degree(v,g) != matrix(v,g)->n)
				throw std::logic_error(std::string("Input dimension of node '") + name(v,g) + "' (" + std::to_string(boost::in_degree(v,g)) + ") does not fit matrix input dimension (" + std::to_string(matrix(v,g)->n) + ").");
			if(used_output_ports.size() != matrix(v,g)->m)
				throw std::logic_error(std::string("Output dimension of node '") + name(v,g) + "' (" + std::to_string(used_output_ports.size()) + ") does not fit matrix output dimension (" + std::to_string(matrix(v,g)->m) + ").");
		}
	}

	void check_each_network_output_has_precessor(const GBN& gbn)
	{
		auto& g = gbn.graph;
		for(auto v : output_vertices(gbn))
		{
			auto tmp = boost::in_edges(v, g);
			if(std::distance(tmp.first, tmp.second) != 1)
				throw std::logic_error(name(v,g) + ": Output port has no or more than one precessor.");
		}
	}

	void check_input_outputs_of_inside_vertex(const GBN& gbn, Vertex v)
	{
		auto& g = gbn.graph;

		// -- all for input ports --
		std::set<int> used_input_ports;
		for(auto e : boost::make_iterator_range(boost::in_edges(v, g)))
		{
			auto res = used_input_ports.insert(port_to(e, g));	

			// same port was already used -> one input has multiple sources -> failure
			if(!res.second)
				throw std::logic_error(name(v,g) + ": Same input port of a vertex was connected to multiple sources.");
		}

		// check that input ports start at 0 and have no spaces in between
		for(std::size_t i = 0; i < used_input_ports.size(); i++)
		{
			if(used_input_ports.find(i) == used_input_ports.end())
				throw std::logic_error(name(v,g) + ": Input port " + std::to_string(i) + " was not used but later port was used.");
		}

		// -- all for output ports --
		std::set<int> used_output_ports;
		for(auto e : boost::make_iterator_range(boost::out_edges(v, g)))
			used_output_ports.insert(port_from(e, g));	

		// check that output ports start at 0 and have no spaces in between
		for(std::size_t i = 0; i < used_output_ports.size(); i++)
		{
			if(used_output_ports.find(i) == used_output_ports.end())
				throw std::logic_error(name(v,g) + ": Output port " + std::to_string(i) + " was not used but later port was used.");
		}
	}
}

void check_gbn_integrity(const GBN& gbn)
{
	// auto g = graph(gbn);
	const auto& g = gbn.graph;
	const auto gbn_vertices = all_vertices(gbn);

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
		check_input_outputs_of_inside_vertex(gbn, v);
		check_matrix_dimension_matches_degree_for_node(gbn, v);
	}

	check_each_network_output_has_precessor(gbn);
}
