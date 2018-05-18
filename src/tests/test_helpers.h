#pragma once

#include "../gbn/matrix/matrix.h"
#include "../gbn/general/gbn.h"
#include "../gbn/general/randomized_generation.h"
#include "../joint_dist/joint_dist.h"

#ifdef FOO 
const std::string TEST_INSTANCE_FOLDER = FOO;
#else
const std::string TEST_INSTANCE_FOLDER = "";
#endif

constexpr std::size_t N_AUTOMATED_RUNS = 100;

void test_matrices_equal(const Matrix& m1, const Matrix& m2);
bool check_matrices_equal(const Matrix& m1, const Matrix& m2);

void test_joint_dist_matrix_equal(const JointDist& joint_dist, const Matrix& m);
bool check_joint_dist_matrix_equal(const JointDist& joint_dist, const Matrix& m);

void compare_joint_dists(const JointDist& dist1, const JointDist& dist2);

GBN read_and_check_gbn(std::string path);
GBN check_evaluates_equal_after_operation(GBN gbn_before, std::function<GBN(GBN)> operation, std::function<void(GBN,GBN)> additional_check_after_operation = std::function<void(GBN,GBN)>());
void randomized_check_evaluates_equal_after_operation(std::function<GBN(GBN)> operation, RandomGBNParams params = RandomGBNParams(), std::function<void(GBN,GBN)> additional_check_after_operation = std::function<void(GBN,GBN)>(), std::size_t n = N_AUTOMATED_RUNS);
