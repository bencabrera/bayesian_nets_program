#pragma once

#include "cnu.h"

void set_op(const std::vector<int> places, bool b, JointDist& dist);
void assert_op(const std::vector<int> places, bool b, JointDist& dist);
void nassert_op(const std::vector<int> places, bool b, JointDist& dist);
