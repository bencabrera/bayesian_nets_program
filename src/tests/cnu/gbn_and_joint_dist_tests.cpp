#include "../../../libs/catch/catch.hpp"

#include <fstream>

#include "../test_helpers.h"
#include "../../gbn/general/gbn.h"
#include "../../gbn/general/special_cases.h"
#include "../../gbn/evaluation/evaluation.h"
#include "../../joint_dist/joint_dist.h"
#include "../../joint_dist/special_cases.h"
#include "../../joint_dist/joint_dist_io.h"
#include "../../gbn/matrix/matrix_io.h"
#include "../../gbn/general/gbn_io.h"
#include "../../gbn/general/check.h"
#include "../../gbn/simplification/simplification.h"
#include "../../cn/randomized_generation.h"
#include "../../cn/cn_io.h"
#include "../../cnu/fire_transition.h"
#include <random>

TEST_CASE("Uniform dist and uniform gbn should be the same.") {
	auto uniform_joint_dist = build_uniform_joint_dist(3);
	auto uniform_gbn = build_uniform_independent_obn(3);
	auto p_m = evaluate(uniform_gbn);

	test_joint_dist_matrix_equal(uniform_joint_dist, *p_m);
}

TEST_CASE("Joint dist and gbn should be the same 1") {
	JointDist joint_dist;
	for(int i1 = 0; i1 < 2; i1++)
		for(int i2 = 0; i2 < 2; i2++)
			for(int i3 = 0; i3 < 2; i3++)
				if(i1 == 0)
					joint_dist.insert({ { static_cast<bool>(i1), static_cast<bool>(i2), static_cast<bool>(i3) }, 1.0/4 });
				else
					joint_dist.insert({ { static_cast<bool>(i1), static_cast<bool>(i2), static_cast<bool>(i3) }, 0.0 });

	auto gbn = build_independent_obn({{1,0},{0.5,0.5},{0.5,0.5}});
	auto p_m = evaluate(gbn);

	test_joint_dist_matrix_equal(joint_dist, *p_m);
}

TEST_CASE("paper_example.cnu: CNU ops on joint dist and GBN should lead to same dist") 
{
	std::ifstream f_cn(TEST_INSTANCE_FOLDER + "paper_example.cn");
	auto cn = read_cn(f_cn);
	auto cn_copy = cn;

	std::ifstream f_joint_dist(TEST_INSTANCE_FOLDER + "paper_example.dist");
	auto dist = read_joint_dist(f_joint_dist);
	std::ifstream f_gbn(TEST_INSTANCE_FOLDER + "paper_example.gbn");
	auto gbn = read_gbn(f_gbn);

	fire_transition_on_gbn(cn, gbn, 3);
	fire_transition_on_joint_dist(cn_copy, dist, 3);
	simplification(gbn);

	auto p_m = evaluate(gbn);

	test_joint_dist_matrix_equal(dist, *p_m);
}

TEST_CASE("CNU ops on joint dist and GBN should lead to same dist") 
{
	std::size_t n_places = 22;
	std::size_t n_transitions = 5;
	std::size_t n_min_tokens = 1;
	std::size_t n_max_tokens = 5;
	std::size_t n_min_pre_places = 1;
	std::size_t n_max_pre_places = 2;
	std::size_t n_min_post_places = 1;
	std::size_t n_max_post_places = 2;

	std::size_t n_simplification_steps = 5;
	std::size_t n_random_transitions_per_simplify = 10;

	std::random_device rd;  
	std::mt19937 mt(rd()); 
	std::uniform_int_distribution<std::size_t> rand_transition(0,n_transitions-1);

	auto cn = randomize_cn(n_places, n_transitions, n_min_tokens, n_max_tokens, n_min_pre_places, n_max_pre_places, n_min_post_places, n_max_post_places, mt);
	auto cn_copy = cn;

	auto gbn = build_uniform_independent_obn(n_places);
	auto joint_dist = build_uniform_joint_dist(n_places);

	for(std::size_t i_simplification_step = 0; i_simplification_step < n_simplification_steps; i_simplification_step++)
	{
		for(std::size_t i_rand_transition = 0; i_rand_transition < n_random_transitions_per_simplify; i_rand_transition++)
		{
			auto i_transition = rand_transition(mt);
			fire_transition_on_gbn(cn, gbn, i_transition);
			fire_transition_on_joint_dist(cn_copy, joint_dist, i_transition);
		}
		check_gbn_integrity(gbn);
		simplification(gbn);
		check_gbn_integrity(gbn);
		auto p_m = evaluate(gbn);
		test_joint_dist_matrix_equal(joint_dist, *p_m);
	}
}
