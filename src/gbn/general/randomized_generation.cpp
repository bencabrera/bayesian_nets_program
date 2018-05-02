#include "randomized_generation.h"

#include <iostream>

#include "../matrix/randomized_generation.h"
#include "../modification/vertex_add_remove.h"

namespace {
	using Port = std::pair<Vertex, int>;

	template<typename Iter, typename T>
	std::vector<T> draw_random_random_elements(Iter begin, Iter end, std::size_t n_items, std::mt19937& mt)
	{
		std::vector<T> vec(begin, end);
		std::shuffle(vec.begin(), vec.end(), mt);

		std::vector<Port> rtn;
		for(std::size_t i = 0; i < std::min(n_items, vec.size()); i++)
			rtn.push_back(vec[i]);

		return rtn;
	}

	std::vector<std::size_t> remove_spaces_in_vec(std::vector<std::size_t> vec)
	{
		if(vec.size() == 0)
			return vec;

		std::sort(vec.begin(), vec.end());

		if(vec[0] != 0)
		{
			for(std::size_t j = 0; j < vec.size(); j++)
				vec[j] -= vec[0];
		}

		if(vec.size() < 2)
			return vec;

		for(std::size_t i = 1; i < vec.size(); i++)
		{
			if(vec[i] > vec[i-1] && vec[i] > vec[i-1]) // because unsigned int -> no underflow
			{
				for(std::size_t j = i; j < vec.size(); j++)
					vec[j]--;
				i--;
			}	
		}

		return vec;
	}
}


