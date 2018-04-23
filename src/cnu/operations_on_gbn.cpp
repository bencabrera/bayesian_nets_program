#include "operations_on_gbn.h"
#include "../gbn/gbn_modification.h"
#include <iostream>

void set_op(const std::vector<int> places, bool b, GBN& gbn)
{
	auto& g = gbn.graph;
	for(auto p : places)
	{
		// get precessor of output port
		auto e_pre = *boost::in_edges(gbn.output_vertices[p], g).first;
		auto e_pos = get(edge_position, g, e_pre);
		auto v_pre = boost::source(e_pre, g);
		boost::remove_edge(v_pre, p, g);

		auto v_term = add_vertex(gbn);
		put(vertex_matrix, g, v_term, MatrixPtr(new TerminatorMatrix()));
		put(vertex_type, g, v_term, NODE);
		put(vertex_name, g, v_term, "T");
		auto e_term = boost::add_edge(v_pre, v_term, g).first;
		put(edge_position, g, e_term, std::pair<std::size_t, std::size_t>{ e_pos.first, 0 });

		auto v_one = add_vertex(gbn);
		put(vertex_matrix, g, v_one, MatrixPtr(new OneBMatrix(b)));
		put(vertex_type, g, v_one, NODE);
		put(vertex_name, g, v_one, std::string("1_")+std::to_string(b));
		auto e_one = boost::add_edge(v_one, p, g).first;
		put(edge_position, g, e_one, std::pair<std::size_t, std::size_t>{ 0, 0 });
	}
}

void assert_op(const std::vector<int> /*places*/, bool /*b*/, GBN& /*gbn*/)
{}

void nassert_op(const std::vector<int> /*places*/, bool /*b*/, GBN& /*gbn*/)
{}

