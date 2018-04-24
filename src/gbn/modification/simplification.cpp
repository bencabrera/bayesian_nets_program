#include "simplification.h"

#include <iostream>
#include "../matrix/matrix_io.h"
#include "vertex_add_remove.h"
#include "merging.h"
#include "splitting.h"

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

				for(auto p : sucessor_ports)
				{
					auto v_new = add_vertex(gbn, MatrixPtr(new OneBMatrix(m.b)),std::string("1_") + std::to_string(m.b));
					auto e = boost::add_edge(v_new, p.first, g).first;
					put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ 0, p.second });
				}

				remove_vertex(v,gbn);
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
			auto v_term = add_vertex(gbn, MatrixPtr(new TerminatorMatrix()), "T");
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

	void check_and_apply_F3(GBN& gbn, Vertex v)
	{
		auto& g = gbn.graph;

		if(type(v,g) != NODE || matrix(v,g)->type != F)
			return;

		auto& m = dynamic_cast<FMatrix&>(*matrix(v,g));
		if(m.k < 2)
			return;

		// check for all precessors of F if they split their signal to multiple ports
		bool found;
		do {
			found = false;
			Vertex v_pre, v_post;
			Index v_pre_port, v_post_port, F_port;

			for(auto [it, end_it] = boost::in_edges(v,g); it != end_it && !found; it++)
			{
				auto e = *it;
				v_pre = boost::source(e,g);
				v_pre_port = port_from(e,g);
				F_port = port_to(e,g);
				for(auto e_pre_post : boost::make_iterator_range(boost::out_edges(v_pre,g)))
				{
					v_post = boost::target(e_pre_post,g);
					if(port_from(e_pre_post,g) == v_pre_port && v_post != v)
					{
						found = true;
						v_post_port = port_to(e_pre_post,g);
						break;
					}
				}
			}

			if(found) {
				boost::remove_edge_if([&](const Edge& e) {
						return boost::source(e,g) == v_pre && boost::target(e,g) == v_post && port_from(e,g) == v_pre_port && port_to(e,g) == v_post_port;	
						}, g);
				auto e_new = boost::add_edge(v,v_post,g).first;
				put(edge_position, g, e_new, std::pair<std::size_t, std::size_t>{ F_port, v_post_port });
			}
		}
		while(found);
	}

	void check_and_apply_F4(GBN& gbn, Vertex v)
	{
		auto& g = gbn.graph;

		if(type(v,g) != NODE || matrix(v,g)->type != ONE_B)
			return;

		auto& m = dynamic_cast<OneBMatrix&>(*matrix(v,g));
		auto b = m.b;

		bool found;
		do {
			found = false;
			Vertex v_F;
			Index F_port;
			for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
			{
				auto v_to = boost::target(e,g);
				if(type(v_to,g) == NODE && matrix(v_to, g)->type == F)
				{
					auto& m_F = dynamic_cast<FMatrix&>(*matrix(v_to, g));
					if(m_F.b == b)
					{
						v_F = v_to;
						F_port = port_to(e,g);
						found = true;
						break;
					}
				}
			}

			if(found) {
				auto& m_F = dynamic_cast<FMatrix&>(*matrix(v_F, g));

				std::vector<Port> output_ports;
				for(auto e : boost::make_iterator_range(boost::out_edges(v_F,g)))
				{
					if(port_from(e,g) == F_port)
					{
						output_ports.push_back({ boost::target(e,g), port_to(e,g) });
					}	
				}

				boost::remove_edge_if([&](const Edge& e) {
					if(boost::target(e,g) == v_F && port_to(e,g) == F_port)
						return true;
					if(boost::source(e,g) == v_F && port_from(e,g) == F_port)
						return true;

					return false;
				},g);
				
				for(auto e : boost::make_iterator_range(boost::in_edges(v_F,g)))
					if(port_to(e,g) >= F_port)
						put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ port_from(e,g), port_to(e,g)-1 });
				for(auto e : boost::make_iterator_range(boost::out_edges(v_F,g)))
					if(port_from(e,g) >= F_port)
						put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ port_from(e,g)-1, port_to(e,g) });

				m_F.k--;
				m_F.n--;
				m_F.m--;
				m_F.one_mask_n = m_F.init_one_mask(m_F.n);
				m_F.one_mask_m = m_F.init_one_mask(m_F.m);

				for(auto p : output_ports)
				{
					auto e = boost::add_edge(v,p.first,g).first;
					put(edge_position,g,e,std::pair<std::size_t,std::size_t>{ 0, p.second });
				}
			}

		}
		while(found);
	}

	void check_and_apply_F5(GBN& gbn, Vertex v)
	{
		auto& g = gbn.graph;

		if(type(v,g) != NODE || matrix(v,g)->type != ONE_B)
			return;

		auto& m = dynamic_cast<OneBMatrix&>(*matrix(v,g));
		auto b = m.b;

		bool found;
		do {
			found = false;
			Vertex v_F;
			Index F_port;
			for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
			{
				auto v_to = boost::target(e,g);
				if(type(v_to,g) == NODE && matrix(v_to, g)->type == F)
				{
					auto& m_F = dynamic_cast<FMatrix&>(*matrix(v_to, g));
					if(m_F.b == !b)
					{
						v_F = v_to;
						F_port = port_to(e,g);
						found = true;
						break;
					}
				}
			}

			if(found) {
				auto& m_F = dynamic_cast<FMatrix&>(*matrix(v_F, g));
				auto k = m_F.k;

				for(std::size_t i_port = 0; i_port < k; i_port++)
				{
					if(i_port == F_port)
						continue;

					Vertex v_pre;
					Index port_pre;
					for(auto e : boost::make_iterator_range(boost::in_edges(v_F,g)))
					{
						if(port_to(e,g) == i_port)
						{
							v_pre = boost::source(e,g);
							port_pre = port_from(e,g);
							break;
						}
					}

					for(auto e : boost::make_iterator_range(boost::out_edges(v_F,g)))
					{
						if(port_from(e,g) == i_port)
						{
							auto e_new = boost::add_edge(v_pre, boost::target(e,g), g).first;
							put(edge_position, g, e_new, std::pair<std::size_t, std::size_t>{ port_pre, port_to(e_new,g) });
						}
					}
				}
				for(auto e : boost::make_iterator_range(boost::out_edges(v_F,g)))
				{
					if(port_from(e,g) == F_port)
					{
						auto e_new = boost::add_edge(v, boost::target(e,g), g).first;	
						put(edge_position, g, e_new, std::pair<std::size_t, std::size_t>{ 0, port_to(e_new,g) });
					}
				}

				remove_vertex(v_F,gbn);
			}
		}
		while(found);
	}

	void gbn_eliminate_without_outputs(GBN& gbn)
	{
		auto& g = gbn.graph;

		bool found;
		// do {
			Vertex v_without;
			auto vec = inside_vertices(gbn);
			for(auto v : vec)	
			{
				bool connected_to_output = false;
				for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
				{
					if(type(boost::target(e,g),g) == OUTPUT)
					{
						connected_to_output = true;
						break;
					}
				}
				if(!connected_to_output && matrix(v,g)->is_stochastic)
				{
					v_without = v;
					found = true;
					break;
				}
			}

			if(found) {
				std::vector<Vertex> successors;
				successors.push_back(v_without);
				for(auto e : boost::make_iterator_range(boost::out_edges(v_without,g)))
				{
					successors.push_back(boost::target(e,g));
				}
				merge_vertices(gbn, successors);

				// recursively_split_vertex(gbn, v_new);
			}
		// }
		// while(found);
	}
}


void gbn_simplification(GBN& gbn)
{
	for(auto v: inside_vertices(gbn))	
	{
		check_and_apply_F1(gbn, v);
		check_and_apply_F2(gbn, v);
		check_and_apply_CoUnit(gbn, v);
		check_and_apply_F3(gbn, v);
		check_and_apply_F4(gbn, v);
		check_and_apply_F5(gbn, v);
	}
	gbn_eliminate_without_outputs(gbn);
}
