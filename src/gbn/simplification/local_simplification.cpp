#include "simplification.h"

#include <iostream>
#include <map>
#include "../matrix/matrix_io.h"
#include "../modification/vertex_add_remove.h"
#include "../modification/merging.h"
#include "../modification/splitting.h"
#include "../general/path_closing.h"
#include "../general/gbn_io.h"

#include <fstream>
#include <unistd.h>

bool check_and_apply_F1(GBN& gbn, Vertex v)
{
	auto& g = gbn.graph;

	if(type(v,g) != NODE)
		return false;

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
				auto v_new = add_vertex(gbn, std::make_shared<OneBMatrix>(m.b),std::string("1_") + std::to_string(m.b));
				auto e = boost::add_edge(v_new, p.first, g).first;
				put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ 0, p.second });
			}

			remove_vertex(v,gbn);

			return true;
		}
	}

	return false;
}

bool check_and_apply_F2(GBN& gbn, Vertex v)
{
	auto& g = gbn.graph;

	if(type(v,g) != NODE)
		return false;

	if(!matrix(v,g)->is_stochastic)
		return false;

	if(boost::out_degree(v,g) == 0)
		return false;

	for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
	{
		auto v_suc = boost::target(e,g);
		if(type(v_suc,g) != NODE)
			return false;
		if(matrix(v_suc,g)->type != TERMINATOR)
			return false;
	}

	// if we reached here, then v_pre is a stochastic vertex whose only successor is a terminator

	std::vector<Port> precessor_ports;	
	for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		precessor_ports.push_back({ boost::source(e,g), port_from(e,g) });

	for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
	{
		auto v_suc = boost::target(e,g);
		remove_vertex(v_suc,gbn);
	}
	remove_vertex(v,gbn);

	for(auto& p : precessor_ports)
	{
		auto v_term = add_vertex(gbn, std::make_shared<TerminatorMatrix>(), "T");
		auto e = boost::add_edge(p.first, v_term, g).first;
		put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ p.second, 0 });
	}

	return true;
}

// TODO: implement the more general version for n -> m
bool check_and_apply_CoUnit(GBN& gbn, Vertex v)
{
	auto& g = gbn.graph;

	if(type(v,g) != NODE || matrix(v,g)->type != TERMINATOR)
		return false;

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
		return false;
	else
		remove_vertex(v,gbn);

	return true;
}

bool check_and_apply_F3(GBN& gbn, Vertex v)
{
	auto& g = gbn.graph;

	if(type(v,g) != NODE || matrix(v,g)->type != F)
		return false;

	// list of input ports of FMatrix node
	std::map<Port, std::size_t> input_ports_of_F;
	for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		input_ports_of_F.insert({{ boost::source(e,g), port_from(e,g) }, port_to(e,g) });

	// list successors of Fmatrix
	std::set<Vertex> successors;
	for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		successors.insert(boost::target(e,g));

	// run through all successors 
	bool found_once = false;
	bool found;
	do {
		found = false;
		Port pre_port, post_port;
		Index F_idx;

		for(auto v_suc : successors)
		{
			for(auto [it, end_it] = boost::in_edges(v_suc,g); it != end_it && !found; it++)
			{
				auto e = *it;
				pre_port = { boost::source(e,g), port_from(e,g) };
				post_port = { boost::target(e,g), port_to(e,g) };

				auto map_it = input_ports_of_F.find(pre_port);
				if(map_it != input_ports_of_F.end())
				{
					F_idx = map_it->second;
					found = true;
				}
			}
			if(found)
				break;
		}

		if(found) {
			found_once = true;
			boost::remove_edge_if([&](const Edge& e) {
				return boost::source(e,g) == pre_port.first && boost::target(e,g) == post_port.first && port_from(e,g) == pre_port.second && port_to(e,g) == post_port.second;	
			}, g);
			auto e_new = boost::add_edge(v,post_port.first,g).first;
			put(edge_position, g, e_new, std::pair<std::size_t, std::size_t>{ F_idx, post_port.second });
		}
	}
	while(found);

	return found_once;
}

