#include "../evaluation/evaluation.h"
#include "vertex_add_remove.h"
#include <iostream>
#include "../matrix/matrix_io.h"
#include "../../helpers.hpp"
#include "../general/subgbn.h"

Vertex merge_vertices(GBN& gbn, std::vector<Vertex> vertices, std::string new_node_label)
{
	auto& g = gbn.graph;

	auto sub_gbn = SubGBN::make_from_vertices(gbn, vertices);
	auto p_m = evaluate(sub_gbn.gbn);

	for(auto v : vertices)
		remove_vertex(v, gbn);

	auto v_new = add_vertex(gbn,p_m,new_node_label);

	auto& external_input_connectors = sub_gbn.input_ports;
	auto& external_output_connectors = sub_gbn.output_ports;

	for(std::size_t i_port = 0; i_port < external_input_connectors.size(); i_port++)
	{
		const auto& [v_ext, pos_ext] = external_input_connectors[i_port];
		auto e = boost::add_edge(v_ext, v_new, g).first;
		put(edge_position, g, e, std::make_pair(pos_ext, i_port));
	}

	for(std::size_t i_port = 0; i_port < external_output_connectors.size(); i_port++)
		for(auto [v_ext, pos_ext] : external_output_connectors[i_port])
		{
			auto e = boost::add_edge(v_new, v_ext, g).first;
			put(edge_position, g, e, std::make_pair(i_port, pos_ext));
		}

	return v_new;
} 	
