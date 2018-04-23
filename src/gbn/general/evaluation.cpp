#include "evaluation.h"
#include "../../helpers.hpp"
#include <iostream>

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

	for(auto v : input_vertices(gbn))
		wire_structure.input_ports.push_back({ v, 0 });
	for(auto v : output_vertices(gbn))
		wire_structure.output_ports.push_back({ v, 0 });
	std::map<Port, std::size_t> input_ports_map, output_ports_map;
	for(std::size_t i = 0; i < wire_structure.input_ports.size(); i++)
		input_ports_map.insert({ wire_structure.input_ports[i], i });
	for(std::size_t i = 0; i < wire_structure.output_ports.size(); i++)
		output_ports_map.insert({ wire_structure.output_ports[i], i });

	// go through all inside vertices 
	std::map<Port, Wire> wires_map;
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

		std::set<Port> unique_out_ports;
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

// TODO: could sort here s.t. big lookups are performed less frequently
// TODO: optimization: matrices with a lot of zeros in front so that they are rarely updated
WireStructure build_wire_structure_for_vertices(const GBN& gbn, std::vector<Vertex> inside_vertices)
{
	using Port = WireStructure::Port;

	if(inside_vertices.empty())
		throw std::logic_error("build_wire_structure_for_vertices() can not be called with empty vertex_set.");

	WireStructure wire_structure;
	reserve_bitvec_for_wire_structure(wire_structure, gbn, inside_vertices);

	const std::set<typename GBNGraph::vertex_descriptor> inside_vertex_set(inside_vertices.begin(), inside_vertices.end());
	auto& g = gbn.graph;

	// find all input and output ports and sort them
	// - INSIDE < NODE && OUTPUT < NODE // - i_j < i_{j+1} // - o_j < o_{j+1} // - (v,i) < (v+1,0) // - (v,i) < (v,i+1)
	auto comparison = [&g](const Port& p1, const Port& p2) -> bool {
		if(type(p1.first,g) != type(p2.first,g))
			return type(p1.first,g) < type(p2.first,g);
		if(name(p1.first,g) != name(p2.first,g))
			return name(p1.first,g) < name(p2.first,g);
		return p1.second < p2.second;
	};
	std::set<Port,decltype(comparison)> input_ports_set(comparison);
	std::set<Port,decltype(comparison)> output_ports_set(comparison);
	for(auto v : inside_vertices)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
			if(!is_in(boost::source(e,g), inside_vertex_set))
				input_ports_set.insert({ boost::source(e,g), port_from(e,g) });
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
			if(!is_in(boost::target(e,g), inside_vertex_set))
				output_ports_set.insert({ boost::target(e,g), port_to(e,g) });
	}
	wire_structure.input_ports = std::vector<Port>(input_ports_set.begin(), input_ports_set.end());
	wire_structure.output_ports = std::vector<Port>(output_ports_set.begin(), output_ports_set.end());
	std::map<Port, std::size_t> input_ports_map, output_ports_map;
	for(std::size_t i = 0; i < wire_structure.input_ports.size(); i++)
		input_ports_map.insert({ wire_structure.input_ports[i], i });
	for(std::size_t i = 0; i < wire_structure.output_ports.size(); i++)
		output_ports_map.insert({ wire_structure.output_ports[i], i });

	// go through all edges again to now build wires
	std::map<Port, Wire> wires_map;
	for(auto v : inside_vertices)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			Port p{boost::source(e,g), port_from(e,g)};
			auto& wire = wires_map[p]; 
			wire.inside_ports.push_back({ v, wire_structure.vertex_input_bitvecs[v], port_to(e,g) });
			if(!is_in(boost::source(e,g), inside_vertex_set))
				wire.io_ports.push_back({ wire_structure.input_bitvec, input_ports_map[p] });
		}

		std::set<Port> unique_out_ports;
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			Port p{boost::source(e,g), port_from(e,g)};
			Port output_p{boost::target(e,g), port_to(e,g)};
			auto& wire = wires_map[p]; 
			unique_out_ports.insert({ v, port_from(e,g) });
			if(!is_in(boost::target(e,g), inside_vertex_set))
				wire.io_ports.push_back({ wire_structure.output_bitvec, output_ports_map[output_p] });
		}

		for(auto p : unique_out_ports)
		{
			auto& wire = wires_map[p]; 
			wire.inside_ports.push_back({ p.first, wire_structure.vertex_output_bitvecs[v], p.second });
		}
	}

	for(auto m : wires_map)
		wire_structure.wires.push_back(m.second);

	return wire_structure;
}

