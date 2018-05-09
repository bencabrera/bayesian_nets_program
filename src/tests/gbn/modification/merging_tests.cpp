#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include <fstream>
#include "../../../gbn/general/check.h"
#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/subgbn.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../../gbn/modification/merging.h"

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

TEST_CASE("replacing 0") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "line.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::ofstream f1("test_before.dot");
	draw_gbn_graph(f1, gbn);

	auto sub_gbn = SubGBN::make_from_vertices(gbn, {0,1});
	std::ofstream f3("test_sub.dot");
	draw_gbn_graph(f3, sub_gbn.gbn);
	auto m_sub = evaluate(sub_gbn.gbn);
	auto m_before = evaluate(gbn);

	merge_vertices(gbn, {0,1}, "A");

	auto it = std::find_if(boost::vertices(gbn.graph).first, boost::vertices(gbn.graph).second, [&gbn](const Vertex v) {
		return name(v,gbn.graph) == "A";
	});
	REQUIRE(it != boost::vertices(gbn.graph).second);

	check_gbn_integrity(gbn);
	auto m_after = evaluate(gbn);

	std::ofstream f2("test_after.dot");
	draw_gbn_graph(f2, gbn);

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

TEST_CASE("replacing 1") 
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	std::vector<Vertex> vertices_to_replace { 4,6 };

	std::ofstream f1("test_before.dot");
	draw_gbn_graph(f1, gbn);

	auto sub_gbn = SubGBN::make_from_vertices(gbn, vertices_to_replace);
	std::ofstream f3("test_sub.dot");
	draw_gbn_graph(f3, sub_gbn.gbn);
	auto m_sub = evaluate(sub_gbn.gbn);

	auto m_before = evaluate(gbn);
	merge_vertices(gbn, vertices_to_replace, "A");

	auto it = std::find_if(boost::vertices(gbn.graph).first, boost::vertices(gbn.graph).second, [&gbn](const Vertex v) {
		return name(v,gbn.graph) == "A";
	});
	REQUIRE(it != boost::vertices(gbn.graph).second);

	check_gbn_integrity(gbn);
	auto m_after = evaluate(gbn);

	std::ofstream f2("test_after.dot");
	draw_gbn_graph(f2, gbn);

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

	auto m_before = evaluate(gbn);
	merge_vertices(gbn, vertices_to_replace, "A");
	auto m_after = evaluate(gbn);

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

	auto m_before = evaluate(gbn);
	merge_vertices(gbn, vertices_to_replace, "A");
	auto m_after = evaluate(gbn);

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

	auto m_before = evaluate(gbn);
	merge_vertices(gbn, vertices_to_replace, "A");
	auto m_after = evaluate(gbn);

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
