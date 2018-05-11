#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/subgbn.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../../gbn/modification/merging.h"
#include "../../test_helpers.h"

TEST_CASE("line.gbn: Merge 0,1") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "line.gbn");

	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { merge_vertices(gbn, {0,1}, "A"); return gbn; }, [](GBN,GBN gbn_after) -> void {
		auto it = std::find_if(boost::vertices(gbn_after.graph).first, boost::vertices(gbn_after.graph).second, [&gbn_after](const Vertex v) {
			return name(v,gbn_after.graph) == "A";
		});
		REQUIRE(it != boost::vertices(gbn_after.graph).second);
	});
}

TEST_CASE("seven_nodes.gbn: Merge 4,6") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { merge_vertices(gbn, {4,6}, "A"); return gbn; });
}

TEST_CASE("seven_nodes.gbn: Merge 0,1,2") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { merge_vertices(gbn, {0,1,2}, "A"); return gbn; });
}

TEST_CASE("seven_nodes.gbn: Merge 0,1,3") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { merge_vertices(gbn, {0,1,3}, "A"); return gbn; });
}

TEST_CASE("seven_nodes.gbn: Merge 5,6") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { merge_vertices(gbn, {5,6}, "A"); return gbn; });
}
