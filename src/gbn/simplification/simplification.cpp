#include "simplification.h"

#include "local_simplification.h"
#include "global_simplification.h"

void simplification(GBN& gbn)
{
	apply_simplifications_for_each_vertex(gbn, check_and_apply_CoUnit, check_and_apply_F1, check_and_apply_F2, check_and_apply_F3, check_and_apply_F4, check_and_apply_F5, split_vertex_if_multiple_outputs, simplify_matrix_for_duplicate_inputs, eliminate_stochastic_vertex_without_outputs, switch_substoch_to_front, normalize_substoch_front_vertices_without_inputs);
}

void non_normalizing_simplification(GBN& gbn)
{
	apply_simplifications_for_each_vertex(gbn, check_and_apply_CoUnit, check_and_apply_F1, check_and_apply_F2, check_and_apply_F3, check_and_apply_F4, check_and_apply_F5, split_vertex_if_multiple_outputs, simplify_matrix_for_duplicate_inputs, eliminate_stochastic_vertex_without_outputs, switch_substoch_to_front);
}
