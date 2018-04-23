#pragma once

#include "../bitvec.h"

#include <unordered_map>
#include <vector>

using Marking = BitVec;

struct Transition {
	std::vector<int> pre;
	std::vector<int> post;
};

struct CN {
	int n; // number of places
	Marking m; // (initial) marking
	std::vector<Transition> transitions; 
};

