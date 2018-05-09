#include "wire_structure.h"

#include "../../helpers.hpp"
#include <iostream>

WireStructure build_wire_structure(const GBN& gbn)
{
	auto& g = gbn.graph;

	auto input_vertices = ::input_vertices(gbn);
	auto output_vertices = ::output_vertices(gbn);
	auto inside_vertices = ::inside_vertices(gbn);
	std::vector<Vertex> origin_vertices;
	std::copy(input_vertices.begin(), input_vertices.end(), std::back_inserter(origin_vertices));
	std::copy(inside_vertices.begin(), inside_vertices.end(), std::back_inserter(origin_vertices));

	// reserve memory for bitvecs
	WireStructure wire_structure;
	auto all_v = all_vertices(gbn);
	auto v_max = *std::max_element(all_v.begin(), all_v.end()); // TODO: put v_max into GBN as a property
	wire_structure.vertex_input_bitvecs.resize(v_max+1);
	wire_structure.vertex_output_bitvecs.resize(v_max+1);
	for(auto v : ::inside_vertices(gbn))
	{
		wire_structure.vertex_input_bitvecs[v] = std::make_shared<BitVec>();
		wire_structure.vertex_output_bitvecs[v] = std::make_shared<BitVec>();
	}
	wire_structure.input_bitvec = std::make_shared<BitVec>();
	wire_structure.output_bitvec = std::make_shared<BitVec>();

	// build a mapping from all vertex outputs to vertex inputs
	std::map<Port, std::set<Port>> from_to_port_map;
	for(auto v : origin_vertices)
	{
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			Vertex v_from = boost::source(e,g), v_to = boost::target(e,g);
			Port p_from { v_from, port_from(e,g) }, p_to { v_to, port_to(e,g) };
			from_to_port_map[p_from].insert(p_to);
		}
	}

	// build actual wire structure
	for(auto [p_from, ports_to] : from_to_port_map)
	{
		Wire w;

		w.source = p_from;

		if(is_in(p_from.first, input_vertices))
			w.io_ports.push_back({ wire_structure.input_bitvec, input_idx(p_from.first,gbn) });
		else
			w.inside_ports.push_back({ p_from.first, wire_structure.vertex_output_bitvecs[p_from.first], p_from.second });

		for(auto p_to : ports_to)
		{
			if(is_in(p_to.first, output_vertices))
				w.io_ports.push_back({ wire_structure.output_bitvec, output_idx(p_to.first,gbn) });
			else
				w.inside_ports.push_back({ p_to.first, wire_structure.vertex_input_bitvecs[p_to.first], p_to.second });
		}

		wire_structure.wires.push_back(w);
	}

	return wire_structure;
}

void print_wire_structure(std::ostream& ostr, const WireStructure& wire_structure, const GBN& gbn)
{
	ostr << "n_wires: " << wire_structure.wires.size() << std::endl;	

	std::size_t i_wire = 0;
	for(auto w : wire_structure.wires)
	{
		ostr << "--- " << i_wire++ << " --- " << std::endl;
		ostr << "inside_ports: " << std::endl;
		for(auto [v,bitvec,pos] : w.inside_ports)		
		{
			ostr << name(v,gbn.graph) << " ";
			ostr << pos << " ";
			ostr << std::endl;
		}
		ostr << "io_ports: ";
		for(auto [bitvec,pos] : w.io_ports)		
			ostr << pos << " ";
		ostr << std::endl;
	}
}
