
#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "../../libs/cxxopts/include/cxxopts.hpp"

#include "../gbn/general/gbn.h"
#include "../gbn/general/gbn_io.h"
#include "../gbn/general/special_cases.h"
#include "../gbn/general/randomized_generation.h"
#include "../gbn/simplification/simplification.h"
#include "helpers/random_transition_helper.h"
#include "helpers/cn_parameters.h"
#include <boost/filesystem.hpp>

int main(int /*argc*/, const char** /*argv*/)
{
    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.n_inputs = 8;
	params.n_outputs = 8;
	params.n_inside_vertices = 15;
	// params.n_inputs = 8;
	// params.n_outputs = 8;
	// params.n_inside_vertices = 15;
	params.vertex_window_size = 16;

	params.n_min_precessors = 0;
	params.n_max_precessors = 5;
	params.n_min_successors = 0;
	params.n_max_successors = 5;

	params.matrix_params.F_matrix_prob = 1;
	params.matrix_params.OneB_matrix_prob = 0.7;

	auto gbn = generate_random_gbn(mt,params);

	// check if folder exists, otherwise create, delete contents
	if(boost::filesystem::is_directory("out"))
	{
		if(boost::filesystem::is_directory("out/dot"))
		{
			boost::filesystem::path path_to_remove("out/dot");
			for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it!=end_dir_it; ++it) {
				boost::filesystem::remove_all(it->path());
			}
		}
		else
			boost::filesystem::create_directory("out/dot");
	}
	else
	{
		boost::filesystem::create_directory("out");
		boost::filesystem::create_directory("out/dot");
	}

	std::size_t i_op = 0;
	auto callback = [&i_op](const GBN& gbn, std::string op) {
		std::stringstream ss;
		ss << "out/dot/";
		ss << std::setfill('0') << std::setw(3) << i_op;
		ss << ".dot";
		std::ofstream f(ss.str());

		std::stringstream title;
		title << "[Step " << i_op << "] " << op;

		draw_gbn_graph(f, gbn, title.str(), true);
		i_op++;
	};
	simplification(gbn, callback);
}
