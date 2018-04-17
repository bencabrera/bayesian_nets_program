#pragma once

#include "cnu.h"

void set_op(const std::vector<int> places, bool b, JointDist& dist);
void assert_op(const std::vector<int> places, bool b, JointDist& dist);
void nassert_op(const std::vector<int> places, bool b, JointDist& dist);

void success_op(const std::vector<int> pre_places, const std::vector<int> post_places, JointDist& dist);
void fail_pre_op(const std::vector<int> pre_places, const std::vector<int> post_places, JointDist& dist);
void fail_post_op(const std::vector<int> pre_places, const std::vector<int> post_places, JointDist& dist);
