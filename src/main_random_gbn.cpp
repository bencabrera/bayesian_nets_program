#include <iostream>
#include <fstream>

#include "gbn/general/gbn.h"
#include "gbn/general/gbn_io.h"
#include "gbn/general/randomized_generation.h"
#include "gbn/general/evaluation.h"
#include "gbn/general/check.h"
#include "gbn/matrix/matrix_io.h"

// #include "../libs/cxxopts/include/cxxopts.hpp"

int main(int /*argc*/, const char** /*argv[]*/)
{
	// cxxopts::Options options("random_cn", "Generate random cn instance.");
	// options.add_options()
		// ("help", "Produces this help message.")

		// ("min-tokens", "Min number of tokens", cxxopts::value<std::size_t>()->default_value("0"))
		// ("max-tokens", "Max number of tokens", cxxopts::value<std::size_t>()->default_value("0"))

		// ("min-pre-places", "Min number of pre places", cxxopts::value<std::size_t>()->default_value("2"))
		// ("max-pre-places", "Max number of pre places", cxxopts::value<std::size_t>()->default_value("3"))

		// ("min-post-places", "Min number of post places", cxxopts::value<std::size_t>()->default_value("2"))
		// ("max-post-places", "Max number of post places", cxxopts::value<std::size_t>()->default_value("3"))

		// ("places", "Number of places", cxxopts::value<std::size_t>())
		// ("transitions", "Number of transitions", cxxopts::value<std::size_t>())
		// ("file", "Output file", cxxopts::value<std::string>())
		// ("seed", "Seed", cxxopts::value<std::size_t>()->default_value("0"))
		// ;
	// options.positional_help("<places> <transitions> <file>").show_positional_help();
	// options.parse_positional({"places", "transitions", "file"});
	// auto params = options.parse(argc, argv);

	// // show help and exit program if --help is set
	// if (params.count("help")) {
		// std::cout << options.help() << std::endl;
		// return 0;
	// }

	// if(!params.count("places") || !params.count("transitions") || !params.count("file"))
		// throw std::logic_error("Wrong parameters");

	// std::size_t n_places = params["places"].as<std::size_t>(); 
	// std::size_t n_transitions = params["transitions"].as<std::size_t>(); 
	// std::size_t n_min_tokens = params["min-tokens"].as<std::size_t>();
	// std::size_t n_max_tokens = params["max-tokens"].as<std::size_t>();
	// std::size_t n_min_pre_places = params["min-pre-places"].as<std::size_t>();
	// std::size_t n_max_pre_places = params["max-pre-places"].as<std::size_t>();
	// std::size_t n_min_post_places = params["min-post-places"].as<std::size_t>(); 
	// std::size_t n_max_post_places = params["max-post-places"].as<std::size_t>();
	// std::size_t seed = params["seed"].as<std::size_t>();

	// std::string path = params["file"].as<std::string>();

    std::random_device rd;  
    std::mt19937 mt(rd()); 

	RandomGBNParams params;
	params.vertex_window_size = 10;
	params.matrix_params.F_matrix_prob = 0.0;
	params.matrix_params.OneB_matrix_prob = 0.0;

	auto gbn = generate_random_gbn(5,5,10,mt,params);


	std::ofstream f("test.dot");
	draw_gbn_graph(f, gbn);

	for(auto v : inside_vertices(gbn))
	{
		if(type(v,gbn.graph) == NODE)
		{
			std::set<int> used_output_ports;
			for(auto e : boost::make_iterator_range(boost::out_edges(v, gbn.graph)))
			{
				used_output_ports.insert(port_from(e,gbn.graph));
			}

			std::cout << name(v,gbn.graph) << " " << boost::in_degree(v,gbn.graph) << " " << used_output_ports.size() << std::endl;
		}
	}


	check_gbn_integrity(gbn);
	auto p_m = evaluate_gbn(gbn);
	print_matrix(std::cout, *p_m);



	return 0;
}
