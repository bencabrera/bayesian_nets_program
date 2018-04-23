#include <iostream>
#include <fstream>

#include "cn/cn.h"
#include "cn/cn_io.h"
#include "cn/cn_operations.h"
#include "gbn/gbn.h"
#include "gbn/gbn_io.h"
#include "gbn/gbn_check.h"
#include "gbn/gbn_evaluation.h"
#include "gbn/matrix_io.h"
#include "cnu/operations_on_gbn.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <functional>

// globals (naughty, naughty, ...)
std::string output_folder;
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
		std::cout << std::endl;
		auto p_m = evaluate_gbn(gbn);
		print_matrix(std::cout, *p_m);
		std::cout << std::endl;
		return;
	}

	if(command_line.substr(0,4) == "draw")
	{
		std::cout << std::endl;
		std::vector<std::string> tmp_vec;
		boost::split(tmp_vec, command_line, boost::is_any_of(" "));
		std::ofstream f(output_folder + tmp_vec[1]);
		draw_gbn_graph(f, gbn);
		return;
	}

	if(command_line.substr(0,3) == "cn")
	{
		std::cout << std::endl;
		print_cn_details(std::cout, cn);
		std::cout << std::endl;
		return;
	}
}

int main(int argc, char** argv)
{
	try {
		if(argc != 4)
			throw std::logic_error("Wrong number of arguments.");

		std::ifstream cn_file(argv[1]);
		std::ifstream gbn_file(argv[2]);
		output_folder = argv[3];
		if(output_folder.back() != '/')
			output_folder += '/';

		auto cn = read_cn(cn_file); 
		auto gbn = read_gbn(gbn_file);

		std::ofstream f(output_folder + std::to_string(0) + ".dot");
		draw_gbn_graph(f, gbn, "init");

		if(gbn.n != 0)
			throw std::logic_error("For CNU application, GBN has to have n = 0.");

		if(gbn.m != cn.n)
			throw std::logic_error("For CNU application, GBN has to have m = |cn.places|.");

		auto callback = [&gbn] (const UpdateMetaData& meta_data) {
			std::cout << meta_data.high_level_op_string << " " << meta_data.low_level_op_string << std::endl;

			std::ofstream f(output_folder + std::to_string(n_operations) + ".dot");
			draw_gbn_graph(f, gbn, meta_data.low_level_op_string);
		};

		std::string line;
		while(true) 
		{
			std::getline(std::cin, line);
			do_command(line, cn, gbn, callback);
		}
	}
	catch(const std::logic_error& e)
	{
		std::cout << "FATAL ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
