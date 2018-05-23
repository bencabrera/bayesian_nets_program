#include "random_transition_helper.h"

RandomTransitionHelper::RandomTransitionHelper(const CN& cn, Type type, double p_success)
	:cn(cn), 
	i_transitions(cn.transitions.size(), 0),
	type(type), 
	p_success(p_success), 
	rand_transition(0,cn.transitions.size()-1),
	uniform_0_1(0,1)
{
	std::iota(i_transitions.begin(), i_transitions.end(), 0);	
}

std::size_t RandomTransitionHelper::next(std::mt19937& mt) 
{
	switch(type) {
		case UNIFORM:
			return rand_transition(mt);

		case FORCED: 
			{
				bool force_success = p_success > uniform_0_1(mt);

				if(force_success) {
					std::shuffle(i_transitions.begin(), i_transitions.end(), mt);
					for(auto i_transition : i_transitions)
					{
						if(check_pre_condition(cn.transitions[i_transition],cn.m) && check_post_condition(cn.transitions[i_transition],cn.m))
							return i_transition;
					}
					return i_transitions.front();
				}
				else
					return rand_transition(mt);
			}
		default:
			throw std::logic_error("Undefined case.");
	}
}
