#include "vertex_set_input_output.h"
#include "../../helpers.hpp"
#include <iostream>

std::pair<std::vector<Port>,std::vector<Port>> build_inputs_outputs_for_vertices(const GBN& gbn, std::vector<Vertex> inside_vertices)
{
	auto& g = gbn.graph;

	std::set<Vertex> inside_vertex_set(inside_vertices.begin(), inside_vertices.end());
	std::set<Port> input_port_set, output_port_set;

	for(auto v : inside_vertices)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			auto u = boost::source(e,g);
			if(!is_in(u,inside_vertex_set))
				input_port_set.insert({ u, port_from(e,g) });
		}

		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			auto w = boost::target(e,g);
			if(!is_in(w,inside_vertex_set))
				output_port_set.insert({ v, port_from(e,g) });
		}
	}

	return { 
		std::vector<Port>(input_port_set.begin(), input_port_set.end()), 
		std::vector<Port>(output_port_set.begin(), output_port_set.end())
	};
}

std::pair<std::vector<Port>,std::vector<Port>> build_inputs_outputs_for_gbn(const GBN& gbn)
{
	std::vector<Port> input_ports, output_ports;
	for(auto v : ::input_vertices(gbn))
		input_ports.push_back({ v, 0 });
	for(auto v : ::output_vertices(gbn))
		output_ports.push_back({ v, 0 });

	return { input_ports, output_ports };
}
