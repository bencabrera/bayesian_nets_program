#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/modification/splitting.h"
#include "../../../gbn/simplification/simplification.h"
#include "../../../gbn/modification/merging.h"
#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/general/randomized_generation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../test_helpers.h"

#include <fstream>

TEST_CASE("Automated: All non normalizing simplifications simplifications")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.5;
	params.matrix_params.OneB_matrix_prob = 0.5;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		non_normalizing_simplification(gbn);
		return gbn;
	}, 
	params
	// ,[](GBN gbn_before, GBN gbn_after) -> void {
		// std::ofstream f1("test_before.dot");
		// std::ofstream f2("test_after.dot");
		// draw_gbn_graph(f1, gbn_before);
		// draw_gbn_graph(f2, gbn_after);
	// }
	);
}
