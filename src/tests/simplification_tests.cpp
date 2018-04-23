#include "../../libs/catch/catch.hpp"

#include "../gbn/gbn_io.h"
#include "../gbn/gbn_check.h"
#include "../gbn/node_splitting.h"
#include "../gbn/gbn_simplification.h"
#include "../gbn/node_elimination.h"
#include "../gbn/gbn_evaluation.h"
#include <fstream>
#include "../gbn/matrix_io.h"

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

namespace {
	bool test_matrices_equal(const Matrix& m1, const Matrix& m2)
	{
		REQUIRE(m1.n == m2.n);
		REQUIRE(m1.m == m2.m);

		unsigned long long to_max = 1;
		unsigned long long from_max = 1;
		to_max = to_max << m1.m;
		from_max = from_max << m1.n;
		for(Index to = 0; to < to_max; to++)
			for(Index from = 0; from < from_max; from++)
				REQUIRE(m1.get(to, from) == Approx(m2.get(to, from)));
	}
}

TEST_CASE("OneB replacement (F1) should work correctly.")
{
	std::ifstream f(TEST_INSTANCE_FOLDER + "multiple_1_bs.gbn");
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	auto p_m_before = evaluate_gbn(gbn);
	auto& m_before = *p_m_before;

	gbn_simplification(gbn);

	auto p_m_after = evaluate_gbn(gbn);
	auto& m_after = *p_m_after;

	test_matrices_equal(m_before, m_after);
}