bool check_and_apply_F4(GBN& gbn, Vertex v_oneb)
{
	auto& g = gbn.graph;

	if(type(v_oneb,g) != NODE || matrix(v_oneb,g)->type != ONE_B)
		return false;

	auto& m = dynamic_cast<OneBMatrix&>(*matrix(v_oneb,g));
	auto b = m.b;

	bool found_once = false;
	bool found;
	do {
		found = false;
		Vertex v_F;
		Index F_port;
		for(auto e : boost::make_iterator_range(boost::out_edges(v_oneb,g)))
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
					found_once = true;
					break;
				}
			}
		}

		if(found) {
			auto p_m_F = matrix(v_F, g);
			auto& m_F = dynamic_cast<FMatrix&>(*p_m_F);

			auto p_m_F_new = std::make_shared<FMatrix>(m_F.k-1, m_F.b);

			std::vector<Port> output_ports;
			for(auto e : boost::make_iterator_range(boost::out_edges(v_F,g)))
				if(port_from(e,g) == F_port)
					output_ports.push_back({ boost::target(e,g), port_to(e,g) });

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

			put(vertex_matrix, g, v_F, p_m_F_new);

			for(auto p : output_ports)
			{
				auto e = boost::add_edge(v_oneb,p.first,g).first;
				put(edge_position,g,e,std::pair<std::size_t,std::size_t>{ 0, p.second });
			}
		}

	}
	while(found);

	return found_once;
}

bool check_and_apply_F5(GBN& gbn, Vertex v)
{
	auto& g = gbn.graph;

	if(type(v,g) != NODE || matrix(v,g)->type != ONE_B)
		return false;

	auto& m = dynamic_cast<OneBMatrix&>(*matrix(v,g));
	auto b = m.b;

	bool found_once = false;
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
				if(m_F.k < 2)
					continue;
				if(m_F.b == !b)
				{
					v_F = v_to;
					F_port = port_to(e,g);
					found = true;
					found_once = true;
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

	return found_once;
}

bool split_vertex_if_multiple_outputs(GBN& gbn, Vertex v)
{
	auto& g = gbn.graph;

	auto& m = *matrix(v,g);

	if(m.m <= 1 || !m.is_stochastic)
		return false;

	recursively_split_vertex(gbn, v);

	return true;
}

bool simplify_matrix_for_duplicate_inputs(GBN& gbn, Vertex v)
{
	auto& g = gbn.graph;
	auto& m = *matrix(v,g);

	std::map<Port,std::vector<std::size_t>> external_input_to_input_port;
	for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
	{
		auto ext_input = std::make_pair(boost::source(e,g), port_from(e,g));
		external_input_to_input_port[ext_input].push_back(port_to(e,g));
	}

	bool found_duplicate = false;
	std::vector<std::vector<std::size_t>> new_to_old_map;
	for(auto t : external_input_to_input_port)
	{
		if(t.second.size() > 1)
			found_duplicate = true;

		new_to_old_map.push_back(t.second);	
	}

	if(!found_duplicate)
		return false;

	new_to_old_map.erase(std::remove_if(new_to_old_map.begin(), new_to_old_map.end(), [](const auto& vec) { return vec.empty(); }), new_to_old_map.end());

	// build new matrix TODO: case of F matrix
	MatrixPtr p_m_new;
	if(m.type == F) {
		auto& m2 = dynamic_cast<FMatrix&>(m);
		p_m_new = std::make_shared<FMatrix>(new_to_old_map.size(), m2.b);
	} else {
		p_m_new = std::make_shared<DynamicMatrix>(new_to_old_map.size(), m.m);
		auto& m_new = *p_m_new;

		unsigned long long i_max_row = 1;
		unsigned long long i_max_col = 1;
		i_max_col = i_max_col << m_new.n;
		i_max_row = i_max_row << m_new.m;
		for(Index i_row = 0; i_row < i_max_row; i_row++)
			for(Index i_col = 0; i_col < i_max_col; i_col++)
			{
				BitVec to(i_row);
				BitVec from_new(i_col);
				BitVec from_old;
				from_old.reset();

				for(std::size_t i = 0; i < new_to_old_map.size(); i++)
					for(auto i_port : new_to_old_map[i])
						if(from_new[i])
							from_old.set(i_port);

				m_new.set(to, from_new, m.get(to,from_old));
			}

		put(vertex_matrix, g, v, p_m_new);
	}

	// rewire vertex
	boost::clear_in_edges(v,g);

	std::size_t i_counter = 0;
	for(const auto [port, i_ports] : external_input_to_input_port)
	{
		auto e = boost::add_edge(port.first, v, g).first;
		put(edge_position, g, e, std::make_pair(port.second,i_counter));
		i_counter++;
	}

	return true;
}
