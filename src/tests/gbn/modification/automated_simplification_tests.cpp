#include "../../../../libs/catch/catch.hpp"

#include "../../../gbn/general/gbn_io.h"
#include "../../../gbn/general/check.h"
#include "../../../gbn/modification/splitting.h"
#include "../../../gbn/modification/simplification.h"
#include "../../../gbn/modification/merging.h"
#include "../../../gbn/evaluation/evaluation.h"
#include "../../../gbn/general/randomized_generation.h"
#include "../../../gbn/matrix/matrix_io.h"
#include <fstream>
#include <random>

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

namespace {
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

	constexpr std::size_t N_RUNS = 100;
}

TEST_CASE("Automated: (CoUnit) simplification should not modify distribution")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.0;
	params.matrix_params.OneB_matrix_prob = 0.0;

	for(std::size_t i = 0; i < N_RUNS; i++)
	{
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		check_gbn_integrity(gbn);

		auto p_m_before = evaluate(gbn);
		auto& m_before = *p_m_before;

		for(auto v : inside_vertices(gbn))
			check_and_apply_CoUnit(gbn, v);

		auto p_m_after = evaluate(gbn);
		auto& m_after = *p_m_after;

		test_matrices_equal(m_before, m_after);
	}
}

TEST_CASE("Automated: (F1) simplification should not modify distribution")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	for(std::size_t i = 0; i < N_RUNS; i++)
	{
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		auto gbn_before = gbn;

		auto p_m_before = evaluate(gbn);
		auto& m_before = *p_m_before;

		for(auto v : inside_vertices(gbn))
			check_and_apply_F1(gbn, v);

		auto p_m_after = evaluate(gbn);
		auto& m_after = *p_m_after;

		if(!check_matrices_equal(m_before, m_after))
		{
			std::ofstream f1("test_before.dot");
			std::ofstream f2("test_after.dot");
			draw_gbn_graph(f1, gbn_before);
			draw_gbn_graph(f2, gbn);
		}

		check_gbn_integrity(gbn);

		test_matrices_equal(m_before, m_after);
	}
}

TEST_CASE("Automated: (F2) simplification should not modify distribution")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	for(std::size_t i = 0; i < N_RUNS; i++)
	{
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		auto gbn_before = gbn;

		auto p_m_before = evaluate(gbn);
		auto& m_before = *p_m_before;

		for(auto v : inside_vertices(gbn))
			check_and_apply_F2(gbn, v);

		auto p_m_after = evaluate(gbn);
		auto& m_after = *p_m_after;

		if(!check_matrices_equal(m_before, m_after))
		{
			std::ofstream f1("test_before.dot");
			std::ofstream f2("test_after.dot");
			draw_gbn_graph(f1, gbn_before);
			draw_gbn_graph(f2, gbn);
		}

		check_gbn_integrity(gbn);

		test_matrices_equal(m_before, m_after);
	}
}

TEST_CASE("Automated: (CoUnit) & (F1) & (F2) simplifications together should not modify distribution")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	for(std::size_t i = 0; i < N_RUNS; i++)
	{
		auto gbn = generate_random_gbn(1,1,4,mt,params);
		auto gbn_before = gbn;

		auto p_m_before = evaluate(gbn);
		auto& m_before = *p_m_before;

		apply_simplifications_for_each_vertex(gbn, check_and_apply_CoUnit, check_and_apply_F1, check_and_apply_F2);

		auto p_m_after = evaluate(gbn);
		auto& m_after = *p_m_after;

		if(!check_matrices_equal(m_before, m_after))
		{
			std::ofstream f1("test_before.dot");
			std::ofstream f2("test_after.dot");
			draw_gbn_graph(f1, gbn_before);
			draw_gbn_graph(f2, gbn);

			print_matrix(std::cout, m_before);
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << std::endl;
			print_matrix(std::cout, m_after);
		}

		check_gbn_integrity(gbn);

		test_matrices_equal(m_before, m_after);
	}
}

