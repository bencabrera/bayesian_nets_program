#include "vertex_set_input_output.h"

namespace {
	template<typename T>
		inline bool is_in(const T& val, const std::set<T>& set)
		{
			return set.count(val) != 0;
		}
}

VertexSetInputOutputs build_inputs_outputs_for_vertices(const GBN& gbn, std::vector<Vertex> inside_vertices)
{
	using Port = VertexSetInputOutputs::Port;

	VertexSetInputOutputs rtn;

	auto& g = gbn.graph;

	// helpers
	std::vector<std::pair<Port, Port>> input_port_pairs; 	// [(v_ext,i_ext),(v_int,i_int)]
	std::vector<std::pair<Port, Port>> output_port_pairs; 	// [(v_int,i_int),(v_ext,i_ext)]
	const std::set<typename GBNGraph::vertex_descriptor> inside_vertex_set(inside_vertices.begin(), inside_vertices.end());

	for(auto v : inside_vertices)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			Port p_from{boost::source(e,g), port_from(e,g)};
			Port p_to{boost::target(e,g), port_to(e,g)};
			if(!is_in(boost::source(e,g), inside_vertex_set))
				input_port_pairs.push_back({ p_from, p_to });
		}

		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			Port p_from{boost::source(e,g), port_from(e,g)};
			Port p_to{boost::target(e,g), port_to(e,g)};
			if(!is_in(boost::target(e,g), inside_vertex_set))
				output_port_pairs.push_back({ p_from, p_to });
		}
	}

	// make input and output ports unique
	std::set<Port,PortComparison> input_port_set;
	for(auto t : input_port_pairs)
		input_port_set.insert(t.second);

	std::set<Port,PortComparison> output_port_set;
	for(auto t : output_port_pairs)
		output_port_set.insert(t.first);

	rtn.input_ports = std::vector<Port>(input_port_set.begin(), input_port_set.end());
	rtn.output_ports = std::vector<Port>(output_port_set.begin(), output_port_set.end());

	// build port indices -> external port map
	std::map<Port,Port,PortComparison> input_port_to_external_map;
	std::map<Port,std::vector<Port>,PortComparison> output_port_to_external_map;
	for(auto t : input_port_pairs)
		input_port_to_external_map.insert({ t.second, t.first });	
	for(auto t : output_port_pairs)
		output_port_to_external_map[t.first].push_back(t.second);

	rtn.input_external_map.resize(rtn.input_ports.size());
	rtn.output_external_map.resize(rtn.output_ports.size());
	{
		Index i = 0;
		for(auto p : rtn.input_ports)
			rtn.input_external_map.at(i++) = input_port_to_external_map.at(p);
	}
	{
		Index i = 0;
		for(auto p : rtn.output_ports)
			rtn.output_external_map.at(i++) = output_port_to_external_map.at(p);
	}

	return rtn;
}

