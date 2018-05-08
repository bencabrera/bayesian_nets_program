#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include <fstream>
#include "../../../gbn/general/check.h"
#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/modification/merging.h"

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

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
