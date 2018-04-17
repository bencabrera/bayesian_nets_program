#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "../libs/catch/catch.hpp"

#include "cnu.h"
#include "io.h"
#include "operations.h"
#include <fstream>

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

void compare_dists(const JointDist& dist1, const JointDist& dist2)
{
	for(const auto& [m, p] : dist1)
		REQUIRE(p == Approx(dist2.at(m)));
}

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

TEST_CASE("Set operation should work correctly 1") {
	std::ifstream f1(TEST_INSTANCE_FOLDER + "operations_initial1.dist");
	std::ifstream f2(TEST_INSTANCE_FOLDER + "operations_set1_1.dist");

	auto initial_dist = read_joint_dist(f1); 
	auto expected_dist = read_joint_dist(f2); 

	auto updated_dist = initial_dist;
	set_op({0}, 1, updated_dist);

	compare_dists(updated_dist, expected_dist);
}

TEST_CASE("Set operation should work correctly 2") {
	std::ifstream f1(TEST_INSTANCE_FOLDER + "operations_initial1.dist");
	std::ifstream f2(TEST_INSTANCE_FOLDER + "operations_set1_2.dist");

	auto initial_dist = read_joint_dist(f1); 
	auto expected_dist = read_joint_dist(f2); 

	auto updated_dist = initial_dist;
	set_op({0,2}, 1, updated_dist);

	compare_dists(updated_dist, expected_dist);
}
