#pragma once

#include "joint_dist.h"

JointDist read_joint_dist(std::istream& istr);
void print_dist(std::ostream& ostr, const JointDist& dist);
