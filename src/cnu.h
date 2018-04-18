#pragma once

#include "bitvec.h"

#include <unordered_map>
#include <vector>

using Marking = BitVec;
using JointDist = std::unordered_map<Marking, double>;

struct Transition {
	std::vector<int> pre;
	std::vector<int> post;
};

struct CNU {
	int n; // number of places
	Marking m; // (initial) marking
	std::vector<Transition> transitions; 
	JointDist dist; // (initial) dist
};

