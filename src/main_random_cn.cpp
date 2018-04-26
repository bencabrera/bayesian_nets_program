#include <iostream>
#include <fstream>

#include "cn/cn.h"
#include "cn/randomized_generation.h"
#include "cn/cn_io.h"

#include "../libs/cxxopts/include/cxxopts.hpp"

int main(int argc, const char* argv[])
{
	cxxopts::Options options("random_cn", "Generate random cn instance.");
	options.add_options()
		("help", "Produces this help message.")

		("min-tokens", "Min number of tokens", cxxopts::value<std::size_t>()->default_value("0"))
		("max-tokens", "Max number of tokens", cxxopts::value<std::size_t>()->default_value("0"))

		("min-pre-places", "Min number of pre places", cxxopts::value<std::size_t>()->default_value("2"))
		("max-pre-places", "Max number of pre places", cxxopts::value<std::size_t>()->default_value("3"))

		("min-post-places", "Min number of post places", cxxopts::value<std::size_t>()->default_value("2"))
		("max-post-places", "Max number of post places", cxxopts::value<std::size_t>()->default_value("3"))

		("places", "Number of places", cxxopts::value<std::size_t>())
		("transitions", "Number of transitions", cxxopts::value<std::size_t>())
		("file", "Output file", cxxopts::value<std::string>())
		("seed", "Seed", cxxopts::value<std::size_t>()->default_value("0"))
		;
	options.positional_help("<places> <transitions> <file>").show_positional_help();
	options.parse_positional({"places", "transitions", "file"});
	auto params = options.parse(argc, argv);

	// show help and exit program if --help is set
	if (params.count("help")) {
		std::cout << options.help() << std::endl;
		return 0;
	}

	if(!params.count("places") || !params.count("transitions") || !params.count("file"))
		throw std::logic_error("Wrong parameters");

	std::size_t n_places = params["places"].as<std::size_t>(); 
	std::size_t n_transitions = params["transitions"].as<std::size_t>(); 
	std::size_t n_min_tokens = params["min-tokens"].as<std::size_t>();
	std::size_t n_max_tokens = params["max-tokens"].as<std::size_t>();
	std::size_t n_min_pre_places = params["min-pre-places"].as<std::size_t>();
	std::size_t n_max_pre_places = params["max-pre-places"].as<std::size_t>();
	std::size_t n_min_post_places = params["min-post-places"].as<std::size_t>(); 
	std::size_t n_max_post_places = params["max-post-places"].as<std::size_t>();
	std::size_t seed = params["seed"].as<std::size_t>();

	std::string path = params["file"].as<std::string>();

    std::random_device rd;  
    std::mt19937 mt(rd()); 

	if(seed != 0)
		mt.seed(seed);

	auto cn = randomize_cn(n_places, n_transitions, n_min_tokens, n_max_tokens, n_min_pre_places, n_max_pre_places, n_min_post_places, n_max_post_places, mt);

	std::ofstream f(path);
	export_cn(f, cn);

	return 0;
}
