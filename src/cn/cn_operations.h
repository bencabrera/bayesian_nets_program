#pragma once

#include "cn.h"

bool check_pre_condition(const Transition& t, const Marking& m);
bool check_post_condition(const Transition& t, const Marking& m);
bool fire_transition(const Transition& t, Marking& m);
