#include <iostream>
#include <fstream>

#include "cn/cn.h"
#include "cn/cn_io.h"
#include "cn/cn_operations.h"
#include "gbn/general/gbn.h"
#include "gbn/general/gbn_io.h"
#include "gbn/general/check.h"
#include "gbn/general/special_cases.h"
#include "gbn/evaluation/evaluation.h"
#include "gbn/simplification/simplification.h"
#include "gbn/matrix/matrix_io.h"
#include "cnu/operations_on_gbn.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <functional>
#include "../libs/cxxopts/include/cxxopts.hpp"

// globals (naughty, naughty, ...)
bool is_step_wise_output = false;
bool is_detailed_output = false;
std::string step_wise_path;
std::string detailed_output_folder;
std::size_t n_operations = 0;

void export_gbn(const GBN& gbn, std::string filename)
{
	std::ofstream f(filename);	
	draw_gbn_graph(f, gbn);
}

struct UpdateMetaData {
	std::size_t i_operation;
	std::string high_level_op_string;
	std::string low_level_op_string;
};

void do_command(std::string command_line, CN& cn, GBN& gbn, std::function<void(UpdateMetaData)> status_callback = std::function<void(UpdateMetaData)>())
{
	boost::trim(command_line);
	if(command_line[0] == 't')
	{
		std::vector<std::string> split_command_line;
		boost::split(split_command_line,command_line,boost::is_any_of(" "));

		bool show_details = false;
		if(split_command_line.size() > 1)
		{
			boost::trim(split_command_line[1]);
			if(split_command_line[1] == "details")
				show_details = true;
		}

		auto i_transition = std::stoi(split_command_line[0].substr(1));
		const auto& transition = cn.transitions[i_transition];
		const auto& pre_places = transition.pre;
		const auto& post_places = transition.post;

		// fail_pre case
		if(!check_pre_condition(transition,cn.m))
		{
			nassert_op(transition.pre, 1, gbn);
			check_gbn_integrity(gbn);

			if(status_callback)
				status_callback(UpdateMetaData{n_operations++, std::string("fail-pre_{t")+std::to_string(i_transition)+"}", std::string("nassert") });
			return;
		}

		// fail_post case
		if(!check_post_condition(transition,cn.m))
		{
			nassert_op(transition.post, 0, gbn);
			check_gbn_integrity(gbn);

			if(status_callback)
				status_callback(UpdateMetaData{n_operations++, std::string("fail-post_{t")+std::to_string(i_transition)+"}", std::string("nassert") });
			return;
		}

		assert_op(pre_places,1, gbn);
		check_gbn_integrity(gbn);
		if(status_callback)
			status_callback(UpdateMetaData{n_operations++, std::string("success_{t")+std::to_string(i_transition)+"}", std::string("assert_pre") });

		assert_op(post_places,0, gbn);
		check_gbn_integrity(gbn);
		if(status_callback)
			status_callback(UpdateMetaData{n_operations++, std::string("success_{t")+std::to_string(i_transition)+"}", std::string("assert_post") });

		set_op(pre_places,0,gbn);
		check_gbn_integrity(gbn);
		if(status_callback)
			status_callback(UpdateMetaData{n_operations++, std::string("success_{t")+std::to_string(i_transition)+"}", std::string("set_pre") });

		set_op(post_places,1,gbn);
		check_gbn_integrity(gbn);
		if(status_callback)
			status_callback(UpdateMetaData{n_operations++, std::string("success_{t")+std::to_string(i_transition)+"}", std::string("set_post") });

		// update marking in CNU
		for(const auto& p : transition.pre)
			cn.m[p] = 0;
		for(const auto& p : transition.post)
			cn.m[p] = 1;

		return;
	}

	if(command_line.substr(0,4) == "dist")
	{
		auto p_m = evaluate(gbn);
		print_matrix(std::cout, *p_m);
		return;
	}

	if(command_line.substr(0,4) == "draw")
	{
		if(!is_step_wise_output)
			throw std::logic_error("To use 'draw' command please start runner with --step-wise-draw-file");

		std::vector<std::string> tmp_vec;
		boost::split(tmp_vec, command_line, boost::is_any_of(" "));
		std::ofstream f(step_wise_path);
		draw_gbn_graph(f, gbn);
		return;
	}

	if(command_line.substr(0,8) == "simplify" || command_line.substr(0,14) == "simplification")
	{
		simplification(gbn);
		return;
	}

	if(command_line.substr(0,3) == "cn")
	{
		print_cn_details(std::cout, cn);
		return;
	}

	std::cout << "Command '" << command_line << "' could not be parsed." << std::endl;
}

