#pragma once

#include "../gbn/general/gbn.h"
#include "../joint_dist/joint_dist.h"
#include "../cn/cn.h"
#include <functional>

void fire_transition_on_joint_dist(CN& cn, JointDist& dist, std::size_t i_transition, std::function<void(std::string,std::string)> status_callback = std::function<void(std::string,std::string)>());
void fire_transition_on_gbn(CN& cn, GBN& gbn, std::size_t i_transition, std::function<void(std::string,std::string)> status_callback = std::function<void(std::string,std::string)>());
