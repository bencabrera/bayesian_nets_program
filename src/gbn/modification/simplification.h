#pragma once

#include "../general/gbn.h"
#include <iostream>

// void gbn_eliminate_without_outputs(GBN& gbn);
void gbn_simplification(GBN& gbn);

bool check_and_apply_F1(GBN& gbn, Vertex v);
bool check_and_apply_F2(GBN& gbn, Vertex v);
bool check_and_apply_CoUnit(GBN& gbn, Vertex v);
bool check_and_apply_F3(GBN& gbn, Vertex v);
bool check_and_apply_F4(GBN& gbn, Vertex v);
bool check_and_apply_F5(GBN& gbn, Vertex v);

bool eliminate_stochastic_vertex_without_outputs(GBN& gbn, Vertex v);
bool switch_substoch_to_front(GBN& gbn, Vertex v);
bool normalize_substoch_front_vertices_without_inputs(GBN& gbn, Vertex v);

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
