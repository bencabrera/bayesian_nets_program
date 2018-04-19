#include "gbn_evaluation.h"

namespace {
	template<typename T>
	inline bool is_in(const T& val, const std::set<T>& set)
	{
		return set.count(val) != 0;
	}

	struct Wire {
		Vertex v;
		int pos;
		enum WireType {
			INSIDE,
			INPUT,
			OUTPUT
		} type;
		int i = 0;
	};
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
	std::set<typename GBNGraph::vertex_descriptor> vertex_set;

	// TODO: check that path closed
	// TODO: check that vertices does not contain input or output vertices

	auto& g = graph(gbn);

	// find all wires that are either incoming, outgoing or inside
	std::vector<Wire> wires;
	for(auto v : vertices)
	{
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			auto type = (is_in(boost::source(e,g), vertex_set)) ? Wire::INSIDE : Wire::INPUT;
			wires.push_back(Wire{ boost::source(e,g), port_from(e,g), type });
		}
		for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
			if(!is_in(boost::target(e,g), vertex_set))
				wires.push_back(Wire{ boost::source(e,g), port_from(e,g), Wire::OUTPUT });
	}
		
	// sort wires so that the following is satisfied:
	// - INSIDE < INPUT < OUTPUT
	// - i_j < i_{j+1}
	// - o_j < o_{j+1}
	// - (v,i) < (v+1,0)
	// - (v,i) < (v,i+1)
	// TODO: could sort here s.t. big lookups are performed less frequently
	auto comparison = [&g](const Wire& w1, const Wire& w2) -> bool {
		if(w1.type != w2.type)
			return w1.type < w2.type;
		if(name(w1.v,g) != name(w2.v,g))
			return name(w1.v,g) < name(w2.v,g);

		return w1.pos < w2.pos;
	};
	std::set<Wire, decltype(comparison)> wires_set(wires.begin(), wires.end(), comparison);
	wires = std::vector<Wire>(wires_set.begin(), wires_set.end());

	std::vector<double> probabilities_for_nodes(vertices.size(),0.0);
	std::vector<std::vector<std::pair<Vertex,int>>> wires_to_node_ports(wires.size());
	std::vector<BitVec> vertex_inputs(vertices.size(), BitVec(0));
	std::vector<BitVec> vertex_outputs(vertices.size(), BitVec(0));
	// notes: if prob is zero computation of product can be skipped by a lot because a lot are already zero
}
