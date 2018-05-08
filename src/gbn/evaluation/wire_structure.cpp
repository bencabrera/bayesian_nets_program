#include "wire_structure.h"

#include "../../helpers.hpp"
#include "vertex_set_input_output.h"

// PSEUDOCODE:
// 1. build, order and define input and output ports
// 	1.1 if whole GBN input and outputs match that of network inputs and outputs
// 	1.2 if only part of GBN then input and output ports can be build by calling vertex_set_input_output.h functions
// 2. For every inside_vertex v (inside refering to either vertex set or actual inside vertices) go through all successor ports
// 	2.1 If v has out_degree > 0 create a wire, otherwise continue
// 	2.2 Hook up with vertex_output_bitvec of v using port_from(e)
// 	2.3 Look at successor port:
// 	 2.3.1 If successor port is in output ports: hook up with output_bitvec using position of output port
// 	 2.3.1 If successor port not in output ports: hook up vertex_input_bitvec of target(e)

namespace {
	void reserve_bitvec_for_wire_structure(WireStructure& wire_structure, const GBN& gbn, const std::vector<Vertex>& inside_vertices)
	{
		auto all_v = all_vertices(gbn);
		auto v_max = *std::max_element(all_v.begin(), all_v.end()); 
		wire_structure.vertex_input_bitvecs.resize(v_max+1);
		wire_structure.vertex_output_bitvecs.resize(v_max+1);
		for(auto v : inside_vertices)
		{
			wire_structure.vertex_input_bitvecs[v] = std::make_shared<BitVec>();
			wire_structure.vertex_output_bitvecs[v] = std::make_shared<BitVec>();
		}
		wire_structure.input_bitvec = std::make_shared<BitVec>();
		wire_structure.output_bitvec = std::make_shared<BitVec>();
	}
}

WireStructure build_wire_structure(const GBN& gbn, std::vector<Vertex> inside_vertices)
{
	if(inside_vertices.empty())
		inside_vertices = ::inside_vertices(gbn);

	auto& g = gbn.graph;

	auto [input_ports, output_ports] = build_inputs_outputs_for_vertices(gbn,inside_vertices);
	std::map<Port, std::size_t> input_port_map, output_port_map;
	{
		Index i_port = 0;
		for(auto p : input_ports)
			input_port_map[p] = i_port++;
	}
	{
		Index i_port = 0;
		for(auto p : output_ports)
			output_port_map[p] = i_port++;
	}

	std::map<Port, std::set<Port>> from_to_port_map;
	for(auto v : inside_vertices)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			Vertex v_from = boost::source(e,g), v_to = boost::target(e,g);
			Port p_from { v_from, port_from(e,g) }, p_to { v_to, port_to(e,g) };
			from_to_port_map[p_from].insert(p_to);
		}
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			Vertex v_from = boost::source(e,g), v_to = boost::target(e,g);
			Port p_from { v_from, port_from(e,g) }, p_to { v_to, port_to(e,g) };
			from_to_port_map[p_from].insert(p_to);
		}
	}

	WireStructure wire_structure;
	for(auto [p_from, ports_to] : from_to_port_map)
	{
		Wire w;
		if(is_in(p_from, input_port_map))
			w.io_ports.push_back({ wire_structure.input_bitvec, input_port_map[p_from] });
		if(is_in(p_from, output_port_map))
		{
			w.io_ports.push_back({ wire_structure.output_bitvec, output_port_map[p_from] });
			w.inside_ports.push_back({ p_from.first, wire_structure.vertex_output_bitvecs[p_from.first], p_from.second });
		}

		for(auto p_to : ports_to)
		{
			if(is_in(p_to.first, inside_vertices))
				w.inside_ports.push_back({ p_to.first, wire_structure.vertex_input_bitvecs[p_to.first], p_to.second });
		}

		wire_structure.wires.push_back(w);
	}

	return wire_structure;
}
