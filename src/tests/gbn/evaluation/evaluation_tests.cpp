#include "../../../../libs/catch/catch.hpp"

#include <fstream>

#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/subgbn.h"
#include "../../../gbn/modification/merging.h"
#include "../../test_helpers.h"

TEST_CASE("Evaluation of single node should just give that matrix of node 1") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "one_node.gbn");

	auto sub_gbn = SubGBN::make_from_vertices(gbn, { 0 });
	auto p_m = evaluate(sub_gbn.gbn);

	REQUIRE(p_m->get(BitVec("0"),BitVec("0")) == Approx(0.333333333333));
	REQUIRE(p_m->get(BitVec("1"),BitVec("0")) == Approx(0.666666666666));
	REQUIRE(p_m->get(BitVec("0"),BitVec("1")) == Approx(0.75));
	REQUIRE(p_m->get(BitVec("1"),BitVec("1")) == Approx(0.25));
}

TEST_CASE("Evaluation of single node should just give that matrix of node 2") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "one_node_two_wires.gbn");

	std::vector<Vertex> vec({ 0 });

	auto sub_gbn = SubGBN::make_from_vertices(gbn, { 0 });
	auto wire_structure = build_wire_structure(sub_gbn.gbn);
	REQUIRE(wire_structure.wires.size() == 4);
	for(auto w : wire_structure.wires)
	{
		REQUIRE(w.inside_ports.size() == 1);
		REQUIRE(w.io_ports.size() == 1);
	}
	REQUIRE(sub_gbn.gbn.n == 2);
	REQUIRE(sub_gbn.gbn.m == 2);

	auto p_m = evaluate(gbn);

	REQUIRE(p_m->get(BitVec("00"),BitVec("00")) == Approx(0.25));
	REQUIRE(p_m->get(BitVec("00"),BitVec("01")) == Approx(0.333333333333));
	REQUIRE(p_m->get(BitVec("00"),BitVec("10")) == Approx(0.5));
	REQUIRE(p_m->get(BitVec("00"),BitVec("11")) == Approx(1.0));

	REQUIRE(p_m->get(BitVec("01"),BitVec("00")) == Approx(0.25));
	REQUIRE(p_m->get(BitVec("01"),BitVec("01")) == Approx(0.333333333333));
	REQUIRE(p_m->get(BitVec("01"),BitVec("10")) == Approx(0.5));
	REQUIRE(p_m->get(BitVec("01"),BitVec("11")) == Approx(0));

	REQUIRE(p_m->get(BitVec("10"),BitVec("00")) == Approx(0.25));
	REQUIRE(p_m->get(BitVec("10"),BitVec("01")) == Approx(0.333333333333));
	REQUIRE(p_m->get(BitVec("10"),BitVec("10")) == Approx(0));
	REQUIRE(p_m->get(BitVec("10"),BitVec("11")) == Approx(0));

	REQUIRE(p_m->get(BitVec("11"),BitVec("00")) == Approx(0.25));
	REQUIRE(p_m->get(BitVec("11"),BitVec("01")) == Approx(0));
	REQUIRE(p_m->get(BitVec("11"),BitVec("10")) == Approx(0));
	REQUIRE(p_m->get(BitVec("11"),BitVec("11")) == Approx(0));

}

TEST_CASE("For a single node gbn every wire should have exactly one v_input bitvec and one io bitvec") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "one_node_two_wires.gbn");

	auto sub_gbn = SubGBN::make_from_vertices(gbn, { 0 });
	auto wire_structure = build_wire_structure(sub_gbn.gbn);

	for(auto& w : wire_structure.wires)
	{
		REQUIRE(w.inside_ports.size() == 1);
		REQUIRE(w.io_ports.size() == 1);
	}

}

