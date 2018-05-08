#include "../evaluation/evaluation.h"
#include "vertex_add_remove.h"
#include <iostream>
#include "../matrix/matrix_io.h"

Vertex merge_vertices(GBN& gbn, std::vector<Vertex> vertices, std::string new_node_label)
{
	auto& g = gbn.graph;
	auto [p_m, vertex_set_input_outputs] = evaluate_vertices(gbn, vertices);

	for(auto v : vertices)
		remove_vertex(v, gbn);

	auto v_new = add_vertex(gbn,p_m,new_node_label);

	for(std::size_t i_port = 0; i_port < vertex_set_input_outputs.input_ports.size(); i_port++)
	{
		const auto& [v_ext, pos_ext] = vertex_set_input_outputs.input_external_map[i_port];
		auto e = boost::add_edge(v_ext, v_new, g).first;
		put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ pos_ext, i_port });
	}

	for(std::size_t i_port = 0; i_port < vertex_set_input_outputs.output_ports.size(); i_port++)
	{
		const auto& external_output_ports = vertex_set_input_outputs.output_external_map[i_port];
		for(auto [v_ext, pos_ext] : external_output_ports)
		{
			auto e = boost::add_edge(v_new, v_ext, g).first;
			put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ i_port, pos_ext });
		}
	}

	return v_new;
} 	
