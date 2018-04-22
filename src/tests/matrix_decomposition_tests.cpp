#include "../../libs/catch/catch.hpp"

#include "../gbn/matrix_io.h"
#include "../gbn/matrix_decomposition.h"
#include <fstream>

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

TEST_CASE("Matrix decomposition should work for small 2 -> 2 example") {
	auto p_m = read_matrix({"dynamic 2 2 [1/4,1/3,1/2,1;1/4,1/3,1/2,0;1/4,1/3,0,0;1/4,0,0,0]"});
	auto& m = *p_m;

	auto [p_m_front, p_m_back] = matrix_decomposition(p_m);
	
	auto& m_front = *p_m_front;	
	auto& m_back = *p_m_back;	

	unsigned long long x_max = 1;
	unsigned long long z_max = 1;
	x_max = x_max << (m.m-1);
	z_max = z_max << (m.n);
	for(Index x = 0; x < x_max; x++)
		for(Index z = 0; z < z_max; z++)
			for(Index y = 0; y < 2; y++)
			{
				BitVec x_bitvec(x), y_bitvec(y), z_bitvec(z);
				auto xz = bitvec_append(x,z,m.m);
				auto xy = bitvec_append(x,y,1);

				REQUIRE(m_front.get(x_bitvec, z_bitvec) * m_back.get(y, xz) == Approx(m.get(xy,z_bitvec)));
			}
}

TEST_CASE("bitvec_append should work properly.") {
	REQUIRE(bitvec_append(BitVec("00"), BitVec("00"), 2).to_ulong() == 0);
	REQUIRE(bitvec_append(BitVec("00"), BitVec("01"), 2).to_ulong() == 1);
	REQUIRE(bitvec_append(BitVec("00"), BitVec("10"), 2).to_ulong() == 2);
	REQUIRE(bitvec_append(BitVec("00"), BitVec("11"), 2).to_ulong() == 3);
	REQUIRE(bitvec_append(BitVec("01"), BitVec("00"), 2).to_ulong() == 4);
	REQUIRE(bitvec_append(BitVec("01"), BitVec("01"), 2).to_ulong() == 5);
	REQUIRE(bitvec_append(BitVec("01"), BitVec("10"), 2).to_ulong() == 6);
	REQUIRE(bitvec_append(BitVec("01"), BitVec("11"), 2).to_ulong() == 7);
	REQUIRE(bitvec_append(BitVec("10"), BitVec("00"), 2).to_ulong() == 8);
}
