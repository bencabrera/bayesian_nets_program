#pragma once 

#include <bitset>
#include <memory>

constexpr int MAX_PLACES = 64;

using BitVec = std::bitset<MAX_PLACES>;
using BitVecPtr = std::shared_ptr<BitVec>;

std::ostream& print_bitvec(std::ostream& ostr, const BitVec& m, const int n = MAX_PLACES);