CN get_cn(const cxxopts::ParseResult& params)
{
	if(!params.count("cn"))
		throw std::logic_error("Please provide CN file.");

	std::ifstream cn_file(params["cn"].as<std::string>());
	if(!cn_file.is_open())
		throw std::logic_error("CN file could not be opened.");

	return read_cn(cn_file); 
}

GBN get_gbn(const cxxopts::ParseResult& params, const CN& cn)
{
	if(params.count("gbn"))
	{
		std::ifstream gbn_file(params["gbn"].as<std::string>());
		auto gbn = read_gbn(gbn_file);

		if(gbn.n != 0)
			throw std::logic_error("For CNU application, GBN has to have n = 0.");

		if(gbn.m != cn.n)
			throw std::logic_error("For CNU application, GBN has to have m = |cn.places|.");

		return gbn;
	} 

	if(params.count("gbn-uniform-init")) {
		return build_uniform_independent_obn(cn.n);
	}

	throw std::logic_error("Please provide some source of GBN.");
}

int main(int argc, const char** argv)
{
	cxxopts::Options options("random_cn", "Generate random cn instance.");
	options.add_options()
		("help", "Produces this help message.")

		("cn", "Path to .cn file containing petri net model.", cxxopts::value<std::string>())
		("gbn", "Path to .gbn file containing initial gbn.", cxxopts::value<std::string>())
		("gbn-uniform-init", "If this flag is set an initial gbn is used where all vertices are independent and initialiazed with (1/2, 1/2).")
		("step-wise-draw-file", "Path to a .dot file containing the GBN and updated after each 'draw' command.", cxxopts::value<std::string>())
		("detailed-draw-folder", "Path to a folder where after each operation a new .dot file is created.", cxxopts::value<std::string>())
		;
	options.positional_help("<cn> <gbn>").show_positional_help();
	options.parse_positional({"cn", "gbn"});
	auto params = options.parse(argc, argv);

	// show help and exit program if --help is set
	if (params.count("help")) {
		std::cout << options.help() << std::endl;
		return 0;
	}

	auto cn = get_cn(params);
	auto gbn = get_gbn(params,cn);

	is_step_wise_output = params.count("step-wise-draw-file") > 0;
	if(is_step_wise_output) 
		step_wise_path = params["step-wise-draw-file"].as<std::string>();

	is_detailed_output = params.count("detailed-draw-folder") > 0;
	if(is_detailed_output) {
		detailed_output_folder = params["detailed-draw-folder"].as<std::string>();
		if(!detailed_output_folder.empty() && detailed_output_folder.back() != '/')
			detailed_output_folder += '/';
	}

	if(is_detailed_output)
	{
		std::ofstream f(detailed_output_folder + std::to_string(0) + ".dot");
		draw_gbn_graph(f, gbn, "init");
	}

	std::function<void(UpdateMetaData)> callback;

	if(is_detailed_output) {
		callback = [&gbn] (const UpdateMetaData& meta_data) {
			std::cout << meta_data.high_level_op_string << " " << meta_data.low_level_op_string << std::endl;

			std::ofstream f(detailed_output_folder + std::to_string(n_operations) + ".dot");
			draw_gbn_graph(f, gbn, meta_data.low_level_op_string);
		};
	} else {
		callback = [&gbn] (const UpdateMetaData& meta_data) {
			std::cout << meta_data.high_level_op_string << " " << meta_data.low_level_op_string << std::endl;
		};
	}

	std::string line;
	while(true) 
	{
		std::getline(std::cin, line);
		do_command(line, cn, gbn, callback);
		std::cout << std::endl;
	}

	return 0;
}