// returns which vertices have to be updated
// if wire. active == false afterwards hints towards carry bit
std::vector<Vertex> flip_wire(Wire& wire)
{
	std::vector<Vertex> changed_vertices;
	wire.active = !wire.active;

	for(auto [v, p_bitvec, i_bit] : wire.inside_ports)
	{
		// TODO: following if could be replaced by one flip without branching?
		if(wire.active)
			p_bitvec->set(i_bit);
		else
			p_bitvec->reset(i_bit);
		changed_vertices.push_back(v); // TODO: could be optimized out by using the vertices from wire...
	}

	for(auto [p_bitvec, i_bit] : wire.io_ports)
	{
		p_bitvec->flip(i_bit);
		// TODO: following if could be replaced by one flip without branching?
		if(wire.active)
			p_bitvec->set(i_bit);
		else
			p_bitvec->reset(i_bit);
	}

	return changed_vertices;
}

ProbabilityBookkeeper::ProbabilityBookkeeper(std::size_t n_vertices, std::vector<Vertex> relevant_vertices)
	:lookup(n_vertices), p_vec(relevant_vertices.size(), 0.0), n_zeros(0)
{
	for(std::size_t i = 0; i < relevant_vertices.size(); i++)
		lookup[relevant_vertices[i]] = i;	
}

// can be optimized by dividing by previous etc.
double ProbabilityBookkeeper::get_product()
{
	if(n_zeros == 0)
	{
		double val = 1;
		for(const auto& p : p_vec)
			val *= p;

		return val;
	}
	else
		return 0.0;
}

void ProbabilityBookkeeper::update_one_node(Vertex v, double p)
{
	auto& p_v = p_vec[lookup[v]];
	bool is_p_zero = prob_is_zero(p);
	if(n_zeros > 0)
	{
		bool is_p_v_zero = prob_is_zero(p_v);
		if(is_p_zero && !is_p_v_zero)
		{
			p_v = 0.0;
			n_zeros++;
		}
		if(!is_p_zero && is_p_v_zero)
		{
			p_v = p;
			n_zeros--;
		}
		if(!is_p_zero && !is_p_v_zero)
		{
			p_v = p;
		}
	}
	else
	{
		if(is_p_zero) {
			p_v = 0.0;
			n_zeros++;
		}
		else
		{
			p_v = p;
		}
	}
}

namespace {
	MatrixPtr evaluate_vertices(const GBN& gbn, WireStructure& wire_structure, std::vector<Vertex> inside_vertices)
	{
		// TODO: check that path closed
		// TODO: check that vertices does not contain input or output vertices
		// TODO: for efficiency move all F matrices to front and multiply it into another matrix

		auto& g = gbn.graph;

		auto& wires = wire_structure.wires;
		MatrixPtr m(new DynamicMatrix(wire_structure.input_ports.size(), wire_structure.output_ports.size()));

		// init probabilities to the value at zero
		auto all_v = all_vertices(gbn);
		auto v_max = *std::max_element(all_v.begin(), all_v.end()); 
		ProbabilityBookkeeper bk(v_max+1, inside_vertices);
		for(auto v : inside_vertices)
		{
			auto& m_v = *matrix(v,g);
			auto p = m_v.get(*wire_structure.vertex_output_bitvecs[v], *wire_structure.vertex_input_bitvecs[v]);
			bk.update_one_node(v,p);
		}
		double product = bk.get_product();
		m->add(*wire_structure.output_bitvec, *wire_structure.input_bitvec, product);

		std::size_t i_wire = 0;
		const std::size_t max_i_wire = wires.size();
		std::set<Vertex> affected_vertices; // TODO: optimization: replace this with flat set
		while(i_wire < max_i_wire) // TODO: do this more efficiently with gray codes -> only one wire flip at a time needed
		{
			auto& w = wires[i_wire];
			auto affected_vertices_vec = flip_wire(w); 
			affected_vertices.insert(affected_vertices_vec.begin(), affected_vertices_vec.end());

			if(!w.active) // carry bit needed
			{
				i_wire++;
			}
			else
			{
				for(auto v : affected_vertices)
				{
					const auto& m_v = *matrix(v,g);
					auto p = m_v.get(*wire_structure.vertex_output_bitvecs[v], *wire_structure.vertex_input_bitvecs[v]);
					bk.update_one_node(v,p);
				}

				double product = bk.get_product();
				m->add(*wire_structure.output_bitvec, *wire_structure.input_bitvec, product);
				affected_vertices.clear();
				i_wire = 0;	
			}
		}

		for(auto v : inside_vertices)
		{
			auto& m_v = *matrix(v,g);
			if(!m_v.is_stochastic)
			{
				m->is_stochastic = false;
				break;
			}
		}

		return m;
	}
}


MatrixPtr evaluate_vertices(const GBN& gbn, const std::vector<Vertex> vertices)
{
	auto wire_structure = build_wire_structure_for_vertices(gbn, vertices);
	return evaluate_vertices(gbn, wire_structure, vertices);
}

MatrixPtr evaluate_gbn(const GBN& gbn)
{
	auto wire_structure = build_wire_structure_for_whole_gbn(gbn);
	auto vertices = inside_vertices(gbn);
	return evaluate_vertices(gbn, wire_structure, vertices);
}
