#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/general/subgbn.h"
#include "../../../gbn/modification/splitting.h"
#include "../../../gbn/modification/merging.h"
#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../test_helpers.h"

TEST_CASE("split_3_2_small2.gbn: merge and split 1") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "split_3_2_small2.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { 
		auto v_new = merge_vertices(gbn, {1});
		split_vertex(gbn, v_new);
		return gbn; 
	});
}

TEST_CASE("Node splitting and merging again should yield the same for seven_nodes.gbn.")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { split_vertex(gbn, 2); return gbn; });
}

TEST_CASE("Node splitting for seven_nodes.gbn should work 3.") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { split_vertex(gbn, 3); return gbn; });
}

TEST_CASE("Node splitting for seven_nodes.gbn should work 4.") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { split_vertex(gbn, 4); return gbn; });
}

TEST_CASE("Node splitting for seven_nodes.gbn should work 5.") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { split_vertex(gbn, 5); return gbn; });
}

TEST_CASE("Node splitting for seven_nodes.gbn should work 2.") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { split_vertex(gbn, 0); return gbn; });
}

TEST_CASE("seven_nodes.gbn: Merge and split") 
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { 
		auto v_new = merge_vertices(gbn, {1,2,3});
		split_vertex(gbn, v_new);
		return gbn; 
	});
}

TEST_CASE("seven_nodes.gbn: Merging full and recursively splitting it again should yield the same.")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "seven_nodes.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { 
		auto v_new = merge_vertices(gbn, {0,1,2,3,4,5,6});
		recursively_split_vertex(gbn, v_new);
		return gbn; 
	});
}
