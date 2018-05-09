#pragma once

#include "../general/gbn.h"

struct Wire {
	std::vector<std::tuple<Vertex, BitVecPtr, Index>> inside_ports;
	std::vector<std::pair<BitVecPtr, Index>> io_ports;
	bool active = false; // TODO: remove this and only flip bits?
	Port source;
};


struct WireStructure {
	std::vector<BitVecPtr> vertex_input_bitvecs;
	std::vector<BitVecPtr> vertex_output_bitvecs;

	BitVecPtr input_bitvec;
	BitVecPtr output_bitvec;

	std::vector<Wire> wires;
};

WireStructure build_wire_structure_for_vertices(const GBN& gbn, std::vector<Vertex> inside_vertices);
WireStructure build_wire_structure_for_gbn(const GBN& gbn);
void print_wire_structure(std::ostream& ostr, const WireStructure& wire_structure, const GBN& gbn);
