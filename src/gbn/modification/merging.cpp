#include "../general/evaluation.h"
#include "vertex_add_remove.h"
#include <iostream>

Vertex merge_vertices(GBN& gbn, std::vector<Vertex> vertices, std::string new_node_label)
{
	auto& g = gbn.graph;
	auto m = evaluate_gbn(gbn, vertices);
	// TODO: this should of course be returned from previous function as well and not computed again
	auto wire_structure = build_wire_structure(gbn, vertices);

	const auto& input_ports = wire_structure.input_ports;
	const auto& output_ports = wire_structure.output_ports;

	for(auto v : vertices)
		remove_vertex(v, gbn);

	auto v_new = add_vertex(gbn);
	put(vertex_matrix, g, v_new, m);
	put(vertex_name, g, v_new, new_node_label);
	put(vertex_type, g, v_new, NODE);

	for(std::size_t i_port = 0; i_port < input_ports.size(); i_port++)
	{
		const auto& p = input_ports[i_port];
		auto tmp = boost::add_edge(p.first, v_new, g);
		put(edge_position, g, tmp.first, std::pair<std::size_t, std::size_t>{ p.second, i_port });
	}

	for(std::size_t i_port = 0; i_port < output_ports.size(); i_port++)
	{
		const auto& p = output_ports[i_port];
		auto tmp = boost::add_edge(v_new, p.first, g);
		put(edge_position, g, tmp.first, std::pair<std::size_t, std::size_t>{ i_port, p.second });
	}

	return v_new;
} 	
