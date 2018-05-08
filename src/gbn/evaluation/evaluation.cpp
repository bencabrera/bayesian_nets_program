#include "evaluation.h"
#include "../../helpers.hpp"
#include <iostream>
#include "probability_bookkeeper.h"

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


namespace {
	MatrixPtr evaluate_vertices(const GBN& gbn, Index n_inputs, Index n_outputs, WireStructure& wire_structure, std::vector<Vertex> inside_vertices)
	{
		// TODO: check that path closed
		// TODO: check that vertices does not contain input or output vertices
		// TODO: for efficiency move all F matrices to front and multiply it into another matrix

		auto& g = gbn.graph;

		auto& wires = wire_structure.wires;
		MatrixPtr m(new DynamicMatrix(n_inputs, n_outputs));

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


std::pair<MatrixPtr,VertexSetInputOutputs> evaluate_vertices(const GBN& gbn, const std::vector<Vertex> vertices)
{
	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vertices);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vertices, vertex_set_input_outputs);
	auto p_m = evaluate_vertices(gbn, vertex_set_input_outputs.input_ports.size(), vertex_set_input_outputs.output_ports.size(), wire_structure, vertices);

	return { p_m, vertex_set_input_outputs };
}

MatrixPtr evaluate_gbn(const GBN& gbn)
{
	auto wire_structure = build_wire_structure_for_whole_gbn(gbn);
	auto vertices = inside_vertices(gbn);
	return evaluate_vertices(gbn, gbn.n, gbn.m, wire_structure, vertices);
}
