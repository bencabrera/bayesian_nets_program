#include "vertex_add_remove.h"
#include "../matrix/matrix_decomposition.h"
#include "../matrix/matrix_io.h"
#include <iostream>

std::pair<Vertex, Vertex> split_vertex(GBN& gbn, Vertex v)
{
	// get matrix of node
	auto p_m = matrix(v,gbn.graph);
	auto& m = *p_m;

	// get name of node and store it for when its deleted later
	std::string old_name = name(v, gbn.graph);

	// if matrix just has one output wire -> abort
	if(m.m == 1)
	return std::pair<Vertex,Vertex>();

	// decompose matrix
	auto [p_m_front, p_m_back] = matrix_decomposition(p_m);

	// store input and output ports of vertex and sort them
	std::vector<std::tuple<Vertex, std::size_t, std::size_t>> precessors;
	for(auto e : boost::make_iterator_range(boost::in_edges(v,gbn.graph)))
		precessors.push_back({ boost::source(e,gbn.graph), port_from(e, gbn.graph), port_to(e,gbn.graph) });
	std::sort(precessors.begin(), precessors.end());

	std::vector<std::tuple<Vertex, std::size_t, std::size_t>> sucessors;
	for(auto e : boost::make_iterator_range(boost::out_edges(v,gbn.graph)))
		sucessors.push_back({ boost::target(e,gbn.graph), port_from(e, gbn.graph), port_to(e,gbn.graph) });
	std::sort(sucessors.begin(), sucessors.end());

	// remove vertex
	remove_vertex(v,gbn);

	// add matrices to the the new node and give them names etc.
	auto v_front = add_vertex(gbn, p_m_front, old_name + " |-");
	auto v_back = add_vertex(gbn, p_m_back, old_name + " -|");

	// connect new nodes with the old inputs
	Index i_front_port = 0;
	Index i_back_port = 0;
	// Index i_back_port = m.m-1;
	for(auto p : precessors)
	{
		// precessors of front
		{
			auto tmp = boost::add_edge(std::get<0>(p), v_front, gbn.graph);
			auto e = tmp.first;
			put(edge_position, gbn.graph, e, std::pair<std::size_t, std::size_t>{ std::get<1>(p), i_front_port++ });
		}

		// precessors of back
		{
			auto tmp = boost::add_edge(std::get<0>(p), v_back, gbn.graph);
			auto e = tmp.first;
			put(edge_position, gbn.graph, e, std::pair<std::size_t, std::size_t>{ std::get<1>(p), i_back_port++ });
		}
	}

	// add inside edges
	for(Index i = 0; i < m.m-1; i++)
	{
		auto tmp = boost::add_edge(v_front, v_back, gbn.graph);
		auto e = tmp.first;
		put(edge_position, gbn.graph, e, std::pair<std::size_t, std::size_t>{ i, i+m.n });
		// put(edge_position, gbn.graph, e, std::pair<std::size_t, std::size_t>{ i, i });
	}

	// connect new nodes with the old outputs
	for(auto& suc : sucessors)
	{
		auto port_from_original = std::get<1>(suc);

		if(port_from_original == 0)
		{
			auto tmp = boost::add_edge(v_back, std::get<0>(suc), gbn.graph);
			auto e = tmp.first;
			put(edge_position, gbn.graph, e, std::pair<std::size_t, std::size_t>{ 0, std::get<2>(suc) });
		}
		else
		{
			auto tmp = boost::add_edge(v_front, std::get<0>(suc), gbn.graph);
			auto e = tmp.first;
			put(edge_position, gbn.graph, e, std::pair<std::size_t, std::size_t>{ port_from_original-1, std::get<2>(suc) });
		}
	}

	return {v_front, v_back};
} 	

void recursively_split_vertex(GBN& gbn, Vertex v)
{
	auto& g = gbn.graph;

	if(matrix(v,g)->m == 1)
		return;

	auto [v_front, v_back] = split_vertex(gbn, v);
	recursively_split_vertex(gbn, v_front);
}
