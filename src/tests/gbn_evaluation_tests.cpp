#include "../../libs/catch/catch.hpp"

#include "../gbn/gbn_evaluation.h"
#include "../gbn/matrix_io.h"
#include <fstream>
#include "../gbn/gbn_check.h"
#include "../gbn/gbn_io.h"

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
	REQUIRE(name(changed_vertices[0],gbn.graph) == "v_1");
	REQUIRE(wire_structure.vertex_input_bitvecs[0]->test(0) == 1);
	REQUIRE(wire_structure.input_bitvec->test(0) == 1);

	std::vector<Wire> right_mosts;
	std::copy_if(wire_structure.wires.begin(), wire_structure.wires.end(), std::back_inserter(right_mosts), [&wire_structure] (const Wire& w) {
		return w.io_ports[0].first == wire_structure.output_bitvec;
	});
	auto& right_most_wire = right_mosts[0];
	REQUIRE(right_most_wire.inside_ports.size() == 1);
	REQUIRE(wire_structure.vertex_output_bitvecs[0]->test(0) == 0);
	changed_vertices = flip_wire(right_most_wire);
	REQUIRE(changed_vertices.size() == 1);
	REQUIRE(name(changed_vertices[0],gbn.graph) == "v_1");
	REQUIRE(wire_structure.vertex_output_bitvecs[0]->test(0) == 1);
	REQUIRE(wire_structure.output_bitvec->test(0) == 1);
}

TEST_CASE("Flipping a wire should turn the right bits on in vertices 2 (split case)") {

	std::ifstream f(TEST_INSTANCE_FOLDER + "split.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto wire_structure = build_wire_structure(gbn, { 0,1,2 });
	std::vector<Wire> tmp;
	std::copy_if(wire_structure.wires.begin(), wire_structure.wires.end(), std::back_inserter(tmp), [&wire_structure] (const Wire& w) {
		return w.inside_ports.size() == 3;
	});
	REQUIRE(tmp.size() == 1);
	auto& w = tmp[0];
	REQUIRE(w.inside_ports.size() == 3);
	REQUIRE(w.io_ports.size() == 0);

	auto affected_vertices = flip_wire(w);
	REQUIRE(affected_vertices.size() == 3);
}

TEST_CASE("Computing one product should work") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "split.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> picked_vertices({ 0,1,2 });
	auto wire_structure = build_wire_structure(gbn, picked_vertices);
	std::vector<Wire> tmp;
	std::copy_if(wire_structure.wires.begin(), wire_structure.wires.end(), std::back_inserter(tmp), [&wire_structure] (const Wire& w) {
		return w.inside_ports.size() == 3;
	});
	REQUIRE(tmp.size() == 1);
	auto& w = tmp[0];
	auto affected_vertices = flip_wire(w);

	auto g = gbn.graph;

	double product = 1;
	for(auto v : picked_vertices)
	{
		const auto& m = *matrix(v,g);
		
		product *= m.get(*wire_structure.vertex_output_bitvecs[v], *wire_structure.vertex_input_bitvecs[v]);
	}

	REQUIRE(product == Approx(0.666666666*0.5*0.5));
}

TEST_CASE("Evaluation of single node should just give that matrix of node 1") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "one_node.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> picked_vertices({ 0 });

	auto m = evaluate_gbn(gbn, picked_vertices);

	REQUIRE(m->get(BitVec("0"),BitVec("0")) == Approx(0.333333333333));
	REQUIRE(m->get(BitVec("1"),BitVec("0")) == Approx(0.666666666666));
	REQUIRE(m->get(BitVec("0"),BitVec("1")) == Approx(0.75));
	REQUIRE(m->get(BitVec("1"),BitVec("1")) == Approx(0.25));
}

