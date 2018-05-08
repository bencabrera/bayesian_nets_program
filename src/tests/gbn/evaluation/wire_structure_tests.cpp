#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/evaluation/wire_structure.h"
#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/check.h"
#include <fstream>

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

TEST_CASE("Wire structure should have correct number of wires. (four_nodes.gbn)") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vec = { 0,1,2 };
	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec, vertex_set_input_outputs);
	REQUIRE(vertex_set_input_outputs.input_ports.size() == 2);
	REQUIRE(vertex_set_input_outputs.output_ports.size() == 2);
	REQUIRE(wire_structure.wires.size() == 6);
}

TEST_CASE("Every wire should always have at least one input port") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);


	std::vector<Vertex> vec = { 0 };
	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec, vertex_set_input_outputs);
	for(auto& w : wire_structure.wires)
		REQUIRE(w.inside_ports.size() > 0);

	vec = { 2, 3 };
	vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	wire_structure = build_wire_structure_for_vertices(gbn, vec, vertex_set_input_outputs);
	for(auto& w : wire_structure.wires)
		REQUIRE(w.inside_ports.size() > 0);
}

