#include "simplification.h"

#include <iostream>
#include "../matrix/matrix_io.h"
#include "vertex_add_remove.h"
#include "merging.h"
#include "splitting.h"
#include "../general/path_closing.h"

namespace {
	using Port = std::pair<Vertex, std::size_t>;
}

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

// bool check_and_apply_F3(GBN& gbn, Vertex v)
// {
// auto& g = gbn.graph;

// if(type(v,g) != NODE || matrix(v,g)->type != F)
// return false;

// auto& m = dynamic_cast<FMatrix&>(*matrix(v,g));
// if(m.k < 2)
// return false;

// // check for all precessors of F if they split their signal to multiple ports
// bool found_once = false;
// bool found;
// do {
// found = false;
// Vertex v_pre, v_post;
// Index v_pre_port, v_post_port, F_port;

// for(auto [it, end_it] = boost::in_edges(v,g); it != end_it && !found; it++)
// {
// auto e = *it;
// v_pre = boost::source(e,g);
// v_pre_port = port_from(e,g);
// F_port = port_to(e,g);
// for(auto e_pre_post : boost::make_iterator_range(boost::out_edges(v_pre,g)))
// {
// v_post = boost::target(e_pre_post,g);
// if(port_from(e_pre_post,g) == v_pre_port && v_post != v)
// {
// found = true;
// v_post_port = port_to(e_pre_post,g);
// break;
// }
// }
// }

// if(found) {
// found_once = true;
// boost::remove_edge_if([&](const Edge& e) {
// return boost::source(e,g) == v_pre && boost::target(e,g) == v_post && port_from(e,g) == v_pre_port && port_to(e,g) == v_post_port;	
// }, g);
// auto e_new = boost::add_edge(v,v_post,g).first;
// put(edge_position, g, e_new, std::pair<std::size_t, std::size_t>{ F_port, v_post_port });
// }
// }
// while(found);

// return found_once;
// }

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

bool gbn_eliminate_without_outputs(GBN& gbn)
{
	auto& g = gbn.graph;

	bool found = false;
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
		if(!connected_to_output && matrix(v,g)->is_stochastic && matrix(v,g)->type != TERMINATOR)
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
			successors.push_back(boost::target(e,g));

		successors = path_closing(gbn, successors);

		auto v_new = merge_vertices(gbn, successors);

		recursively_split_vertex(gbn, v_new);
	}

	return found;
}

bool gbn_switch_substoch_to_front(GBN& gbn)
{
	auto& g = gbn.graph;

	bool found = false;
	Vertex v_substoch;
	auto vec = inside_vertices(gbn);
	for(auto v : vec)	
	{
		auto m = matrix(v,g);
		if(!m->is_stochastic && boost::in_degree(v,g) > 0) {
			v_substoch = v;
			found = true;	
		}
	}

	std::cout << "found: " << found << std::endl;

	if(found) {
		std::vector<Vertex> precessors;
		precessors.push_back(v_substoch);
		for(auto e : boost::make_iterator_range(boost::in_edges(v_substoch,g)))
		{
			precessors.push_back(boost::source(e,g));
		}

		precessors = path_closing(gbn, precessors);
		auto v_new = merge_vertices(gbn, precessors);

		recursively_split_vertex(gbn, v_new);
	}

	return found;
}

bool normalize_substoch_front_vertices(GBN& gbn)
{
	std::cout << "NORMALIZATION" << std::endl;
	auto& g = gbn.graph;

	bool found = false;
	Vertex v_substoch;
	auto vec = inside_vertices(gbn);
	for(auto v : vec)	
	{
		auto m = matrix(v,g);
		if(!m->is_stochastic && boost::in_degree(v,g) == 0) {
			v_substoch = v;
			found = true;	
		}
	}

	std::cout << "found substoch: " << found << std::endl;

	if(found) {
		std::cout << name(v_substoch,g) << std::endl;
		for(auto v : vec)	
		{
			std::cout << name(v,g) << std::endl;
			print_matrix(std::cout, *matrix(v,g));
		}
		auto m = matrix(v_substoch,g);
		double sum = m->get(BitVec(0),BitVec(0))+m->get(BitVec(1),BitVec(0));
		std::cout << "sum: " << sum << std::endl;

		m->set(BitVec(0),BitVec(0), m->get(BitVec(0),BitVec(0))/sum);
		m->set(BitVec(1),BitVec(0), m->get(BitVec(1),BitVec(0))/sum);
		m->is_stochastic = true;

		for(auto v : vec)	
		{
			std::cout << name(v,g) << std::endl;
			print_matrix(std::cout, *matrix(v,g));
		}
	}

	return found;
}


void gbn_simplification(GBN& gbn)
{
	bool found_at_least_one = false;
	// do {
	for(auto v: inside_vertices(gbn))	
	{
		found_at_least_one = found_at_least_one || check_and_apply_F1(gbn, v);
		found_at_least_one = found_at_least_one || check_and_apply_F2(gbn, v);
		found_at_least_one = found_at_least_one || check_and_apply_CoUnit(gbn, v);
		found_at_least_one = found_at_least_one || check_and_apply_F3(gbn, v);
		found_at_least_one = found_at_least_one || check_and_apply_F4(gbn, v);
		found_at_least_one = found_at_least_one || check_and_apply_F5(gbn, v);
	}
	// }
	// while(found_at_least_one);

	// bool found;
	// do {
	// found = gbn_eliminate_without_outputs(gbn);
	// }
	// while(found);

	// do {
	// found = gbn_switch_substoch_to_front(gbn);
	// }
	// while(found);

	// do {
	// found = normalize_substoch_front_vertices(gbn);
	// }
	// while(found);
}
