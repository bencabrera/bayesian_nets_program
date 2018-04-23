#include "evaluation.h"
#include "../../helpers.hpp"
#include <iostream>

namespace {
	template<typename T>
		inline bool is_in(const T& val, const std::set<T>& set)
		{
			return set.count(val) != 0;
		}
}

// TODO: could sort here s.t. big lookups are performed less frequently
// TODO: optimization: matrices with a lot of zeros in front so that they are rarely updated
WireStructure build_wire_structure(const GBN& gbn, std::vector<Vertex> vertices_vec, bool is_full_gbn)
{
	using Port = WireStructure::Port;

	std::size_t n_vertices = gbn.n_vertices;

	if(vertices_vec.empty())
	{
		is_full_gbn = true;
		for(auto v : vertices(gbn))
			if(type(v,gbn.graph) == NODE)
				vertices_vec.push_back(v);
	}

	WireStructure wire_structure;
	wire_structure.vertex_input_bitvecs.resize(n_vertices);
	wire_structure.vertex_output_bitvecs.resize(n_vertices);
	for(auto v : vertices_vec)
	{
		wire_structure.vertex_input_bitvecs[v] = BitVecPtr(new BitVec());
		wire_structure.vertex_output_bitvecs[v] = BitVecPtr(new BitVec());
	}
	wire_structure.input_bitvec = BitVecPtr(new BitVec());
	wire_structure.output_bitvec = BitVecPtr(new BitVec());

	const std::set<typename GBNGraph::vertex_descriptor> vertex_set(vertices_vec.begin(), vertices_vec.end());
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
	for(auto v : vertices_vec)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
			if(!is_in(boost::source(e,g), vertex_set))
				input_ports_set.insert({ boost::source(e,g), port_from(e,g) });
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
			if(!is_in(boost::target(e,g), vertex_set))
				output_ports_set.insert({ boost::target(e,g), port_to(e,g) });
	}
	if(is_full_gbn)
	{
		std::cout << "IS_FUL_GBN" << std::endl;
		for(auto v : gbn.input_vertices)
			input_ports_set.insert({ v, 0 });
		for(auto v : gbn.output_vertices)
			output_ports_set.insert({ v, 0 });
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
	for(auto v : vertices_vec)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			Port p{boost::source(e,g), port_from(e,g)};
			auto& wire = wires_map[p]; 
			wire.inside_ports.push_back({ v, wire_structure.vertex_input_bitvecs[v], port_to(e,g) });
			if(!is_in(boost::source(e,g), vertex_set))
				wire.io_ports.push_back({ wire_structure.input_bitvec, input_ports_map[p] });
		}

		std::set<Port> unique_out_ports;
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			Port p{boost::source(e,g), port_from(e,g)};
			Port output_p{boost::target(e,g), port_to(e,g)};
			auto& wire = wires_map[p]; 
			unique_out_ports.insert({ v, port_from(e,g) });
			if(!is_in(boost::target(e,g), vertex_set))
				wire.io_ports.push_back({ wire_structure.output_bitvec, output_ports_map[output_p] });
		}

		for(auto p : unique_out_ports)
		{
			auto& wire = wires_map[p]; 
			wire.inside_ports.push_back({ p.first, wire_structure.vertex_output_bitvecs[v], p.second });
		}
	}


	if(is_full_gbn)
	{
		for(auto input_vertex : gbn.input_vertices)
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
	}

	for(auto m : wires_map)
	{
		wire_structure.wires.push_back(m.second);
	}

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

// pseudocode:
// 1. 	for each vertex in vertices check which input edges are inside the vertex set and which not 
// 		(outside => put in input set, inside => put in inside set)
// 2. 	for each vertex in vertices check which output edges are inside the vertex set and which not 
// 		(outside => put in output set)
// => input and output set are the indices for the matrix
// 3. 	order these both sets so that input (i_0, ..., i_n come first) and then the other vertices in any order, same for output
// 4.	now run two nested loops i = 0 =>  i < |inputs|, i = 0 => i < |outputs|: 
// 5.	for each of them all possible allocations of the inside wires have to be tried and multiplied and summed up
MatrixPtr evaluate_gbn(const GBN& gbn, std::vector<Vertex> vertices_vec)
{
	bool is_full_gbn = false;
	if(vertices_vec.empty())
	{
		is_full_gbn = true;
		for(auto v : vertices(gbn))
			if(type(v,gbn.graph) == NODE)
				vertices_vec.push_back(v);
	}
	// TODO: check that path closed
	// TODO: check that vertices does not contain input or output vertices
	// TODO: for efficiency move all F matrices to front and multiply it into another matrix

	auto& g = gbn.graph;
	auto wire_structure = build_wire_structure(gbn, vertices_vec, is_full_gbn);

	auto& wires = wire_structure.wires;
	MatrixPtr m(new DynamicMatrix(wire_structure.input_ports.size(), wire_structure.output_ports.size()));

	// init probabilities to the value at zero
	ProbabilityBookkeeper bk(gbn.n_vertices, vertices_vec);
	for(auto v : vertices_vec)
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

	for(auto v : vertices_vec)
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
