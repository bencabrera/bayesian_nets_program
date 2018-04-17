#include "../../libs/catch/catch.hpp"

#include "../cnu.h"
#include "../io.h"
#include <fstream>

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

TEST_CASE("Reading in distributions should work properly") {
	std::ifstream f1(TEST_INSTANCE_FOLDER + "read_in_test1.cnu");
	std::ifstream f2(TEST_INSTANCE_FOLDER + "read_in_test1.dist");

	auto cnu = read_cnu(f1, f2);
	REQUIRE(cnu.n == 3);
	REQUIRE(cnu.m.to_string().substr(MAX_PLACES-3) == "110");
	REQUIRE(cnu.transitions.size() == 2);
	REQUIRE(cnu.transitions[0].pre.size() == 1);
	REQUIRE(cnu.transitions[0].post.size() == 1);
	REQUIRE(cnu.transitions[1].pre.size() == 2);
	REQUIRE(cnu.transitions[1].post.size() == 1);
}
