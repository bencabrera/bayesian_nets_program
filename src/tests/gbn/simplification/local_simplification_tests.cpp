#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/modification/splitting.h"
#include "../../../gbn/simplification/simplification.h"
#include "../../../gbn/simplification/local_simplification.h"
#include "../../../gbn/modification/merging.h"
#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../test_helpers.h"
#include <fstream>

TEST_CASE("OneB replacement (F1) should work correctly.")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "multiple_1_bs.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { simplification(gbn); return gbn; });
}

TEST_CASE("(F2) should work correctly.")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "f2_simplification.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { simplification(gbn);simplification(gbn);simplification(gbn); return gbn; });
}

TEST_CASE("(F3) should work correctly (f3_simplification_1.gbn).")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "f3_simplification_1.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { check_and_apply_F3(gbn,5); return gbn; });
}

TEST_CASE("(F3) should work correctly (f3_simplification_2.gbn).")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "f3_simplification_2.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { check_and_apply_F3(gbn,5); return gbn; });
}

TEST_CASE("(F3) should work correctly (f3_simplification_3.gbn).")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "f3_simplification_3.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { check_and_apply_F3(gbn,6); return gbn; });
}

TEST_CASE("(F3) should work correctly (f3_simplification_4.gbn).")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "f3_simplification_4.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { check_and_apply_F3(gbn,2); return gbn; });
}

TEST_CASE("(F4) should work correctly (f4_simplification_1.gbn).")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "f4_simplification_1.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { check_and_apply_F4(gbn,0); return gbn; }, [](GBN gbn_before, GBN gbn_after) -> void {
		REQUIRE(boost::num_edges(gbn_after.graph) == boost::num_edges(gbn_before.graph)-1);	
	});
}

TEST_CASE("(F4) should work correctly (f4_simplification_2.gbn).")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "f4_simplification_2.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { check_and_apply_F4(gbn,0); return gbn; }, [](GBN gbn_before, GBN gbn_after) -> void {
		REQUIRE(boost::num_edges(gbn_after.graph) == boost::num_edges(gbn_before.graph)-1);	
	});
}

TEST_CASE("(F4) should work correctly (f4_simplification_3.gbn).")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "f4_simplification_3.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { check_and_apply_F4(gbn,0); return gbn; }, [](GBN gbn_before, GBN gbn_after) -> void {
		REQUIRE(boost::num_edges(gbn_after.graph) == boost::num_edges(gbn_before.graph)-1);	
	});
}

TEST_CASE("(F5) should work correctly (f5_simplification_1.gbn).")
{
	auto gbn = read_and_check_gbn(TEST_INSTANCE_FOLDER + "f5_simplification_1.gbn");
	check_evaluates_equal_after_operation(gbn, [](GBN gbn) -> GBN { check_and_apply_F5(gbn,0); return gbn; }, [](GBN gbn_before, GBN gbn_after) -> void {
		REQUIRE(boost::num_edges(gbn_before.graph) == 8);	
		REQUIRE(boost::num_edges(gbn_after.graph) == 5);	
	});
}
