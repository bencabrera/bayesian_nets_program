#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/evaluation/wire_structure.h"
#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/general/subgbn.h"
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

	auto sub_gbn = SubGBN::make_from_vertices(gbn, { 0, 1, 2 });
	auto wire_structure = build_wire_structure(sub_gbn.gbn);
	REQUIRE(sub_gbn.gbn.n == 2);
	REQUIRE(sub_gbn.gbn.m == 2);
	REQUIRE(wire_structure.wires.size() == 6);
}

TEST_CASE("Every wire should always have at least one input port") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);


	auto sub_gbn = SubGBN::make_from_vertices(gbn, { 0 });
	auto wire_structure = build_wire_structure(sub_gbn.gbn);
	for(auto& w : wire_structure.wires)
		REQUIRE(w.inside_ports.size() > 0);

	auto sub_gbn2 = SubGBN::make_from_vertices(gbn, { 2, 3 });
	wire_structure = build_wire_structure(sub_gbn2.gbn);
	for(auto& w : wire_structure.wires)
		REQUIRE(w.inside_ports.size() > 0);
}

