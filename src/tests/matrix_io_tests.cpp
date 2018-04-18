#include "../../libs/catch/catch.hpp"

#include "../gbn/matrix.h"
#include "../gbn/matrix_io.h"
#include <fstream>

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

TEST_CASE("Reading the three types of matrices should work") {
	read_matrix({ "dynamic 1 1 [1,1;0,0]" });
	read_matrix({ "dynamic 1 1" });

	read_matrix({ "1_{0}" });
	read_matrix({ "1_{1}" });

	read_matrix({ "F_{1,0}" });
	read_matrix({ "F_{1,1}" });
	read_matrix({ "F_{4,1}" });
	read_matrix({ "F_{4,0}" });
}

TEST_CASE("F matrix should be correct 1") {
	auto p_matrix = read_matrix({ "F_{1,1}" });
	REQUIRE(p_matrix->get(BitVec("1"), BitVec("1")) == 0);
	REQUIRE(p_matrix->get(BitVec("0"), BitVec("1")) == 0);
	REQUIRE(p_matrix->get(BitVec("1"), BitVec("0")) == 0);
	REQUIRE(p_matrix->get(BitVec("0"), BitVec("0")) == 1);
}

TEST_CASE("F matrix should be correct 2") {
	auto p_matrix = read_matrix({ "F_{3,1}" });
	REQUIRE(p_matrix->get(BitVec("111"), BitVec("111")) == 0);
	REQUIRE(p_matrix->get(BitVec("101"), BitVec("101")) == 1);
	REQUIRE(p_matrix->get(BitVec("001"), BitVec("001")) == 1);
	REQUIRE(p_matrix->get(BitVec("000"), BitVec("000")) == 1);
	REQUIRE(p_matrix->get(BitVec("001"), BitVec("000")) == 0);
}

TEST_CASE("F matrix should be correct 3") {
	auto p_matrix = read_matrix({ "F_{2,0}" });
	REQUIRE(p_matrix->get(BitVec("11"), BitVec("11")) == 1);
	REQUIRE(p_matrix->get(BitVec("10"), BitVec("10")) == 1);
	REQUIRE(p_matrix->get(BitVec("00"), BitVec("00")) == 0);
	REQUIRE(p_matrix->get(BitVec("01"), BitVec("00")) == 0);
}

TEST_CASE("1_b matrix should be correct 1") {
	auto p_matrix = read_matrix({ "1_{0}" });
	REQUIRE(p_matrix->get(BitVec("1"), BitVec()) == 0);
	REQUIRE(p_matrix->get(BitVec("0"), BitVec()) == 1);
}

TEST_CASE("1_b matrix should be correct 2") {
	auto p_matrix = read_matrix({ "1_{1}" });
	REQUIRE(p_matrix->get(BitVec("1"), BitVec()) == 1);
	REQUIRE(p_matrix->get(BitVec("0"), BitVec()) == 0);
}
