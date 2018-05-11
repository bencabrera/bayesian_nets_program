#include "../../../../libs/catch/catch.hpp"

#include "../../test_helpers.h"
#include "../../../gbn/general/check.h"

TEST_CASE("Automated: Random GBN generation should work")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.5;
	params.matrix_params.OneB_matrix_prob = 1.0;

	for(std::size_t i = 0; i < N_AUTOMATED_RUNS; i++)
	{
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		check_gbn_integrity(gbn);
	}
}