TEST_CASE("Automated: (F3) simplification should not modify distribution")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 1.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	for(std::size_t i = 0; i < N_RUNS; i++)
	{
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		auto gbn_before = gbn;

		auto p_m_before = evaluate(gbn);
		auto& m_before = *p_m_before;

		for(auto v : inside_vertices(gbn))
			check_and_apply_F3(gbn, v);

		auto p_m_after = evaluate(gbn);
		auto& m_after = *p_m_after;

		if(!check_matrices_equal(m_before, m_after))
		{
			std::ofstream f1("test_before.dot");
			std::ofstream f2("test_after.dot");
			draw_gbn_graph(f1, gbn_before);
			draw_gbn_graph(f2, gbn);
		}

		check_gbn_integrity(gbn);

		test_matrices_equal(m_before, m_after);
	}
}

TEST_CASE("Automated: (F4) simplification should not modify distribution")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 1.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	for(std::size_t i = 0; i < N_RUNS; i++)
	{
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		auto gbn_before = gbn;

		auto p_m_before = evaluate(gbn);
		auto& m_before = *p_m_before;

		for(auto v : inside_vertices(gbn))
			check_and_apply_F4(gbn, v);

		auto p_m_after = evaluate(gbn);
		auto& m_after = *p_m_after;

		if(!check_matrices_equal(m_before, m_after))
		{
			std::ofstream f1("test_before.dot");
			std::ofstream f2("test_after.dot");
			draw_gbn_graph(f1, gbn_before);
			draw_gbn_graph(f2, gbn);
		}

		check_gbn_integrity(gbn);

		test_matrices_equal(m_before, m_after);
	}
}

TEST_CASE("Automated: (F5) simplification should not modify distribution")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 1.0;
	params.matrix_params.OneB_matrix_prob = 1.0;

	for(std::size_t i = 0; i < N_RUNS; i++)
	{
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		auto gbn_before = gbn;

		auto p_m_before = evaluate(gbn);
		auto& m_before = *p_m_before;

		for(auto v : inside_vertices(gbn))
			check_and_apply_F5(gbn, v);

		auto p_m_after = evaluate(gbn);
		auto& m_after = *p_m_after;

		// if(!check_matrices_equal(m_before, m_after))
		// {
			// std::ofstream f1("test_before.dot");
			// std::ofstream f2("test_after.dot");
			// draw_gbn_graph(f1, gbn_before);
			// draw_gbn_graph(f2, gbn);
		// }

		check_gbn_integrity(gbn);

		test_matrices_equal(m_before, m_after);
	}
}

TEST_CASE("Automated: Random GBN generation should work")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.5;
	params.matrix_params.OneB_matrix_prob = 1.0;

	for(std::size_t i = 0; i < N_RUNS; i++)
	{
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		check_gbn_integrity(gbn);
	}
}

TEST_CASE("Automated: (CoUnit) - (F5) simplifications together should not modify distribution")
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.5;
	params.matrix_params.OneB_matrix_prob = 1.0;

	for(std::size_t i = 0; i < N_RUNS; i++)
	{
		auto gbn = generate_random_gbn(5,5,10,mt,params);
		auto gbn_before = gbn;

		std::ofstream f1("test_before.dot");
		draw_gbn_graph(f1, gbn_before);

		auto p_m_before = evaluate(gbn);
		auto& m_before = *p_m_before;

		apply_simplifications_for_each_vertex(gbn, check_and_apply_CoUnit, check_and_apply_F1, check_and_apply_F2, check_and_apply_F3, check_and_apply_F4, check_and_apply_F5);

		auto p_m_after = evaluate(gbn);
		auto& m_after = *p_m_after;

		if(!check_matrices_equal(m_before, m_after))
		{
			std::ofstream f1("test_before.dot");
			std::ofstream f2("test_after.dot");
			draw_gbn_graph(f1, gbn_before);
			draw_gbn_graph(f2, gbn);
		}

		check_gbn_integrity(gbn);

		test_matrices_equal(m_before, m_after);
	}
}
