#include "subgbn.h"

#include "../../helpers.hpp"
#include <iostream>

namespace {
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
}


SubGBN SubGBN::make_from_vertices(const GBN& gbn, std::vector<Vertex> inside_vertices)
{
	auto& g_global = gbn.graph;

	auto [input_ports, output_ports] = build_inputs_outputs_for_vertices(gbn, inside_vertices);
	std::map<Port, std::size_t> input_port_map, output_port_map;
	{
		Index i_port = 0;
		for(auto p : input_ports)
			input_port_map[p] = i_port++;
	}
	{
		Index i_port = 0;
		for(auto p : output_ports)
			output_port_map[p] = i_port++;
	}

	GBN gbn_local(input_ports.size(), output_ports.size(), inside_vertices.size());
	auto& g_local = gbn_local.graph;

	auto input_vertices = ::input_vertices(gbn_local);
	auto output_vertices = ::output_vertices(gbn_local);

	std::map<Vertex,Vertex> global_to_local_vertex_map;
	std::size_t i_local = 0;
	for(auto v : inside_vertices)
		global_to_local_vertex_map.insert({ v, i_local++ });

	std::vector<Port> subgbn_input_ports(input_ports.size());
	std::vector<std::vector<Port>> subgbn_output_ports(output_ports.size());

	for(auto v : inside_vertices)
	{
		put(vertex_matrix, g_local, global_to_local_vertex_map.at(v), matrix(v,g_global));

		for(auto e : boost::make_iterator_range(boost::out_edges(v,g_global)))
		{
			auto u_global = boost::source(e,g_global);
			auto v_global = boost::target(e,g_global);
			auto pos_global = get(edge_position, g_global, e);
			auto u_local = global_to_local_vertex_map.at(u_global);

			if(is_in(v_global, inside_vertices))
			{
				auto v_local = global_to_local_vertex_map.at(v_global);
				auto e_local = boost::add_edge(u_local, v_local, g_local).first;
				put(edge_position, g_local, e_local, pos_global);
			}
			else
			{
				Port p_output{ u_global, port_from(e,g_global) };
				auto i_output = output_port_map.at(p_output);
				subgbn_output_ports.at(i_output).push_back({ v_global, port_to(e,g_global) });

				if(!boost::edge(u_local, output_vertices[i_output], g_local).second)
				{
					auto e_local = boost::add_edge(u_local, output_vertices[i_output], g_local).first;
					put(edge_position, g_local, e_local, std::make_pair(pos_global.first, 0));
				}
			}
		}

		for(auto e : boost::make_iterator_range(boost::in_edges(v,g_global)))
		{
			auto u_global = boost::source(e,g_global);
			auto v_global = boost::target(e,g_global);
			auto pos_global = get(edge_position, g_global, e);
			auto v_local = global_to_local_vertex_map.at(v_global);

			if(!is_in(u_global, inside_vertices))
			{
				Port p_input{ u_global, port_from(e,g_global) };
				auto i_input = input_port_map.at(p_input);
				auto e_local = boost::add_edge(input_vertices[i_input], v_local, g_local).first;
				put(edge_position, g_local, e_local, std::make_pair(0, pos_global.second));
			
				subgbn_input_ports.at(i_input) = { u_global, port_from(e,g_global) };
			}
		}
	}

	SubGBN sub_gbn(gbn_local, subgbn_input_ports, subgbn_output_ports);

	return sub_gbn;
}

SubGBN::SubGBN(GBN gbn, std::vector<Port> input_ports, std::vector<std::vector<Port>> output_ports)
	:gbn(gbn), input_ports(input_ports), output_ports(output_ports)
{}
