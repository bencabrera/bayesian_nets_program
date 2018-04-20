cmake_minimum_required(VERSION 3.1.0) 

project(Petri_Nets_With_Uncertainty)

# set c++ standard to c++11
set (CMAKE_CXX_STANDARD 17)

# look for boost library (required)
find_package(Boost REQUIRED regex)
include_directories(${Boost_INCLUDE_DIRS})

# files
set(FILES
	src/io.cpp
	src/operations.cpp
	src/deterministic_operations.cpp
	src/gbn/matrix.cpp
	src/gbn/matrix_io.cpp
	src/gbn/gbn.cpp
	src/gbn/gbn_io.cpp
	src/gbn/gbn_evaluation.cpp
)

set(TEST_FILES
	src/tests/io_tests.cpp
	src/tests/matrix_io_tests.cpp
	src/tests/gbn_io_tests.cpp
	src/tests/operation_tests.cpp
	src/tests/gbn_evaluation_tests.cpp
)

add_library(core_lib STATIC ${FILES})

add_executable(runner src/main_runner.cpp)
target_link_libraries(runner core_lib)
target_link_libraries(runner Boost::regex)

add_executable(draw_gbn src/main_draw_gbn.cpp)
target_link_libraries(draw_gbn core_lib)
target_link_libraries(draw_gbn Boost::regex)

add_executable(tests src/main_tests.cpp ${TEST_FILES})
target_link_libraries(tests core_lib)
target_link_libraries(tests Boost::regex)
target_compile_definitions(tests PRIVATE FOO="${CMAKE_CURRENT_SOURCE_DIR}/src/tests/instances/")
