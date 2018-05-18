#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/modification/splitting.h"
#include "../../../gbn/simplification/simplification.h"
#include "../../../gbn/simplification/global_simplification.h"
#include "../../../gbn/modification/merging.h"
#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../test_helpers.h"
#include <fstream>

TEST_CASE("seven_nodes.gbn: (StochWithoutOutputs) v_4")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { std::string s; eliminate_stochastic_vertex_without_outputs(gbn,3,s); return gbn; }, [](GBN gbn_before, GBN gbn_after) -> void {
		std::ofstream f1("test_before.dot");
		std::ofstream f2("test_after.dot");
		draw_gbn_graph(f1, gbn_before);
		draw_gbn_graph(f2, gbn_after);
	});
}

TEST_CASE("seven_nodes.gbn: (StochWithoutOutputs) all vertices")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { 
		apply_simplifications_for_each_vertex(gbn, std::function<void(const GBN&, std::string)>(), eliminate_stochastic_vertex_without_outputs);
		return gbn; 
	}
	// , [](GBN gbn_before, GBN gbn_after) -> void {
		// std::ofstream f1("test_before.dot");
		// std::ofstream f2("test_after.dot");
		// draw_gbn_graph(f1, gbn_before);
		// draw_gbn_graph(f2, gbn_after);
	// }
	);
}

TEST_CASE("seven_nodes.gbn: (SwitchSubstochToFront) all vertices")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { 
		apply_simplifications_for_each_vertex(gbn, std::function<void(const GBN&, std::string)>(), switch_substoch_to_front);
		return gbn; 
	}
	// , [](GBN gbn_before, GBN gbn_after) -> void {
		// std::ofstream f1("test_before.dot");
		// std::ofstream f2("test_after.dot");
		// draw_gbn_graph(f1, gbn_before);
		// draw_gbn_graph(f2, gbn_after);
	// }
	);
}

TEST_CASE("substoch2.gbn: (NormalizingSubstochFront) all vertices")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "substoch2.gbn");

	auto p_m_before = evaluate(gbn);
	apply_simplifications_for_each_vertex(gbn, std::function<void(const GBN&, std::string)>(),normalize_substoch_front_vertices_without_inputs);
	auto p_m_after = evaluate(gbn);

	REQUIRE(p_m_before->n == p_m_after->n);
	REQUIRE(p_m_before->m == p_m_after->m);

	double sum = p_m_before->get(BitVec(0), BitVec(0)) + p_m_before->get(BitVec(1), BitVec(0));

	REQUIRE(p_m_after->get(BitVec(0), BitVec(0)) == Approx(p_m_before->get(BitVec(0), BitVec(0))/sum));
	REQUIRE(p_m_after->get(BitVec(1), BitVec(0)) == Approx(p_m_before->get(BitVec(1), BitVec(0))/sum));
}
