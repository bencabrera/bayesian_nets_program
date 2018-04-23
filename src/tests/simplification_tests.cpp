#include "../../libs/catch/catch.hpp"

#include "../gbn/general/gbn_io.h"
#include "../gbn/general/check.h"
#include "../gbn/modification/splitting.h"
#include "../gbn/modification/simplification.h"
#include "../gbn/modification/merging.h"
#include "../gbn/general/evaluation.h"
#include <fstream>
#include "../gbn/matrix/matrix_io.h"

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

TEST_CASE("OneB replacement (F1) should work correctly.")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "multiple_1_bs.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;

	gbn_simplification(gbn);

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("(F2) should work correctly.")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "f2_simplification.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;
	auto n_vertices_before = gbn.n_vertices;

	gbn_simplification(gbn);
	gbn_simplification(gbn);

	auto n_vertices_after = gbn.n_vertices;

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	REQUIRE(n_vertices_after == 4);

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("(F3) should work correctly (f3_simplification_1.gbn).")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "f3_simplification_1.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;
	auto n_vertices_before = gbn.n_vertices;

	gbn_simplification(gbn);
	check_gbn_integrity(gbn);

	auto n_vertices_after = gbn.n_vertices;

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("(F3) should work correctly (f3_simplification_2.gbn).")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "f3_simplification_2.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;
	auto n_vertices_before = gbn.n_vertices;

	gbn_simplification(gbn);
	check_gbn_integrity(gbn);

	auto n_vertices_after = gbn.n_vertices;

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("(F3) should work correctly (f3_simplification_3.gbn).")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "f3_simplification_3.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;
	auto n_vertices_before = gbn.n_vertices;

	gbn_simplification(gbn);
	check_gbn_integrity(gbn);

	auto n_vertices_after = gbn.n_vertices;

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("(F3) should work correctly (f3_simplification_3.gbn) 2.")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "f3_simplification_3.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;
	auto n_vertices_before = gbn.n_vertices;

	gbn_simplification(gbn);
	gbn_simplification(gbn);
	check_gbn_integrity(gbn);

	auto n_vertices_after = gbn.n_vertices;

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("(F4) should work correctly (f4_simplification_1.gbn).")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "f4_simplification_1.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;
	auto n_vertices_before = gbn.n_vertices;

	gbn_simplification(gbn);
	gbn_simplification(gbn);
	check_gbn_integrity(gbn);

	auto n_vertices_after = gbn.n_vertices;

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("(F4) should work correctly (f4_simplification_2.gbn).")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "f4_simplification_2.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;
	auto n_vertices_before = gbn.n_vertices;

	gbn_simplification(gbn);
	gbn_simplification(gbn);
	check_gbn_integrity(gbn);

	auto n_vertices_after = gbn.n_vertices;

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

TEST_CASE("(F4) should work correctly (f4_simplification_3.gbn).")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "f4_simplification_3.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;
	auto n_vertices_before = gbn.n_vertices;

	gbn_simplification(gbn);
	gbn_simplification(gbn);
	check_gbn_integrity(gbn);

	auto n_vertices_after = gbn.n_vertices;

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}

// TEST_CASE("(F5) should work correctly (f5_simplification_1.gbn).")
// {
	// std::ifstream f(TEST_INSTANCE_FOLDER + "f5_simplification_1.gbn");
	// auto gbn = read_gbn(f);
	// check_gbn_integrity(gbn);

	// auto p_m_before = evaluate_gbn(gbn);
	// auto& m_before = *p_m_before;
	// auto n_vertices_before = gbn.n_vertices;

	// gbn_simplification(gbn);
	// gbn_simplification(gbn);
	// check_gbn_integrity(gbn);

	// auto n_vertices_after = gbn.n_vertices;

	// auto p_m_after = evaluate_gbn(gbn);
	// auto& m_after = *p_m_after;

	// test_matrices_equal(m_before, m_after);
// }
