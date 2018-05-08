#pragma once

#include "../general/gbn.h"
#include "vertex_set_input_output.h"

struct Wire {
	std::vector<std::tuple<Vertex, BitVecPtr, int>> inside_ports;
	std::vector<std::pair<BitVecPtr, int>> io_ports;
	bool active = false; // TODO: remove this and only flip bits?
};


struct WireStructure {
	using Port = std::pair<Vertex, int>;

	std::vector<BitVecPtr> vertex_input_bitvecs;
	std::vector<BitVecPtr> vertex_output_bitvecs;

	BitVecPtr input_bitvec;
	BitVecPtr output_bitvec;

	std::vector<Wire> wires;
};

WireStructure build_wire_structure_for_vertices(const GBN& gbn, std::vector<Vertex> inside_vertices, VertexSetInputOutputs vertex_set_input_outputs);
WireStructure build_wire_structure_for_whole_gbn(const GBN& gbn);

void print_wire_structure(std::ostream& ostr, const WireStructure& wire_structure, const GBN& gbn);

