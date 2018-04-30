cmake_minimum_required(VERSION 3.1.0) 

project(Petri_Nets_With_Uncertainty)

# set c++ standard to c++11
set (CMAKE_CXX_STANDARD 17)

# warning level
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Werror=return-type")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# look for boost library (required)
find_package(Boost REQUIRED regex)
include_directories(${Boost_INCLUDE_DIRS})

set(GENERAL_FILES
	src/bitvec.cpp
)

set(CN_FILES
	src/cn/cn_io.cpp
	src/cn/cn_operations.cpp
	src/cn/randomized_generation.cpp
)

set(JOINT_DIST_FILES
	src/joint_dist/joint_dist_check.cpp
	src/joint_dist/joint_dist_io.cpp
)

set(CNU_FILES
	src/cnu/operations_on_gbn.cpp
	src/cnu/operations_on_joint_dist.cpp
)

set(GBN_FILES
	src/gbn/matrix/matrix.cpp
	src/gbn/matrix/matrix_io.cpp
	src/gbn/matrix/matrix_decomposition.cpp
	src/gbn/matrix/randomized_generation.cpp

	src/gbn/general/gbn.cpp
	src/gbn/general/gbn_io.cpp
	src/gbn/general/check.cpp
	src/gbn/general/evaluation.cpp
	src/gbn/general/vertex_set_input_output.cpp
	src/gbn/general/path_closing.cpp
	src/gbn/general/randomized_generation.cpp

	src/gbn/modification/vertex_add_remove.cpp
	src/gbn/modification/merging.cpp
	src/gbn/modification/splitting.cpp
	src/gbn/modification/simplification.cpp
)

set(TEST_FILES
	src/tests/cn/cn_io_tests.cpp

	src/tests/gbn/matrix_io_tests.cpp
	src/tests/gbn/matrix_decomposition_tests.cpp
	src/tests/gbn/gbn_io_tests.cpp
	src/tests/gbn/evaluation_tests.cpp
	src/tests/gbn/splitting_tests.cpp
	src/tests/gbn/simplification_tests.cpp
	src/tests/gbn/automated_simplification_tests.cpp

	src/tests/cnu/operations_on_joint_dist_tests.cpp
)

add_library(core_lib STATIC ${GENERAL_FILES} ${CN_FILES} ${JOINT_DIST_FILES} ${CNU_FILES} ${GBN_FILES})

add_executable(joint_dist_runner src/main_joint_dist_runner.cpp)
target_link_libraries(joint_dist_runner core_lib)
target_link_libraries(joint_dist_runner Boost::regex)

add_executable(gbn_runner src/main_gbn_runner.cpp)
target_link_libraries(gbn_runner core_lib)
target_link_libraries(gbn_runner Boost::regex)

add_executable(draw_gbn src/main_draw_gbn.cpp)
target_link_libraries(draw_gbn core_lib)
target_link_libraries(draw_gbn Boost::regex)

add_executable(draw_cn src/main_draw_cn.cpp)
target_link_libraries(draw_cn core_lib)
target_link_libraries(draw_cn Boost::regex)

add_executable(evaluate_gbn src/main_evaluate_gbn.cpp)
target_link_libraries(evaluate_gbn core_lib)
target_link_libraries(evaluate_gbn Boost::regex)

add_executable(merging src/main_merging.cpp)
target_link_libraries(merging core_lib)
target_link_libraries(merging Boost::regex)

add_executable(splitting src/main_splitting.cpp)
target_link_libraries(splitting core_lib)
target_link_libraries(splitting Boost::regex)

add_executable(simplification src/main_simplification.cpp)
target_link_libraries(simplification core_lib)
target_link_libraries(simplification Boost::regex)

add_executable(path_closing src/main_path_closing.cpp)
target_link_libraries(path_closing core_lib)
target_link_libraries(path_closing Boost::regex)

add_executable(random_cn src/main_random_cn.cpp)
target_link_libraries(random_cn core_lib)
target_link_libraries(random_cn Boost::regex)

add_executable(random_gbn src/main_random_gbn.cpp)
target_link_libraries(random_gbn core_lib)
target_link_libraries(random_gbn Boost::regex)

add_executable(tests src/main_tests.cpp ${TEST_FILES})
target_link_libraries(tests core_lib)
target_link_libraries(tests Boost::regex)
target_compile_definitions(tests PRIVATE FOO="${CMAKE_CURRENT_SOURCE_DIR}/src/tests/instances/")
