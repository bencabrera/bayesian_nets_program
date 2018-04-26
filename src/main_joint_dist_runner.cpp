#include <iostream>
#include <fstream>

#include "cn/cn.h"
#include "cn/cn_io.h"
#include "cn/cn_operations.h"
#include "joint_dist/joint_dist.h"
#include "joint_dist/joint_dist_io.h"
#include "cnu/operations_on_joint_dist.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

void do_command(std::string command_line, CN& cn, JointDist& dist)
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
			nassert_op(transition.pre, 1, dist);

			std::cout << "- fail_pre -" << std::endl;
			if(show_details) {
				print_dist(std::cout,dist); 
				std::cout << std::endl;
			}
			return;
		}

		// fail_post case
		if(!check_post_condition(transition,cn.m))
		{
			nassert_op(transition.post, 0, dist);

			std::cout << "- fail_post -" << std::endl;
			if(show_details) {
				print_dist(std::cout,dist); 
				std::cout << std::endl;
			}
			return;
		}

		std::cout << "- success -" << std::endl;

		assert_op(pre_places,1, dist);
		if(show_details) {
			std::cout << "- assert_pre -" << std::endl;
			print_dist(std::cout,dist); 
			std::cout << std::endl;
		}

		assert_op(post_places,0, dist);
		if(show_details) {
			std::cout << "- assert_post -" << std::endl;
			print_dist(std::cout,dist); 
			std::cout << std::endl;
		}

		set_op(pre_places,0,dist);
		if(show_details) {
			std::cout << "- set_pre -" << std::endl;
			print_dist(std::cout,dist); 
			std::cout << std::endl;
		}

		set_op(post_places,1,dist);
		if(show_details) {
			std::cout << "- set_post -" << std::endl;
			print_dist(std::cout,dist); 
			std::cout << std::endl;
		}

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
		print_dist(std::cout, dist);
		std::cout << std::endl;
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
		if(argc != 3)
			throw std::logic_error("Wrong number of arguments.");

		std::ifstream cn_file(argv[1]);
		std::ifstream dist_file(argv[2]);

		auto cn = read_cn(cn_file); 
		auto dist = read_joint_dist(dist_file);

		std::string line;
		while(true) 
		{
			std::getline(std::cin, line);
			do_command(line, cn, dist);
		}
	}
	catch(const std::logic_error& e)
	{
		std::cout << "FATAL ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
