cmake_minimum_required(VERSION 3.1.0) 

project(Petri_Nets_With_Uncertainty)

# set c++ standard to c++11
set (CMAKE_CXX_STANDARD 17)

# look for boost library (required)
find_package(Boost REQUIRED regex)
include_directories(${Boost_INCLUDE_DIRS})

# files
set(FILES
	src/cnu.cpp
)

set(TEST_FILES
)

add_library(core_lib STATIC ${FILES})

add_executable(runner src/main_runner.cpp)
target_link_libraries(runner core_lib)
target_link_libraries(runner Boost::regex)
# add_executable(tests src/tests/main_tests.cpp ${TEST_FILES})
# target_link_libraries(tests core_lib)
