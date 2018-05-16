#pragma once

#include "../general/gbn.h"

void simplification(GBN& gbn);
void non_normalizing_simplification(GBN& gbn);

template<typename ...Functions>
void apply_simplifications_for_each_vertex(GBN& gbn, const Functions& ...functions)
{
	auto funcs = { functions... };

	bool changed = false;
	do {
		changed = false;
		for(auto f : funcs)
		{
			for(auto v : inside_vertices(gbn))
			{
				changed = changed || f(gbn,v);
				if(changed)
					break;
			}
			if(changed)
				break;
		}
	}
	while(changed);
}