TEST_CASE("Evaluation for split.gbn should be correct") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "split.gbn");

	auto sub_gbn = SubGBN::make_from_vertices(gbn, { 0, 1, 2 });
	auto wire_structure = build_wire_structure(sub_gbn.gbn);

	auto p_m = evaluate(sub_gbn.gbn);
	
	// check dimensions
	REQUIRE(p_m->n == 1);
	REQUIRE(p_m->m == 2);

	// checkp_ values
	REQUIRE(p_m->get(BitVec("00"), BitVec("0")) == Approx(0.2037037037));
	REQUIRE(p_m->get(BitVec("01"), BitVec("0")) == Approx(0.2407407407));
	REQUIRE(p_m->get(BitVec("10"), BitVec("0")) == Approx(0.2407407407));
	REQUIRE(p_m->get(BitVec("11"), BitVec("0")) == Approx(0.3148148148));

	REQUIRE(p_m->get(BitVec("00"), BitVec("1")) == Approx(0.1805555556));
	REQUIRE(p_m->get(BitVec("01"), BitVec("1")) == Approx(0.2361111111));
	REQUIRE(p_m->get(BitVec("10"), BitVec("1")) == Approx(0.2361111111));
	REQUIRE(p_m->get(BitVec("11"), BitVec("1")) == Approx(0.3472222222));
}

TEST_CASE("Evaluation for four_nodes.gbn should be correct") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "four_nodes.gbn");

	auto sub_gbn = SubGBN::make_from_vertices(gbn, { 0, 1, 2, 3 });
	auto wire_structure = build_wire_structure(sub_gbn.gbn);
	auto p_m = evaluate(sub_gbn.gbn);
	
	// check dimensions
	REQUIRE(p_m->n == 2);
	REQUIRE(p_m->m == 1);

	// checkp_ values
	REQUIRE(p_m->get(BitVec("0"), BitVec("00")) == Approx(0.4074074074));
	REQUIRE(p_m->get(BitVec("0"), BitVec("01")) == Approx(0.4166666667));
	REQUIRE(p_m->get(BitVec("0"), BitVec("10")) == Approx(0.4305555556));
	REQUIRE(p_m->get(BitVec("0"), BitVec("11")) == Approx(0.4375000000));

	REQUIRE(p_m->get(BitVec("1"), BitVec("00")) == Approx(0.5925925926));
	REQUIRE(p_m->get(BitVec("1"), BitVec("01")) == Approx(0.5833333333));
	REQUIRE(p_m->get(BitVec("1"), BitVec("10")) == Approx(0.5694444444));
	REQUIRE(p_m->get(BitVec("1"), BitVec("11")) == Approx(0.5625000000));
}

TEST_CASE("Evaluation for seven_nodes.gbn should work.") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");

	auto sub_gbn = SubGBN::make_from_vertices(gbn, { 0, 1, 2, 3, 4, 5, 6 });
	auto wire_structure = build_wire_structure(sub_gbn.gbn);
	auto wire_structure2 = build_wire_structure(gbn);
	auto p_m = evaluate(sub_gbn.gbn);

	REQUIRE(wire_structure.wires.size() == wire_structure2.wires.size());
	REQUIRE(wire_structure.wires.size() == 13);

	REQUIRE(p_m->n == 4);
	REQUIRE(p_m->m == 3);
	REQUIRE(is_stochastic(*p_m) == true);
}

TEST_CASE("Evaluating id_1.gbn should yield identity 1 -> 1") {
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "id_1.gbn");

	auto m = evaluate(gbn);

	unsigned long long i_max_row = 1;
	unsigned long long i_max_col = 1;
	i_max_col = i_max_col << m->n;
	i_max_row = i_max_row << m->m;

	for(unsigned long long i_row = 0; i_row < i_max_row; i_row++)
		for(unsigned long long i_col = 0; i_col < i_max_col; i_col++)
		{
			if(i_row == i_col)
				REQUIRE(m->get(i_row, i_col) == 1);
			else
				REQUIRE(m->get(i_row, i_col) == 0);
		}
}

TEST_CASE("Evaluating id_1.gbn should yield identity 2 -> 2") {
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "id_2.gbn");

	auto m = evaluate(gbn);

	unsigned long long i_max_row = 1;
	unsigned long long i_max_col = 1;
	i_max_col = i_max_col << m->n;
	i_max_row = i_max_row << m->m;

	for(unsigned long long i_row = 0; i_row < i_max_row; i_row++)
		for(unsigned long long i_col = 0; i_col < i_max_col; i_col++)
		{
			if(i_row == i_col)
				REQUIRE(m->get(i_row, i_col) == 1);
			else
				REQUIRE(m->get(i_row, i_col) == 0);
		}
}
