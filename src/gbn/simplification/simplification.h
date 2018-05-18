#pragma once

#include "../general/gbn.h"
#include <iostream>

void simplification(GBN& gbn, std::function<void(const GBN&,std::string)> status_callback = std::function<void(const GBN&,std::string)>());
void non_normalizing_simplification(GBN& gbn, std::function<void(const GBN&,std::string)> status_callback = std::function<void(const GBN&,std::string)>());

template<typename ...Functions>
void apply_simplifications_for_each_vertex(GBN& gbn, std::function<void(const GBN&,std::string)> status_callback, const Functions& ...functions)
{
	auto funcs = { functions... };

	bool changed = false;
	do {
		changed = false;
		for(auto f : funcs)
		{
			for(auto v : inside_vertices(gbn))
			{
				std::string op;
				changed = changed || f(gbn,v,op);
				if(changed) {
					if(status_callback)
						status_callback(gbn,op);
					break;
				}
			}
			if(changed)
				break;
		}
	}
	while(changed);
}
