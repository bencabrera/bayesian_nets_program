#include "../../libs/catch/catch.hpp"

#include "../gbn/gbn_io.h"
#include "../gbn/gbn_check.h"
#include <fstream>

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

TEST_CASE("Reading in basic example should work 1") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "basic1.gbn");
	auto gbn = read_gbn(f);
}

TEST_CASE("Reading in basic example and intergrity check should work") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "basic1.gbn");
	auto gbn = read_gbn(f);
	REQUIRE(boost::num_vertices(gbn.graph) == gbn.n_vertices);
	check_gbn_integrity(gbn);
}

TEST_CASE("Non matching node and matrix dimension should be detected") {
	std::ifstream f(TEST_INSTANCE_FOLDER + "wrong_four_nodes.gbn");
	auto gbn = read_gbn(f);
	REQUIRE_THROWS_AS(check_gbn_integrity(gbn), std::logic_error);
}