TEST_CASE("Evaluation of single node should just give that matrix of node 2") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "one_node_two_wires.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> picked_vertices({ 0 });

	auto wire_structure = build_wire_structure(gbn, picked_vertices);
	REQUIRE(wire_structure.input_ports.size() == 2);
	REQUIRE(wire_structure.output_ports.size() == 2);
	auto m = evaluate_gbn(gbn, picked_vertices);

	REQUIRE(m->get(BitVec("00"),BitVec("00")) == Approx(0.25));
	REQUIRE(m->get(BitVec("00"),BitVec("01")) == Approx(0.333333333333));
	REQUIRE(m->get(BitVec("00"),BitVec("10")) == Approx(0.5));
	REQUIRE(m->get(BitVec("00"),BitVec("11")) == Approx(1.0));

	REQUIRE(m->get(BitVec("01"),BitVec("00")) == Approx(0.25));
	REQUIRE(m->get(BitVec("01"),BitVec("01")) == Approx(0.333333333333));
	REQUIRE(m->get(BitVec("01"),BitVec("10")) == Approx(0.5));
	REQUIRE(m->get(BitVec("01"),BitVec("11")) == Approx(0));

	REQUIRE(m->get(BitVec("10"),BitVec("00")) == Approx(0.25));
	REQUIRE(m->get(BitVec("10"),BitVec("01")) == Approx(0.333333333333));
	REQUIRE(m->get(BitVec("10"),BitVec("10")) == Approx(0));
	REQUIRE(m->get(BitVec("10"),BitVec("11")) == Approx(0));

	REQUIRE(m->get(BitVec("11"),BitVec("00")) == Approx(0.25));
	REQUIRE(m->get(BitVec("11"),BitVec("01")) == Approx(0));
	REQUIRE(m->get(BitVec("11"),BitVec("10")) == Approx(0));
	REQUIRE(m->get(BitVec("11"),BitVec("11")) == Approx(0));

}

TEST_CASE("For a single node gbn every wire should have exactly one v_input bitvec and one io bitvec") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "one_node_two_wires.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> picked_vertices({ 0 });

	auto wire_structure = build_wire_structure(gbn, picked_vertices);

	for(auto& w : wire_structure.wires)
	{
		REQUIRE(w.inside_ports.size() == 1);
		REQUIRE(w.io_ports.size() == 1);
	}

}

TEST_CASE("Evaluation for split.gbn should be correct") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "split.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> picked_vertices({ 0, 1, 2 });

	auto m = evaluate_gbn(gbn, picked_vertices);
	
	// check dimensions
	REQUIRE(m->n == 1);
	REQUIRE(m->m == 2);

	// check values
	REQUIRE(m->get(BitVec("00"), BitVec("0")) == Approx(0.2037037037));
	REQUIRE(m->get(BitVec("01"), BitVec("0")) == Approx(0.2407407407));
	REQUIRE(m->get(BitVec("10"), BitVec("0")) == Approx(0.2407407407));
	REQUIRE(m->get(BitVec("11"), BitVec("0")) == Approx(0.3148148148));

	REQUIRE(m->get(BitVec("00"), BitVec("1")) == Approx(0.1805555556));
	REQUIRE(m->get(BitVec("01"), BitVec("1")) == Approx(0.2361111111));
	REQUIRE(m->get(BitVec("10"), BitVec("1")) == Approx(0.2361111111));
	REQUIRE(m->get(BitVec("11"), BitVec("1")) == Approx(0.3472222222));
}

TEST_CASE("Evaluation for four_nodes.gbn should be correct") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> picked_vertices({ 0, 1, 2, 3 });

	auto m = evaluate_gbn(gbn, picked_vertices);
	
	// check dimensions
	REQUIRE(m->n == 2);
	REQUIRE(m->m == 1);

	// check values
	REQUIRE(m->get(BitVec("0"), BitVec("00")) == Approx(0.4074074074));
	REQUIRE(m->get(BitVec("0"), BitVec("01")) == Approx(0.4166666667));
	REQUIRE(m->get(BitVec("0"), BitVec("10")) == Approx(0.4305555556));
	REQUIRE(m->get(BitVec("0"), BitVec("11")) == Approx(0.4375000000));

	REQUIRE(m->get(BitVec("1"), BitVec("00")) == Approx(0.5925925926));
	REQUIRE(m->get(BitVec("1"), BitVec("01")) == Approx(0.5833333333));
	REQUIRE(m->get(BitVec("1"), BitVec("10")) == Approx(0.5694444444));
	REQUIRE(m->get(BitVec("1"), BitVec("11")) == Approx(0.5625000000));
}

TEST_CASE("Evaluation for seven_nodes.gbn should work.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> picked_vertices({ 0, 1, 2, 3, 4, 5, 6 });
	auto m = evaluate_gbn(gbn, picked_vertices);

	REQUIRE(m->n == 4);
	REQUIRE(m->m == 3);
	REQUIRE(is_stochastic(*m) == true);
}
