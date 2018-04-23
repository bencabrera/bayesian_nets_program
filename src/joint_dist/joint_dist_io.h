#pragma once

#include "joint_dist.h"

JointDist read_joint_dist(std::istream& istr);
std::ostream& print_dist(std::ostream& ostr, const JointDist& dist, const int n);
