#include "gbn_evaluation.h"
#include "gbn_modification.h"
#include "matrix_decomposition.h"
#include <iostream>

void node_splitting(GBN& gbn, Vertex v)
{
	auto p_m = matrix(v,gbn.graph);
	auto& m = *p_m;

	// if matrix just has one output wire -> abort
	if(m.m == 1)
		return;

	auto [p_m_front, p_m_back] = matrix_decomposition(p_m);
} 	
