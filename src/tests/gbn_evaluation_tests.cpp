#include "../../libs/catch/catch.hpp"

#include "../gbn/gbn_io.h"
#include "../gbn/gbn_evaluation.h"
#include <fstream>

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

TEST_CASE("Number of wires, input and output port in WireStructure should be correct 1") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto wire_structure = build_wire_structure(gbn, { 0 });
	REQUIRE(wire_structure.wires.size() == 2);
	REQUIRE(wire_structure.input_ports.size() == 1);
	REQUIRE(wire_structure.output_ports.size() == 1);

	wire_structure = build_wire_structure(gbn, { 1 });
	REQUIRE(wire_structure.wires.size() == 2);
	REQUIRE(wire_structure.input_ports.size() == 1);
	REQUIRE(wire_structure.output_ports.size() == 1);

	wire_structure = build_wire_structure(gbn, { 0, 1 });
	REQUIRE(wire_structure.wires.size() == 4);
	REQUIRE(wire_structure.input_ports.size() == 2);
	REQUIRE(wire_structure.output_ports.size() == 2);

	wire_structure = build_wire_structure(gbn, { 0, 1, 2 });
	REQUIRE(wire_structure.wires.size() == 6);
	REQUIRE(wire_structure.input_ports.size() == 2);
	REQUIRE(wire_structure.output_ports.size() == 2);

	wire_structure = build_wire_structure(gbn, { 2 });
	REQUIRE(wire_structure.wires.size() == 4);
	REQUIRE(wire_structure.input_ports.size() == 2);
	REQUIRE(wire_structure.output_ports.size() == 2);

	wire_structure = build_wire_structure(gbn, { 2, 3 });
	REQUIRE(wire_structure.wires.size() == 5);
	REQUIRE(wire_structure.input_ports.size() == 2);
	REQUIRE(wire_structure.output_ports.size() == 1);
}

TEST_CASE("ProbabilityBookkeeper should work correctly") {
	ProbabilityBookkeeper bk(10, {3, 7, 8});

	REQUIRE(bk.get_product() == Approx(0));
	bk.update_one_node(3, 0.5);
	REQUIRE(bk.get_product() == Approx(0));
	bk.update_one_node(7, 0.5);
	REQUIRE(bk.get_product() == Approx(0));
	bk.update_one_node(8, 0.5);
	REQUIRE(bk.get_product() == Approx(0.125));
	bk.update_one_node(7, 0);
	REQUIRE(bk.get_product() == Approx(0));
	bk.update_one_node(8, 0);
	REQUIRE(bk.get_product() == Approx(0));
	bk.update_one_node(7, 0.25);
	REQUIRE(bk.get_product() == Approx(0));
	bk.update_one_node(8, 0.25);
	REQUIRE(bk.get_product() == Approx(0.03125));
}

TEST_CASE("Every wire should always have at least one input port") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto wire_structure = build_wire_structure(gbn, { 0 });
	for(auto& w : wire_structure.wires)
		REQUIRE(w.inside_ports.size() > 0);

	wire_structure = build_wire_structure(gbn, { 2, 3 });
	for(auto& w : wire_structure.wires)
		REQUIRE(w.inside_ports.size() > 0);
}

TEST_CASE("Flipping a wire should turn the right bits on in vertices") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto wire_structure = build_wire_structure(gbn, { 0 });
	std::vector<Wire> left_mosts;
	std::copy_if(wire_structure.wires.begin(), wire_structure.wires.end(), std::back_inserter(left_mosts), [&wire_structure] (const Wire& w) {
		return w.io_ports[0].first == wire_structure.input_bitvec;
	});
	auto& left_most_wire = left_mosts[0];
	REQUIRE(left_most_wire.inside_ports.size() == 1);
	REQUIRE(wire_structure.vertex_input_bitvecs[0]->test(0) == 0);
	auto changed_vertices = flip_wire(left_most_wire);
	REQUIRE(changed_vertices.size() == 1);
	REQUIRE(name(changed_vertices[0],graph(gbn)) == "v_1");
	REQUIRE(wire_structure.vertex_input_bitvecs[0]->test(0) == 1);
}
