cmake_minimum_required(VERSION 3.1.0) 

project(Petri_Nets_With_Uncertainty)

# set c++ standard to c++11
set (CMAKE_CXX_STANDARD 17)

# look for boost library (required)
find_package(Boost REQUIRED regex)
include_directories(${Boost_INCLUDE_DIRS})

set(GENERAL_FILES
	src/bitvec.cpp
)

set(CN_FILES
	src/cn/cn_io.cpp
	src/cn/cn_operations.cpp
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
	src/gbn/matrix.cpp
	src/gbn/matrix_io.cpp
	src/gbn/matrix_decomposition.cpp
	src/gbn/gbn.cpp
	src/gbn/gbn_io.cpp
	src/gbn/gbn_check.cpp
	src/gbn/gbn_modification.cpp
	src/gbn/gbn_evaluation.cpp
	src/gbn/node_elimination.cpp
	src/gbn/node_splitting.cpp
)

set(TEST_FILES
	src/tests/cn_io_tests.cpp
	src/tests/matrix_io_tests.cpp
	src/tests/matrix_decomposition_tests.cpp
	src/tests/gbn_io_tests.cpp
	src/tests/operations_on_joint_dist_tests.cpp
	src/tests/gbn_evaluation_tests.cpp
	src/tests/node_splitting_tests.cpp
)

add_library(core_lib STATIC ${GENERAL_FILES} ${CN_FILES} ${JOINT_DIST_FILES} ${CNU_FILES} ${GBN_FILES})

add_executable(runner src/main_joint_dist_runner.cpp)
target_link_libraries(runner core_lib)
target_link_libraries(runner Boost::regex)

add_executable(draw_gbn src/main_draw_gbn.cpp)
target_link_libraries(draw_gbn core_lib)
target_link_libraries(draw_gbn Boost::regex)

add_executable(evaluate_gbn src/main_evaluate_gbn.cpp)
target_link_libraries(evaluate_gbn core_lib)
target_link_libraries(evaluate_gbn Boost::regex)

add_executable(node_elimination src/main_node_elimination.cpp)
target_link_libraries(node_elimination core_lib)
target_link_libraries(node_elimination Boost::regex)

add_executable(node_splitting src/main_node_splitting.cpp)
target_link_libraries(node_splitting core_lib)
target_link_libraries(node_splitting Boost::regex)

add_executable(tests src/main_tests.cpp ${TEST_FILES})
target_link_libraries(tests core_lib)
target_link_libraries(tests Boost::regex)
target_compile_definitions(tests PRIVATE FOO="${CMAKE_CURRENT_SOURCE_DIR}/src/tests/instances/")
