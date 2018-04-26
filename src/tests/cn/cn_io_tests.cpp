#include "../../../libs/catch/catch.hpp"

#include "../../cn/cn.h"
#include "../../cn/cn_io.h"
#include "../../helpers.hpp"
#include <fstream>

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

TEST_CASE("Reading in CN should work properly") {
	std::ifstream f1(TEST_INSTANCE_FOLDER + "read_in_test1.cnu");

	auto cn = read_cn(f1);
	REQUIRE(cn.n == 3);
	REQUIRE(to_string(cn.m) == "011");
	REQUIRE(cn.transitions.size() == 2);
	REQUIRE(cn.transitions[0].pre.size() == 1);
	REQUIRE(cn.transitions[0].post.size() == 1);
	REQUIRE(cn.transitions[1].pre.size() == 2);
	REQUIRE(cn.transitions[1].post.size() == 1);
}

TEST_CASE("Reading in CN and then exporting and reading again should work properly") {
	std::ifstream f1(TEST_INSTANCE_FOLDER + "read_in_test1.cnu");
	auto cn = read_cn(f1);

	std::stringstream ss;
	export_cn(ss,cn);

	ss.seekg(std::ios::beg);
	auto cn2 = read_cn(ss);

	REQUIRE(cn.n == cn2.n);
	REQUIRE(cn.m.size() == cn2.m.size());
	REQUIRE(cn.transitions.size() == cn2.transitions.size());
}
