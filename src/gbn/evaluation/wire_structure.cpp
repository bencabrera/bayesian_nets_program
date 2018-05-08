#include "wire_structure.h"

namespace {
	template<typename T>
		inline bool is_in(const T& val, const std::set<T>& set)
		{
			return set.count(val) != 0;
		}

	void reserve_bitvec_for_wire_structure(WireStructure& wire_structure, const GBN& gbn, const std::vector<Vertex>& inside_vertices)
	{
		auto all_v = all_vertices(gbn);
		auto v_max = *std::max_element(all_v.begin(), all_v.end()); 
		wire_structure.vertex_input_bitvecs.resize(v_max+1);
		wire_structure.vertex_output_bitvecs.resize(v_max+1);
		for(auto v : inside_vertices)
		{
			wire_structure.vertex_input_bitvecs[v] = BitVecPtr(new BitVec());
			wire_structure.vertex_output_bitvecs[v] = BitVecPtr(new BitVec());
		}
		wire_structure.input_bitvec = BitVecPtr(new BitVec());
		wire_structure.output_bitvec = BitVecPtr(new BitVec());
	}
}


// TODO: could sort here s.t. big lookups are performed less frequently
// TODO: optimization: matrices with a lot of zeros in front so that they are rarely updated
WireStructure build_wire_structure_for_whole_gbn(const GBN& gbn)
{
	using Port = WireStructure::Port;

	auto& g = gbn.graph;
	auto inside_vertices = ::inside_vertices(gbn);

	WireStructure wire_structure;
	reserve_bitvec_for_wire_structure(wire_structure, gbn, inside_vertices);

	std::vector<Port> input_ports;
	std::vector<Port> output_ports;
	for(auto v : input_vertices(gbn))
		input_ports.push_back({ v, 0 });
	for(auto v : output_vertices(gbn))
		output_ports.push_back({ v, 0 });
	std::map<Port, std::size_t,PortComparison> input_ports_map, output_ports_map;
	for(std::size_t i = 0; i < input_ports.size(); i++)
		input_ports_map.insert({ input_ports[i], i });
	for(std::size_t i = 0; i < output_ports.size(); i++)
		output_ports_map.insert({ output_ports[i], i });

	// go through all inside vertices 
	std::map<Port, Wire,PortComparison> wires_map;
	for(auto v : inside_vertices)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			Port p{boost::source(e,g), port_from(e,g)};
			auto& wire = wires_map[p]; 
			wire.inside_ports.push_back({ v, wire_structure.vertex_input_bitvecs[v], port_to(e,g) });
			if(type(boost::source(e,g),g) == INPUT)
				wire.io_ports.push_back({ wire_structure.input_bitvec, input_ports_map[p] });
		}

		std::set<Port,PortComparison> unique_out_ports;
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			Port p{boost::source(e,g), port_from(e,g)};
			Port output_p{boost::target(e,g), port_to(e,g)};
			auto& wire = wires_map[p]; 
			unique_out_ports.insert({ v, port_from(e,g) });
			if(type(boost::target(e,g),g) == OUTPUT)
				wire.io_ports.push_back({ wire_structure.output_bitvec, output_ports_map[output_p] });
		}

		for(auto p : unique_out_ports)
		{
			auto& wire = wires_map[p]; 
			wire.inside_ports.push_back({ p.first, wire_structure.vertex_output_bitvecs[v], p.second });
		}
	}


	// for input vertices do this manually
	for(auto input_vertex : input_vertices(gbn))
	{
		Port p{input_vertex, 0};
		if(wires_map.find(p) == wires_map.end())
		{
			Wire w;
			w.io_ports.push_back({ wire_structure.input_bitvec, input_ports_map[{ input_vertex, 0 }] });
			for(auto e : boost::make_iterator_range(boost::out_edges(input_vertex,g)))
			{
				auto v_to = boost::target(e,g);
				if(type(v_to,g) == OUTPUT)
				{
					w.io_ports.push_back({ wire_structure.output_bitvec, output_ports_map[{ v_to, 0 }] });
				}
			}
			wires_map.insert({ p, w });
		}
	}

	for(auto m : wires_map)
	{
		wire_structure.wires.push_back(m.second);
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
	}
}

WireStructure build_wire_structure_for_vertices(const GBN& gbn, std::vector<Vertex> inside_vertices, VertexSetInputOutputs vertex_set_input_outputs)
{
	using Port = WireStructure::Port;

	if(inside_vertices.empty())
		throw std::logic_error("build_wire_structure_for_vertices() can not be called with empty vertex_set.");

	WireStructure wire_structure;
	reserve_bitvec_for_wire_structure(wire_structure, gbn, inside_vertices);

	auto& g = gbn.graph;

	std::map<Port, Wire> wires_map;

	// input / output wires
	{
		Index i = 0;
		for(auto [v, pos] : vertex_set_input_outputs.input_ports)
		{
			Wire w;
			w.inside_ports.push_back({ v, wire_structure.vertex_input_bitvecs[v], pos });
			w.io_ports.push_back({ wire_structure.input_bitvec, i });
			wires_map.insert({ vertex_set_input_outputs.input_external_map[i], w });
			i++;
		}
	}
	{
		Index i = 0;
		for(auto [v, pos] : vertex_set_input_outputs.output_ports)
		{
			Wire w;
			w.inside_ports.push_back({ v, wire_structure.vertex_output_bitvecs[v], pos });
			w.io_ports.push_back({ wire_structure.output_bitvec, i });
			wires_map.insert({ {v,pos}, w });
			i++;
		}
	}

	// inside wires
	const std::set<typename GBNGraph::vertex_descriptor> inside_vertex_set(inside_vertices.begin(), inside_vertices.end());
	std::map<Port, std::vector<Port>,PortComparison> inside_wires_map;
	for(auto v : inside_vertices)
	{
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			if(!is_in(boost::target(e,g), inside_vertex_set))
				continue;

			Port p_from{boost::source(e,g), port_from(e,g)};
			Port p_to{boost::target(e,g), port_to(e,g)};
			inside_wires_map[p_from].push_back(p_to);
		}
	}
	for(auto [p_from, p_to_vec] : inside_wires_map)
	{
		Wire& w = wires_map[p_from];
		w.inside_ports.push_back({ p_from.first, wire_structure.vertex_output_bitvecs[p_from.first], p_from.second });
		for(auto [v_to, pos_to] : p_to_vec)
			w.inside_ports.push_back({ v_to, wire_structure.vertex_input_bitvecs[v_to], pos_to });
	}

	for(auto t : wires_map)
	{
		wire_structure.wires.push_back(t.second);
	}

	return wire_structure;
}
