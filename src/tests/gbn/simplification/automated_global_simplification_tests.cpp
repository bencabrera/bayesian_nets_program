#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/modification/splitting.h"
#include "../../../gbn/simplification/global_simplification.h"
#include "../../../gbn/simplification/simplification.h"
#include "../../../gbn/modification/merging.h"
#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/general/randomized_generation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include "../../test_helpers.h"

#include <fstream>

TEST_CASE("Automated: (StochWithoutOutputs)")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.5;
	params.matrix_params.OneB_matrix_prob = 1.0;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		apply_simplifications_for_each_vertex(gbn, std::function<void(const GBN&, std::string)>(), eliminate_stochastic_vertex_without_outputs);
		return gbn;
	}, params);
}

TEST_CASE("Automated: (SwitchSubstochToFront)")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 1.0;
	params.matrix_params.OneB_matrix_prob = 0.5;

	randomized_check_evaluates_equal_after_operation([](GBN gbn) -> GBN { 
		apply_simplifications_for_each_vertex(gbn, std::function<void(const GBN&, std::string)>(), switch_substoch_to_front);
		return gbn;
	}, 
	params, 
	[](GBN /*gbn_before*/, GBN gbn_after) -> void {
		auto& g = gbn_after.graph;
		for(auto v : inside_vertices(gbn_after))
		{
			if(!matrix(v,g)->is_stochastic)
			{
				for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
					REQUIRE(type(boost::source(e,g),g) == INPUT);

			}
		}
	});
}
