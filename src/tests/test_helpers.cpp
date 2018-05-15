#include "test_helpers.h"

#include <fstream>
#include <random>

#include "../../libs/catch/catch.hpp"
#include "../gbn/general/gbn_io.h"
#include "../gbn/general/check.h"
#include "../gbn/matrix/matrix_io.h"
#include "../gbn/evaluation/evaluation.h"

void test_matrices_equal(const Matrix& m1, const Matrix& m2)
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

bool check_matrices_equal(const Matrix& m1, const Matrix& m2)
{
	REQUIRE(m1.n == m2.n);
	REQUIRE(m1.m == m2.m);

	unsigned long long to_max = 1;
	unsigned long long from_max = 1;
	to_max = to_max << m1.m;
	from_max = from_max << m1.n;
	for(Index to = 0; to < to_max; to++)
		for(Index from = 0; from < from_max; from++)
			if(m1.get(to, from) != Approx(m2.get(to, from)))
			{
				return false;
			}

	return true;
}

GBN read_and_check_gbn(std::string path)
{
	std::ifstream f(path);
	auto gbn = read_gbn(f);
	check_gbn_integrity(gbn);

	return gbn;
}

GBN check_evaluates_equal_after_operation(GBN gbn_before, std::function<GBN(GBN)> operation, std::function<void(GBN,GBN)> additional_check_after_operation)
{
	auto p_m_before = evaluate(gbn_before);
	auto& m_before = *p_m_before;

	auto gbn_after = operation(gbn_before);

	if(additional_check_after_operation)
		additional_check_after_operation(gbn_before,gbn_after);

	check_gbn_integrity(gbn_after);

	auto p_m_after = evaluate(gbn_after);
	auto& m_after = *p_m_after;

	if(!check_matrices_equal(m_before, m_after))
	{
		std::ofstream f1("test_before.dot");
		std::ofstream f2("test_after.dot");
		draw_gbn_graph(f1, gbn_before);
		draw_gbn_graph(f2, gbn_after);

		std::cout << "before: " << std::endl;
		print_matrix(std::cout, m_before);
		std::cout << std::endl << std::endl << std::endl;

		std::cout << "after: " << std::endl;
		print_matrix(std::cout, m_after);
		std::cout << std::endl << std::endl << std::endl;
	}

	test_matrices_equal(m_before, m_after);

	return gbn_after;
}

void randomized_check_evaluates_equal_after_operation(std::function<GBN(GBN)> operation, RandomGBNParams params, std::function<void(GBN,GBN)> additional_check_after_operation, std::size_t n)
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	for(std::size_t i = 0; i < n; i++)
	{
		std::cout << i << std::endl;
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		check_gbn_integrity(gbn);

		check_evaluates_equal_after_operation(gbn, operation, additional_check_after_operation);
	}
}
