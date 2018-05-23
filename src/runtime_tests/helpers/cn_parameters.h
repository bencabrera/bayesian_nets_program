#pragma once

#include "random_transition_helper.h"
#include "../../../libs/cxxopts/include/cxxopts.hpp"

struct CN_Parameters {
	std::size_t N_MIN_PLACES;
	std::size_t N_MAX_PLACES;
	std::size_t N_RUNS;
	std::size_t N_TRANSITIONS_PER_RUN;
	double PERCENT_TRANSITIONS;
	double PERCENT_MIN_TOKENS;
	double PERCENT_MAX_TOKENS;

	std::size_t N_MIN_PRE_PLACES;
	std::size_t N_MAX_PRE_PLACES;
	std::size_t N_MIN_POST_PLACES;
	std::size_t N_MAX_POST_PLACES;

	RandomTransitionHelper::Type RAND_TRANSITION_TYPE;
	double P_SUCCESS;
};

CN_Parameters read_cn_params(const cxxopts::ParseResult& params);
void write_cn_params(std::ostream& ostr, const CN_Parameters& cn_params);
