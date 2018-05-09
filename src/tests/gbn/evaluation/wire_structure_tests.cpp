#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/evaluation/wire_structure.h"
#include "../../../gbn/evaluation/vertex_set_input_output.h"
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
	auto [input_ports, output_ports] =  build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec);
	REQUIRE(input_ports.size() == 2);
	REQUIRE(output_ports.size() == 2);
	REQUIRE(wire_structure.wires.size() == 6);
}

TEST_CASE("Every wire should always have at least one input port") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);


	std::vector<Vertex> vec = { 0 };
	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec);
	for(auto& w : wire_structure.wires)
		REQUIRE(w.inside_ports.size() > 0);

	vec = { 2, 3 };
	vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	wire_structure = build_wire_structure_for_vertices(gbn, vec);
	for(auto& w : wire_structure.wires)
		REQUIRE(w.inside_ports.size() > 0);
}

