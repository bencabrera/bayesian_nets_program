#pragma once

#include "../joint_dist/joint_dist.h"
#include <vector>

void set_op(const std::vector<std::size_t> places, bool b, JointDist& dist);
void assert_op(const std::vector<std::size_t> places, bool b, JointDist& dist);
void nassert_op(const std::vector<std::size_t> places, bool b, JointDist& dist);
