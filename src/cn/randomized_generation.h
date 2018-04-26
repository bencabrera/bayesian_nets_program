#pragma once

#include "cn.h"

CN randomize_cn(
	std::size_t n_places, 
	std::size_t n_transitions, 
	std::size_t n_min_tokens,
	std::size_t n_max_tokens,
	std::size_t n_min_pre_places, 
	std::size_t n_max_pre_places,
	std::size_t n_min_post_places, 
	std::size_t n_max_post_places
);
