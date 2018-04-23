#include "../../libs/catch/catch.hpp"

#include "../gbn/gbn_io.h"
#include "../gbn/gbn_check.h"
#include "../gbn/node_splitting.h"
#include "../gbn/node_elimination.h"
#include "../gbn/gbn_evaluation.h"
#include <fstream>
#include "../gbn/matrix_io.h"

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

namespace {
	bool test_matrices_equal(const Matrix& m1, const Matrix& m2)
	{
		REQUIRE(m1.n == m2.n);
		REQUIRE(m1.m == m2.m);

		unsigned long long to_max = 1;
		unsigned long long from_max = 1;
		to_max = to_max << m1.m;
		from_max = from_max << m1.n;
		for(Index to = 0; to < to_max; to++)
			for(Index from = 0; from < from_max; from++)
				REQUIRE(m1.get(to, from) == Approx(m2.get(to, from)));
	}
}

TEST_CASE("Node splitting and merging again should yield the same for seven_nodes.gbn.")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn, {2});
	auto& m_before = *p_m_before;

	std::ofstream out_file1("before.dot");
	draw_gbn_graph(out_file1, gbn);

	auto [v_front, v_back] = node_splitting(gbn, 2);
	std::ofstream out_file2("after.dot");
	draw_gbn_graph(out_file2, gbn);

	auto p_m_after = evaluate_gbn(gbn, {v_front, v_back});
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("Node splitting for seven_nodes.gbn should work 1.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;

	node_splitting(gbn, 2);

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("Node splitting for seven_nodes.gbn should work 3.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;

	node_splitting(gbn, 3);

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("Node splitting for seven_nodes.gbn should work 4.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;

	node_splitting(gbn, 4);

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("Node splitting for seven_nodes.gbn should work 5.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;

	node_splitting(gbn, 5);

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("Node splitting for seven_nodes.gbn should work 2.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;

	node_splitting(gbn, 0);

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("Node splitting for first_v_of_seven_nodes.gbn should work.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "first_v_of_seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;

	std::ofstream out_file1("before.dot");
	draw_gbn_graph(out_file1, gbn);

	node_splitting(gbn, 0);
	check_gbn_integrity(gbn);

	std::ofstream out_file2("after.dot");
	draw_gbn_graph(out_file2, gbn);

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}



TEST_CASE("Node splitting and merging again of third_v_of_seven_nodes.gbn")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "third_v_of_seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn, {0});
	auto& m_before = *p_m_before;

	std::ofstream out_file1("before.dot");
	draw_gbn_graph(out_file1, gbn);

	auto [v_front,v_back] = node_splitting(gbn, 0);
	check_gbn_integrity(gbn);

	std::ofstream out_file2("after.dot");
	draw_gbn_graph(out_file2, gbn);

	auto p_m_after = evaluate_gbn(gbn, {v_front, v_back});
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("Replace and split test.") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;

	auto v_new = replace_nodes_by_matrix(gbn, {1,2,3});
	auto [v_front, v_back] = node_splitting(gbn, v_new);
	node_splitting(gbn, v_front);
	check_gbn_integrity(gbn);

	std::ofstream out_file2("after.dot");
	draw_gbn_graph(out_file2, gbn);

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}
