#pragma once

#include "gbn.h"

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

	std::vector<Port> input_ports;
	std::vector<Port> output_ports;
};

struct ProbabilityBookkeeper
{
	public:
		ProbabilityBookkeeper(std::size_t n_vertices, std::vector<Vertex> relevant_vertices);

		// can be optimized by dividing by previous etc.
		double get_product();

		void update_one_node(Vertex v, double p);

	private:
		std::vector<std::size_t> lookup;
		std::vector<double> p_vec;

		int n_zeros;
};

WireStructure build_wire_structure(const GBN& gbn, std::vector<Vertex> vertices);
std::vector<Vertex> flip_wire(Wire& wire);
MatrixPtr evaluate_gbn(const GBN& gbn);
