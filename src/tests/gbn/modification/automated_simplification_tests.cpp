#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/modification/splitting.h"
#include "../../../gbn/modification/simplification.h"
#include "../../../gbn/modification/merging.h"
#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/general/randomized_generation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../test_helpers.h"

TEST_CASE("Automated: (CoUnit) simplification")
{
	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.0;
	params.matrix_params.OneB_matrix_prob = 0.0;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		for(auto v : inside_vertices(gbn))
			check_and_apply_CoUnit(gbn, v);
		return gbn;
	}, params);
}

TEST_CASE("Automated: (F1) simplification")
{
	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		for(auto v : inside_vertices(gbn))
			check_and_apply_F1(gbn, v);
		return gbn;
	}, params);
}

TEST_CASE("Automated: (F2) simplification should not modify distribution")
{
	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		for(auto v : inside_vertices(gbn))
			check_and_apply_F2(gbn, v);
		return gbn;
	}, params);
}

TEST_CASE("Automated: (CoUnit) & (F1) & (F2) simplifications together should not modify distribution")
{
	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		apply_simplifications_for_each_vertex(gbn, check_and_apply_CoUnit, check_and_apply_F1, check_and_apply_F2);
		return gbn;
	}, params);
}

TEST_CASE("Automated: (F3) simplification should not modify distribution")
{
	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 1.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		for(auto v : inside_vertices(gbn))
			check_and_apply_F3(gbn, v);
		return gbn;
	}, params);
}

TEST_CASE("Automated: (F4) simplification should not modify distribution")
{
	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 1.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		for(auto v : inside_vertices(gbn))
			check_and_apply_F4(gbn, v);
		return gbn;
	}, params);
}

TEST_CASE("Automated: (F5) simplification should not modify distribution")
{
	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 1.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		for(auto v : inside_vertices(gbn))
			check_and_apply_F5(gbn, v);
		return gbn;
	}, params);
}

TEST_CASE("Automated: (CoUnit) - (F5) simplifications together should not modify distribution")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.5;
	params.matrix_params.OneB_matrix_prob = 1.0;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		apply_simplifications_for_each_vertex(gbn, check_and_apply_CoUnit, check_and_apply_F1, check_and_apply_F2, check_and_apply_F3, check_and_apply_F4, check_and_apply_F5);
		return gbn;
	}, params);
}

