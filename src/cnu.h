#pragma once

#include <bitset>
#include <unordered_map>
#include <vector>

constexpr int MAX_PLACES = 64;

using Marking = std::bitset<MAX_PLACES>;
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

