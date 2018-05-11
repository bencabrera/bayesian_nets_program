#include "randomized_generation.h"

#include <iostream>
#include <cassert>

#include "../matrix/randomized_generation.h"
#include "../modification/vertex_add_remove.h"

namespace {
	using Port = std::pair<Vertex, int>;

	template<typename Iter, typename T>
	T draw_random_element(Iter begin, Iter end, std::mt19937& mt)
	{
		std::vector<T> vec(begin, end);
		if(vec.empty())
			throw std::logic_error("Cannot randomly draw element from empty vector");

		std::shuffle(vec.begin(), vec.end(), mt);

		return vec[0];
	}

	template<typename Iter, typename T>
	std::vector<T> draw_random_elements(Iter begin, Iter end, std::size_t n_items, std::mt19937& mt)
	{
		std::vector<T> vec(begin, end);
		if(vec.empty())
			throw std::logic_error("Cannot randomly draw element from empty vector");

		std::shuffle(vec.begin(), vec.end(), mt);

		std::vector<T> rtn;
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

	std::pair<std::vector<Port>, std::vector<Port>> generate_random_ports(std::size_t n, std::size_t m, std::size_t n_inside_vertices, std::mt19937& mt, RandomGBNParams params)
	{
		std::uniform_int_distribution<std::size_t> rand_n_precessors(params.n_min_precessors, params.n_max_precessors);
		std::uniform_int_distribution<std::size_t> rand_n_successors(params.n_min_successors, params.n_max_successors);
		std::uniform_int_distribution<std::size_t> rand_n_successors_without_pre(std::min(1ul,params.n_min_successors), params.n_max_successors);

		std::vector<Port> input_ports, output_ports;

		// add network inputs as first output ports
		for(std::size_t i = 0; i < n; i++)
			output_ports.push_back({ n_inside_vertices + i, 0 });

		for(Vertex v = 0; v < n_inside_vertices; v++)
		{
			std::size_t n_precessors = (v == 0 && n == 0) ? 0 : rand_n_precessors(mt); // the first vertex in a net without input should never have an input port
			std::size_t n_successors = (n_precessors == 0) ? rand_n_successors_without_pre(mt) : rand_n_successors(mt);

			for(Index i_port = 0; i_port < n_precessors; i_port++)
				input_ports.push_back({ v, i_port });
			for(Index i_port = 0; i_port < n_successors; i_port++)
				output_ports.push_back({ v, i_port });
		}
		
		// add network outputs as last input ports
		for(std::size_t i = 0; i < m; i++)
			input_ports.push_back({ n_inside_vertices + n + i, 0 });

		return { input_ports, output_ports };
	}

	// constraints to satisfy:
	// - every network input should be used at least once
	// - every network output should be used at least once
	// - every input port should be connected to only the last n_window_size output_ports
	std::map<Port, Port> randomly_connect_ports(std::size_t n, std::size_t /*m*/, std::size_t n_inside_vertices, std::mt19937& mt, RandomGBNParams params, const std::pair<std::vector<Port>, std::vector<Port>>& ports)
	{
		const auto& input_ports = ports.first;
		const auto& output_ports = ports.second;

		std::map<Port, Port> input_to_output_map, output_to_input_map;

		// first run through all input ports and connect them to the last n_window_size output_ports
		for(auto& input_port : input_ports)
		{
			// find first output port that belongs to same node than this input port
			std::vector<Port> ports_before;
			std::copy_if(output_ports.begin(), output_ports.end(), std::back_inserter(ports_before), [&](const Port& output_port) {
				if(output_port.first >= n_inside_vertices)
					return true;
				return input_port.first > output_port.first;
			});
			std::size_t i_window = std::min(params.vertex_window_size, ports_before.size());
			auto output_port = draw_random_element<std::vector<Port>::reverse_iterator, Port>(ports_before.rbegin(), ports_before.rbegin() + i_window, mt);

			// auto end_it = std::find_if(output_ports.begin(), output_ports.end(), [&](const Port& output_port) {
				// if(output_port.first >= n_inside_vertices)
					// return true;
				// return input_port.first < output_port.first;
			// });
			// std::size_t i_end = std::distance(output_ports.begin(), end_it);
			// std::size_t i_begin = (i_end > params.vertex_window_size) ? i_end - params.vertex_window_size : 0;
			// assert(i_begin < i_end);
			// std::vector<Port> ports_in_window(output_ports.begin()+i_begin, output_ports.begin()+i_end);
			// auto output_port = draw_random_element<std::vector<Port>::iterator, Port>(ports_in_window.begin(), ports_in_window.end(), mt);
			input_to_output_map.insert({ input_port, output_port });	
			output_to_input_map.insert({ output_port, input_port });	
		}

		// run through map and store all network inputs that were not used
		std::vector<Index> unused_network_inputs;
		for(Index i_input = 0; i_input < n; i_input++)
			if(!output_to_input_map.count({ n_inside_vertices + i_input, 0 }))
				unused_network_inputs.push_back(i_input);

		// input ports that do not itself point to network inputs already
		std::vector<Port> input_ports_not_pointing_to_network_inputs;	
		std::copy_if(input_ports.begin(), input_ports.end(), std::back_inserter(input_ports_not_pointing_to_network_inputs), [&input_to_output_map, &n_inside_vertices] (const Port& input_port) {
			return input_to_output_map[input_port].first < n_inside_vertices;
		});

		// take the first window sized inputs, out of them randomly choose enough and rewire them to unused network inputs
		std::size_t i_window = std::min(params.vertex_window_size, input_ports_not_pointing_to_network_inputs.size());
		if(i_window < unused_network_inputs.size())
			throw std::logic_error("i_window < unused_network_inputs");

		auto input_ports_to_rewire = draw_random_elements<std::vector<Port>::iterator, Port>(input_ports_not_pointing_to_network_inputs.begin(), input_ports_not_pointing_to_network_inputs.begin() + i_window, unused_network_inputs.size(), mt);
		for(std::size_t i = 0; i < input_ports_to_rewire.size(); i++)
		{
			input_to_output_map[input_ports_to_rewire[i]] = { n_inside_vertices + unused_network_inputs[i], 0 };
		}

		return input_to_output_map;
	}

	void assign_matching_matrices_to_inside_vertices(GBN& gbn, RandomMatrixParams& matrix_params, std::mt19937& mt) {
		auto& g = gbn.graph;

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

			auto p_m = generate_random_matrix(n_v, m_v, mt, matrix_params);

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
	}

	void remove_isolated_nodes(GBN& gbn)
	{
		auto& g = gbn.graph;

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
		while(found);
	}
}

// window size has to be larger than n
GBN generate_random_gbn(std::size_t n, std::size_t m, std::size_t n_inside_vertices, std::mt19937& mt, RandomGBNParams params)
{
	params.n_inputs = n;
	params.n_outputs = m;
	params.n_outputs = n_inside_vertices;

	return generate_random_gbn(mt,params);
}

// window size has to be larger than n
GBN generate_random_gbn(std::mt19937& mt, RandomGBNParams params)
{
	auto ports = generate_random_ports(params.n_inputs, params.n_outputs, params.n_inside_vertices, mt, params);
	auto input_to_output_map = randomly_connect_ports(params.n_inputs, params.n_outputs, params.n_inside_vertices, mt, params, ports);

	GBN gbn(params.n_inputs,params.n_outputs,params.n_inside_vertices);
	auto& g = gbn.graph;

	// wire up graph according to input output map
	for(auto input_to_output : input_to_output_map)
	{
		auto& u = input_to_output.second.first;
		auto& v = input_to_output.first.first;
		auto& port_from = input_to_output.second.second;
		auto& port_to = input_to_output.first.second;
		auto e = boost::add_edge(u,v,g).first;
		put(edge_position, g, e, std::make_pair(port_from, port_to));
	}

	remove_isolated_nodes(gbn);

	// reset output ports of inside vertices so that there are no space in ports
	for(auto v : inside_vertices(gbn))
	{
		if(boost::out_degree(v,g) == 0)
			continue;

		std::vector<std::size_t> tmp;
		for(std::size_t i = 0; i < boost::out_degree(v,g); i++)
			for(std::size_t p = 0; p < boost::out_degree(v,g); p++)
				tmp.push_back(p);

		auto new_output_ports = draw_random_elements<std::vector<std::size_t>::iterator, std::size_t>(tmp.begin(), tmp.end(), boost::out_degree(v,g),mt);
		new_output_ports = remove_spaces_in_vec(new_output_ports);
		std::shuffle(new_output_ports.begin(), new_output_ports.end(), mt);

		std::size_t i = 0;
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			auto p = std::make_pair(new_output_ports[i++], get(edge_position,g,e).second);
			put(edge_position, g, e, p);
		}
	}

	// add terminator to inside vertices without outputs
	for(auto v : inside_vertices(gbn))
	{
		if(boost::out_degree(v,g) == 0)
		{
			auto v_new = add_vertex(gbn, std::make_shared<TerminatorMatrix>(), "T");
			boost::add_edge(v, v_new, g);
		}
	}

	assign_matching_matrices_to_inside_vertices(gbn, params.matrix_params, mt);

	return gbn;
}
