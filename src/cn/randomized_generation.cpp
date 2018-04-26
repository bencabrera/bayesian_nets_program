#include "randomized_generation.h"

#include <random>
#include <algorithm>

CN randomize_cn(
	std::size_t n_places, 
	std::size_t n_transitions, 
	std::size_t n_min_tokens,
	std::size_t n_max_tokens,
	std::size_t n_min_pre_places, 
	std::size_t n_max_pre_places,
	std::size_t n_min_post_places, 
	std::size_t n_max_post_places
)
{
	CN cn;
	cn.n = n_places;

    std::random_device rd;  
    std::mt19937 mt(rd()); 

	std::vector<std::size_t> places(n_places);
	std::iota(places.begin(), places.end(), 0);

    std::uniform_int_distribution<std::size_t> rand_tokens(n_min_tokens, n_max_tokens);
    std::uniform_int_distribution<std::size_t> rand_pre_places(n_min_pre_places, n_max_pre_places);
    std::uniform_int_distribution<std::size_t> rand_post_places(n_min_post_places, n_max_post_places);

	for(std::size_t i_transition = 0; i_transition < n_transitions; i_transition++)
	{
		Transition t;

		std::shuffle(places.begin(), places.end(),mt);
		auto n_pre_places = rand_pre_places(mt);
		auto n_post_places = rand_pre_places(mt);
		t.pre = std::vector<std::size_t>(places.begin(), places.begin()+n_pre_places);
		t.post = std::vector<std::size_t>(places.begin()+n_pre_places, places.begin()+n_pre_places+n_post_places);

		cn.transitions.push_back(t);
	}

	std::shuffle(places.begin(), places.end(),mt);
	auto n_tokens = rand_tokens(mt);
	cn.m = Marking(n_places, false);
	for(std::size_t i = 0; i < n_tokens; i++)
		cn.m[places[i]] = true;

	return cn;
}
