#include "operations.h"
#include <iostream>

bool check_places(std::vector<int> places, const Marking& m, bool condition)
{
	for(const auto p: places)
		if(m[p] != condition)
			return false;

	return true;
}

bool check_pre_condition(const Transition& t, const Marking& m)
{
	return check_places(t.pre, m, true);
}

bool check_post_condition(const Transition& t, const Marking& m)
{
	return check_places(t.post, m, false);
}

bool fire_transition(const Transition& t, Marking& m)
{
	if(!check_pre_condition(t,m) || !check_post_condition(t,m))
		throw std::logic_error("Trying to fire transition where pre or post condition is not satisfied.");	

	for(const auto p: t.pre)
		m[p] = 0;

	for(const auto p: t.post)
		m[p] = 1;
}
