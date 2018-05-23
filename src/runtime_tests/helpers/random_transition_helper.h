#pragma once

#include "../../cn/cn.h"
#include "../../cn/randomized_generation.h"
#include "../../cn/cn_io.h"
#include "../../cn/cn_operations.h"

#include <algorithm>

struct RandomTransitionHelper 
{
	const CN& cn;
	std::vector<std::size_t> i_transitions;
	enum Type {
		UNIFORM,
		FORCED
	} type;
	double p_success;

	std::uniform_int_distribution<std::size_t> rand_transition;
	std::uniform_real_distribution<double> uniform_0_1;

	RandomTransitionHelper(const CN& cn, Type type, double p_success);

	std::size_t next(std::mt19937& mt);

};