GBN generate_random_gbn(std::size_t n, std::size_t m, std::size_t n_inside_vertices, std::mt19937& mt, RandomGBNParams params)
{
	GBN gbn(n,m,n_inside_vertices);

	auto& g = gbn.graph;

	std::uniform_int_distribution<std::size_t> rand_n_precessors(params.n_min_precessors, params.n_max_precessors);
	std::uniform_int_distribution<std::size_t> rand_n_successors(params.n_min_successors, params.n_max_successors);
	std::uniform_int_distribution<std::size_t> rand_n_successors_without_pre(std::min(1ul,params.n_min_successors), params.n_max_successors);

	std::vector<Port> previous_output_ports;
	// add inputs to previous output ports
	for(std::size_t i = 0; i < n; i++)
		previous_output_ports.push_back({ n_inside_vertices + i, 0 });
	
	std::vector<std::pair<std::size_t, std::size_t>> pre_and_successors;

	for(Vertex v = 0; v < n_inside_vertices; v++)
	{
		put(vertex_name, g, v, std::string("v_")+std::to_string(v));
		std::size_t n_precessors = rand_n_precessors(mt);
		std::size_t n_successors = (n_precessors == 0) ? rand_n_successors_without_pre(mt) : rand_n_successors(mt);
		pre_and_successors.push_back({ n_precessors, n_successors });

		auto it = (params.vertex_window_size <= previous_output_ports.size()) ? previous_output_ports.end()-params.vertex_window_size : previous_output_ports.begin();
		auto precessor_ports = draw_random_random_elements<std::vector<Port>::iterator, Port>(it, previous_output_ports.end(), n_precessors, mt);

		std::size_t i_post_port = 0;
		for(auto t : precessor_ports)
		{
			auto e = boost::add_edge(t.first, v, g).first;
			put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ 0, i_post_port++ });
		}

		for(std::size_t i = 0; i < n_successors; i++)
			previous_output_ports.push_back({ v, i });
	}

	std::vector<Vertex> input_ports_without;
	// identify input ports without connection
	for(Vertex i_input = 0; i_input < n; i_input++)
	{
		Vertex v = n_inside_vertices+i_input;
		if(boost::out_degree(v,g) == 0)
			input_ports_without.push_back(v);
	}

	// TODO: ease that case
	if(input_ports_without.size() > m)
		throw std::logic_error("Too many inputs without successors for the outputs");

	// connect output ports to inputs without successor
	for(Vertex i_output = 0; i_output < input_ports_without.size(); i_output++)
	{
		Vertex v = n_inside_vertices+n+i_output;
		auto e = boost::add_edge(input_ports_without[i_output], v, g).first;
		put(edge_position, g, e, std::make_pair(0,0));
	}

	// connect output ports
	std::vector<Port> non_input_ports;
	std::copy_if(previous_output_ports.begin(), previous_output_ports.end(), std::back_inserter(non_input_ports), [&g](const std::pair<Vertex,std::size_t>& p) {
		return type(p.first,g) != INPUT;
	});

	auto precessor_ports_for_outputs = draw_random_random_elements<std::vector<Port>::iterator, Port>(non_input_ports.begin(), non_input_ports.end(), m-input_ports_without.size(), mt);
	std::shuffle(precessor_ports_for_outputs.begin(), precessor_ports_for_outputs.end(), mt);
	std::size_t i_port = 0;
	for(Vertex i_output = 0; i_output < m; i_output++)
	{
		Vertex v = n_inside_vertices+n+i_output;
		if(boost::in_degree(v,g) > 0)
			continue;

		auto& port = precessor_ports_for_outputs[i_port];
		auto e = boost::add_edge(port.first, v, g).first;
		put(edge_position, g, e, std::make_pair(port.second, 0));

		i_port++;
	}

	// remove space in between output ports
	for(Vertex v = 0; v < n_inside_vertices; v++)
	{
		if(boost::out_degree(v,g) == 0 || pre_and_successors[v].second == 0)
			continue;
		
		std::vector<std::size_t> tmp;
		for(std::size_t i = 0; i < boost::out_degree(v,g); i++)
			for(std::size_t p = 0; p < pre_and_successors[v].second; p++)
				tmp.push_back(p);

		std::shuffle(tmp.begin(), tmp.end(), mt);
		std::vector<std::size_t> output_ports(tmp.begin(), tmp.begin()+boost::out_degree(v,g));

		output_ports = remove_spaces_in_vec(output_ports);

		std::shuffle(output_ports.begin(), output_ports.end(), mt);

		std::size_t i = 0;
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			auto p = std::make_pair(output_ports[i++], get(edge_position,g,e).second);
			put(edge_position, g, e, p);
		}
	}


	// remove isolated nodes
	bool found = false;
	do {
		found = false;
		Vertex v_isolated = 0;

		for(auto v : inside_vertices(gbn))
		{
			if(boost::in_degree(v, g) == 0 && boost::out_degree(v,g) == 0)
			{
				found = true;
				v_isolated = v;
				break;
			}
		}

		if(found)
			remove_vertex(v_isolated, gbn);
	}
	while(found) ;


	// add terminator to inside vertices without outputs
	for(auto v : inside_vertices(gbn))
	{
		if(boost::out_degree(v,g) == 0)
		{
			auto v_new = add_vertex(gbn, std::make_shared<TerminatorMatrix>(), "T");
			boost::add_edge(v, v_new, g);
		}
	}


	// assign matrices
	for(auto v : inside_vertices(gbn))
	{
		std::set<std::size_t> input_ports;
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
			input_ports.insert(port_to(e,g));

		std::set<std::size_t> output_ports;
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
			output_ports.insert(port_from(e,g));

		auto n_v = input_ports.size();
		auto m_v = output_ports.size();

		auto p_m = generate_random_matrix(n_v, m_v, mt, params.matrix_params);

		put(vertex_matrix, g, v, p_m);
		switch(p_m->type) {
			case F: {
				auto& m = dynamic_cast<FMatrix&>(*p_m);		
				put(vertex_name, g, v, std::string("F_{") + std::to_string(m.k) + "," + std::to_string(m.b) + "}");
				break;
			}
			case ONE_B: {
				auto& m = dynamic_cast<OneBMatrix&>(*p_m);		
				put(vertex_name, g, v, std::string("1_") + std::to_string(m.b));
				break;
			}

			case TERMINATOR: {
				put(vertex_name, g, v, std::string("T"));
				break;
			}

			default:
				break;
		}
	}

	return gbn;
}
