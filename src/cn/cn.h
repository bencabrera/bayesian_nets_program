#pragma once

#include "../bitvec.h"

#include <unordered_map>
#include <vector>

using Marking = std::vector<bool>;

struct Transition {
	std::vector<std::size_t> pre;
	std::vector<std::size_t> post;
};

struct CN {
	std::size_t n; // number of places
	Marking m; // (initial) marking
	std::vector<Transition> transitions; 
};

