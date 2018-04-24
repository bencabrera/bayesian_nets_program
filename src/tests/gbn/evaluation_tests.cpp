#include "../../../libs/catch/catch.hpp"

#include "../../gbn/general/evaluation.h"
#include "../../gbn/matrix/matrix_io.h"
#include <fstream>
#include "../../gbn/general/check.h"
#include "../../gbn/general/gbn_io.h"
#include "../../gbn/modification/merging.h"

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

TEST_CASE("BEN") {
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

TEST_CASE("Flipping a wire should turn the right bits on in vertices") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vec = { 0 };
	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec, vertex_set_input_outputs);

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

	std::vector<Vertex> vec = { 0,1,2 };
	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec, vertex_set_input_outputs);

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

	std::vector<Vertex> vec = { 0,1,2 };
	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec, vertex_set_input_outputs);
	std::vector<Wire> tmp;
	std::copy_if(wire_structure.wires.begin(), wire_structure.wires.end(), std::back_inserter(tmp), [&wire_structure] (const Wire& w) {
		return w.inside_ports.size() == 3;
	});
	REQUIRE(tmp.size() == 1);
	auto& w = tmp[0];
	auto affected_vertices = flip_wire(w);

	auto g = gbn.graph;

	double product = 1;
	for(auto v : vec)
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

	auto [p_m, vertex_set_input_outputs] = evaluate_vertices(gbn, picked_vertices);

	REQUIRE(p_m->get(BitVec("0"),BitVec("0")) == Approx(0.333333333333));
	REQUIRE(p_m->get(BitVec("1"),BitVec("0")) == Approx(0.666666666666));
	REQUIRE(p_m->get(BitVec("0"),BitVec("1")) == Approx(0.75));
	REQUIRE(p_m->get(BitVec("1"),BitVec("1")) == Approx(0.25));
}

TEST_CASE("Evaluation of single node should just give that matrix of node 2") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "one_node_two_wires.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vec({ 0 });

	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec, vertex_set_input_outputs);
	REQUIRE(wire_structure.wires.size() == 4);
	for(auto w : wire_structure.wires)
	{
		REQUIRE(w.inside_ports.size() == 1);
		REQUIRE(w.io_ports.size() == 1);
	}

	auto [p_m, bla] = evaluate_vertices(gbn, vec);
	REQUIRE(vertex_set_input_outputs.input_ports.size() == 2);
	REQUIRE(vertex_set_input_outputs.output_ports.size() == 2);

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
	std::ifstream f(TEST_INSTANCE_FOLDER + "one_node_two_wires.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vec = { 0 };
	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec, vertex_set_input_outputs);

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
	auto [p_m, vertex_set_input_outputs] = evaluate_vertices(gbn, picked_vertices);
	
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
	std::ifstream f(TEST_INSTANCE_FOLDER + "four_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> picked_vertices({ 0, 1, 2, 3 });

	auto [p_m,tmp] = evaluate_vertices(gbn, picked_vertices);
	
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
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vec({ 0, 1, 2, 3, 4, 5, 6 });
	auto vertex_set_input_outputs = build_inputs_outputs_for_vertices(gbn, vec);
	auto wire_structure = build_wire_structure_for_vertices(gbn, vec, vertex_set_input_outputs);
	auto wire_structure2 = build_wire_structure_for_whole_gbn(gbn);
	auto [p_m,tmp] = evaluate_vertices(gbn, vec);

	REQUIRE(wire_structure.wires.size() == wire_structure2.wires.size());
	REQUIRE(wire_structure.wires.size() == 13);

	// print_matrix(std::cout, *p_m);

	REQUIRE(p_m->n == 4);
	REQUIRE(p_m->m == 3);
	REQUIRE(is_stochastic(*p_m) == true);
}


TEST_CASE("seven_nodes.gbn: After replacing vertices 1,2 by matrix it should evaluate to the same as before.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vertices_to_replace { 1,2 };

	auto m_before = evaluate_gbn(gbn);
	merge_vertices(gbn, vertices_to_replace, "A");
	auto m_after = evaluate_gbn(gbn);

	// compare dimensions
	REQUIRE(m_before->n == m_after->n);
	REQUIRE(m_before->m == m_after->m);

	unsigned long long i_max_row = 1;
	unsigned long long i_max_col = 1;
	i_max_col = i_max_col << m_before->n;
	i_max_row = i_max_row << m_before->m;

	for(unsigned long long i_row = 0; i_row < i_max_row; i_row++)
		for(unsigned long long i_col = 0; i_col < i_max_col; i_col++)
			REQUIRE(m_before->get(i_row, i_col) == Approx(m_after->get(i_row, i_col)));
}

TEST_CASE("seven_nodes.gbn: After replacing vertices 0,1,2 by matrix it should evaluate to the same as before.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vertices_to_replace { 0,1,2 };

	auto m_before = evaluate_gbn(gbn);
	merge_vertices(gbn, vertices_to_replace, "A");
	auto m_after = evaluate_gbn(gbn);

	// compare dimensions
	REQUIRE(m_before->n == m_after->n);
	REQUIRE(m_before->m == m_after->m);

	unsigned long long i_max_row = 1;
	unsigned long long i_max_col = 1;
	i_max_col = i_max_col << m_before->n;
	i_max_row = i_max_row << m_before->m;

	for(unsigned long long i_row = 0; i_row < i_max_row; i_row++)
		for(unsigned long long i_col = 0; i_col < i_max_col; i_col++)
			REQUIRE(m_before->get(i_row, i_col) == Approx(m_after->get(i_row, i_col)));
}

TEST_CASE("seven_nodes.gbn: After replacing vertices 0,1,3 by matrix it should evaluate to the same as before.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vertices_to_replace { 0,1,3 };

	auto m_before = evaluate_gbn(gbn);
	merge_vertices(gbn, vertices_to_replace, "A");
	auto m_after = evaluate_gbn(gbn);

	// compare dimensions
	REQUIRE(m_before->n == m_after->n);
	REQUIRE(m_before->m == m_after->m);

	unsigned long long i_max_row = 1;
	unsigned long long i_max_col = 1;
	i_max_col = i_max_col << m_before->n;
	i_max_row = i_max_row << m_before->m;

	for(unsigned long long i_row = 0; i_row < i_max_row; i_row++)
		for(unsigned long long i_col = 0; i_col < i_max_col; i_col++)
			REQUIRE(m_before->get(i_row, i_col) == Approx(m_after->get(i_row, i_col)));
}

TEST_CASE("seven_nodes.gbn: After replacing vertices 5,6 by matrix it should evaluate to the same as before.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vertices_to_replace { 5,6 };

	auto m_before = evaluate_gbn(gbn);
	merge_vertices(gbn, vertices_to_replace, "A");
	auto m_after = evaluate_gbn(gbn);

	// compare dimensions
	REQUIRE(m_before->n == m_after->n);
	REQUIRE(m_before->m == m_after->m);

	unsigned long long i_max_row = 1;
	unsigned long long i_max_col = 1;
	i_max_col = i_max_col << m_before->n;
	i_max_row = i_max_row << m_before->m;

	for(unsigned long long i_row = 0; i_row < i_max_row; i_row++)
		for(unsigned long long i_col = 0; i_col < i_max_col; i_col++)
			REQUIRE(m_before->get(i_row, i_col) == Approx(m_after->get(i_row, i_col)));
}


TEST_CASE("Evaluating id_1.gbn should yield identity 1 -> 1") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "id_1.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto m = evaluate_gbn(gbn);

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
	std::ifstream f(TEST_INSTANCE_FOLDER + "id_2.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto m = evaluate_gbn(gbn);

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
