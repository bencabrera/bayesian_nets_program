#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/evaluation/probability_bookkeeper.h"

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

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

