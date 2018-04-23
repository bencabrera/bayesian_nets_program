#include "joint_dist_check.h"

#include <cmath>

bool is_valid_joint_dist(const JointDist& dist)
{
	double sum = 0;
	for(auto p : dist)
		sum += p.second;

	return std::abs(1-sum) < 1e-8;
}
