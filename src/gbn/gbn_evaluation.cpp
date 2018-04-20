#include "gbn_evaluation.h"
#include "../helpers.hpp"
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
WireStructure build_wire_structure(const GBN& gbn, std::vector<Vertex> vertices)
{
	std::size_t n_vertices = boost::num_vertices(graph(gbn));
	using Port = WireStructure::Port;

	WireStructure wire_structure;
	wire_structure.vertex_input_bitvecs.resize(n_vertices);
	wire_structure.vertex_output_bitvecs.resize(n_vertices);
	for(auto v : vertices)
	{
		wire_structure.vertex_input_bitvecs[v] = BitVecPtr(new BitVec());
		wire_structure.vertex_output_bitvecs[v] = BitVecPtr(new BitVec());
	}
	wire_structure.input_bitvec = BitVecPtr(new BitVec());
	wire_structure.output_bitvec = BitVecPtr(new BitVec());

	const std::set<typename GBNGraph::vertex_descriptor> vertex_set(vertices.begin(), vertices.end());
	auto& g = graph(gbn);

	// find all input and output ports and sort them
	// - INSIDE < NODE && OUTPUT < NODE // - i_j < i_{j+1} // - o_j < o_{j+1} // - (v,i) < (v+1,0) // - (v,i) < (v,i+1)
	auto comparison = [&g](const Port& p1, const Port& p2) -> bool {
		if(node_type(p1.first,g) != node_type(p2.first,g))
			return node_type(p1.first,g) < node_type(p2.first,g);
		if(name(p1.first,g) != name(p2.first,g))
			return name(p1.first,g) < name(p2.first,g);
		return p1.second < p2.second;
	};
	std::set<Port,decltype(comparison)> input_ports_set(comparison);
	std::set<Port,decltype(comparison)> output_ports_set(comparison);
	for(auto v : vertices)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
			if(!is_in(boost::source(e,g), vertex_set))
				input_ports_set.insert({ boost::source(e,g), port_from(e,g) });
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
			if(!is_in(boost::target(e,g), vertex_set))
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
	for(auto v : vertices)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			Port p{boost::source(e,g), port_from(e,g)};
			auto& wire = wires_map[p]; 
			wire.inside_ports.push_back({ v, wire_structure.vertex_input_bitvecs[v], port_to(e,g) });
			if(!is_in(boost::source(e,g), vertex_set))
				wire.io_ports.push_back({ wire_structure.input_bitvec, input_ports_map[p] });
		}

		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
		{
			Port p{boost::source(e,g), port_from(e,g)};
			auto& wire = wires_map[p]; 
			wire.inside_ports.push_back({ v, wire_structure.vertex_output_bitvecs[v], port_from(e,g) });
			if(!is_in(boost::target(e,g), vertex_set))
				wire.io_ports.push_back({ wire_structure.output_bitvec, output_ports_map[p] });
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
MatrixPtr evaluate_gbn(const GBN& gbn, const std::vector<Vertex> vertices)
{

	// TODO: check that path closed
	// TODO: check that vertices does not contain input or output vertices
	// TODO: for efficiency move all F matrices to front and multiply it into another matrix

	auto& g = graph(gbn);



	std::vector<double> probabilities_for_nodes(vertices.size(),0.0);
	std::vector<BitVec> vertex_inputs(vertices.size(), BitVec());
	std::vector<BitVec> vertex_outputs(vertices.size(), BitVec());
	// notes: if prob is zero computation of product can be skipped by a lot because a lot are already zero
}
