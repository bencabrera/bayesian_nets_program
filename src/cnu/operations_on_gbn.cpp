#include "operations_on_gbn.h"
#include "../gbn/modification/vertex_add_remove.h"
#include <iostream>

void set_op(const std::vector<int> places, bool b, GBN& gbn)
{
	auto& g = gbn.graph;
	auto& output_vertices = ::output_vertices(gbn);

	for(auto p : places)
	{
		// get precessor of output port
		auto tmp_in_edges = boost::in_edges(output_vertices[p], g);
		if(std::distance(tmp_in_edges.first, tmp_in_edges.second) != 1)
			throw std::logic_error(std::string("Place ") + std::to_string(p) + " has none or more than one precessor");
		auto e_pre = *(tmp_in_edges.first);
		auto e_pos = get(edge_position, g, e_pre);
		auto v_pre = boost::source(e_pre, g);

		auto v_term = add_vertex(gbn, MatrixPtr(new TerminatorMatrix()), "T");
		auto e_term = boost::add_edge(v_pre, v_term, g).first;
		put(edge_position, g, e_term, std::pair<std::size_t, std::size_t>{ e_pos.first, 0 });

		auto v_one = add_vertex(gbn, MatrixPtr(new OneBMatrix(b)), std::string("1_")+std::to_string(b));
		auto e_one = boost::add_edge(v_one, output_vertices[p], g).first;
		put(edge_position, g, e_one, std::pair<std::size_t, std::size_t>{ 0, 0 });

		boost::remove_edge(v_pre, output_vertices[p], g);
	}
}

void assert_op(const std::vector<int> places, bool b, GBN& gbn)
{
	auto& g = gbn.graph;
	auto& output_vertices = ::output_vertices(gbn);

	for(auto p : places)
	{
		// get precessor of output port
		auto tmp_in_edges = boost::in_edges(output_vertices[p], g);
		if(std::distance(tmp_in_edges.first, tmp_in_edges.second) != 1)
			throw std::logic_error(std::string("Place ") + std::to_string(p) + " has none or more than one precessor");

		auto e_pre = *(tmp_in_edges.first);
		auto e_pos = get(edge_position, g, e_pre);
		auto v_pre = boost::source(e_pre, g);

		auto v_assert = add_vertex(gbn, MatrixPtr(new FMatrix(1,1-b)), std::string("F_{1,") + std::to_string(1-b) + "}");
		auto e_term = boost::add_edge(v_pre, v_assert, g).first;
		put(edge_position, g, e_term, std::pair<std::size_t, std::size_t>{ e_pos.first, 0 });
		auto e_out = boost::add_edge(v_assert, output_vertices[p], g).first;
		put(edge_position, g, e_out, std::pair<std::size_t, std::size_t>{ 0, 0 });
		boost::remove_edge(v_pre, output_vertices[p], g);
	}
}

void nassert_op(const std::vector<int> places, bool b, GBN& gbn)
{
	auto& g = gbn.graph;
	auto& output_vertices = ::output_vertices(gbn);

	std::size_t k = places.size();
	
	auto v_nassert = add_vertex(gbn, MatrixPtr(new FMatrix(k,b)),std::string("F_{") + std::to_string(k) + "," + std::to_string(b) + "}");
	std::size_t i_place = 0;
	for(auto p : places)
	{
		// get precessor of output port
		auto tmp_in_edges = boost::in_edges(output_vertices[p], g);
		if(std::distance(tmp_in_edges.first, tmp_in_edges.second) != 1)
			throw std::logic_error(std::string("Place ") + std::to_string(p) + " has none or more than one precessor");

		auto e_pre = *(tmp_in_edges.first);
		auto e_pos = get(edge_position, g, e_pre);
		auto v_pre = boost::source(e_pre, g);

		auto e_to_nassert = boost::add_edge(v_pre, v_nassert, g).first;
		put(edge_position, g, e_to_nassert, std::pair<std::size_t, std::size_t>{ e_pos.first, i_place });
		auto e_from_nassert = boost::add_edge(v_nassert, output_vertices[p], g).first;
		put(edge_position, g, e_from_nassert, std::pair<std::size_t, std::size_t>{ i_place, 0 });
		boost::remove_edge(v_pre, output_vertices[p], g);
	}
}
