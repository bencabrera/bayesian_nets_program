#include "../../libs/catch/catch.hpp"

#include "../gbn/gbn_io.h"
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
	check_gbn_integrity(gbn);
}
