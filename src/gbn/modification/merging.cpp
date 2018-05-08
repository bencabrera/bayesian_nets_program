#include "../evaluation/evaluation.h"
#include "vertex_add_remove.h"
#include <iostream>
#include "../matrix/matrix_io.h"
#include "../../helpers.hpp"

Vertex merge_vertices(GBN& gbn, std::vector<Vertex> vertices, std::string new_node_label)
{
	auto& g = gbn.graph;
	auto [input_ports, output_ports] = build_inputs_outputs_for_vertices(gbn, vertices);
	auto p_m = evaluate(gbn, vertices);

	std::vector<std::set<Port>> output_ports_to_external_vertex_inputs_map;
	for(auto p : output_ports)
	{
		output_ports_to_external_vertex_inputs_map.push_back({});
		auto& set = output_ports_to_external_vertex_inputs_map.back();

		auto v = p.first;
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
			if(port_from(e,g) == p.second && !is_in(boost::target(e,g), vertices))
				set.insert({ boost::target(e,g), port_to(e,g) });		
	}

	for(auto v : vertices)
		remove_vertex(v, gbn);

	auto v_new = add_vertex(gbn,p_m,new_node_label);

	for(std::size_t i_port = 0; i_port < input_ports.size(); i_port++)
	{
		const auto& [v_ext, pos_ext] = input_ports[i_port];
		auto e = boost::add_edge(v_ext, v_new, g).first;
		put(edge_position, g, e, std::make_pair(pos_ext, i_port));
	}

	for(std::size_t i_port = 0; i_port < output_ports.size(); i_port++)
	{
		const auto& external_output_ports = output_ports_to_external_vertex_inputs_map[i_port];
		for(auto [v_ext, pos_ext] : external_output_ports)
		{
			auto e = boost::add_edge(v_new, v_ext, g).first;
			put(edge_position, g, e, std::make_pair(i_port, pos_ext));
		}
	}

	return v_new;
} 	
