#include "gbn_simplification.h"

#include <iostream>
#include "matrix_io.h"
#include "gbn_modification.h"

namespace {
	using Port = std::pair<Vertex, int>;

	void check_and_apply_F1(GBN& gbn, Vertex v)
	{
		auto& g = gbn.graph;

		if(type(v,g) != NODE)
			return;

		if(matrix(v,g)->type == ONE_B)
		{
			if(boost::out_degree(v,g) > 1)
			{
				auto& m = dynamic_cast<OneBMatrix&>(*matrix(v,g));

				std::vector<Port> sucessor_ports;	
				for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
					sucessor_ports.push_back({ boost::target(e,g), port_to(e,g) });

				remove_vertex(v,gbn);

				for(auto p : sucessor_ports)
				{
					auto v_new = add_vertex(gbn);
					put(vertex_name, g, v_new, std::string("1_") + std::to_string(m.b));
					put(vertex_type, g, v_new, NODE);
					put(vertex_matrix, g, v_new, MatrixPtr(new OneBMatrix(m.b)));
					auto e = boost::add_edge(v_new, p.first, g).first;
					put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ 0, p.second });
				}
			}
		}
	}

	// TODO: implement the more general version for n -> m
	void check_and_apply_F2(GBN& gbn, Vertex v)
	{
		auto& g = gbn.graph;

		if(type(v,g) != NODE || matrix(v,g)->type != TERMINATOR)
			return;

		auto e_pre = *(boost::in_edges(v,g).first);
		auto v_pre = boost::source(e_pre, g);

		if(boost::out_degree(v_pre,g) != 1)
			return;

		// if we reached here, then v_pre is a stochastic vertex whose only successor is a terminator
		
		std::vector<Port> precessor_ports;	
		for(auto e : boost::make_iterator_range(boost::in_edges(v_pre,g)))
			precessor_ports.push_back({ boost::source(e,g), port_from(e,g) });

		remove_vertex(v,gbn);
		remove_vertex(v_pre,gbn);

		for(auto& p : precessor_ports)
		{
			auto v_term = add_vertex(gbn);
			put(vertex_name, g, v_term, std::string("T"));
			put(vertex_type, g, v_term, NODE);
			put(vertex_matrix, g, v_term, MatrixPtr(new TerminatorMatrix()));
			auto e = boost::add_edge(p.first, v_term, g).first;
			put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ p.second, 0 });
		}
	}

	// TODO: implement the more general version for n -> m
	void check_and_apply_CoUnit(GBN& gbn, Vertex v)
	{
		auto& g = gbn.graph;

		if(type(v,g) != NODE || matrix(v,g)->type != TERMINATOR)
			return;

		auto e_pre = *(boost::in_edges(v,g).first);
		auto v_pre = boost::source(e_pre, g);

		auto term_pre_port = port_from(e_pre, g);
		std::size_t n_same_ports = 0;
		for(auto e : boost::make_iterator_range(boost::out_edges(v_pre,g)))
		{
			if(port_from(e,g) == term_pre_port)
				n_same_ports++;
		}

		if(n_same_ports <= 1)
			return;
		else
			remove_vertex(v,gbn);
	}
}


void gbn_simplification(GBN& gbn)
{
	auto& g = gbn.graph;

	// (F1): run over all ONE_B matrices and check their precessors
	for(auto v: vertices(gbn))	
	{
		check_and_apply_F1(gbn, v);
		check_and_apply_F2(gbn, v);
		check_and_apply_CoUnit(gbn, v);
	}
}
