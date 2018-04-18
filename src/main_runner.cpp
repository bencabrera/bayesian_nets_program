// Next TODOs:
// - implement and check set, assert and nassert operations

#include <iostream>
#include <fstream>

#include "cnu.h"
#include "io.h"
#include "operations.h"
#include "deterministic_operations.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

void do_command(std::string command_line, CNU& cnu)
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
		const auto& transition = cnu.transitions[i_transition];
		const auto& pre_places = transition.pre;
		const auto& post_places = transition.post;

		// fail_pre case
		if(!check_pre_condition(transition,cnu.m))
		{
			nassert_op(transition.pre, 1, cnu.dist);

			std::cout << "- fail_pre -" << std::endl;
			if(show_details) {
				print_dist(std::cout,cnu.dist,cnu.n); 
				std::cout << std::endl;
			}
			return;
		}

		// fail_post case
		if(!check_post_condition(transition,cnu.m))
		{
			nassert_op(transition.post, 0, cnu.dist);

			std::cout << "- fail_post -" << std::endl;
			if(show_details) {
				print_dist(std::cout,cnu.dist,cnu.n); 
				std::cout << std::endl;
			}
			return;
		}

		std::cout << "- success -" << std::endl;

		assert_op(pre_places,1, cnu.dist);
		if(show_details) {
			std::cout << "- assert_pre -" << std::endl;
			print_dist(std::cout,cnu.dist,cnu.n); 
			std::cout << std::endl;
		}

		assert_op(post_places,0, cnu.dist);
		if(show_details) {
			std::cout << "- assert_post -" << std::endl;
			print_dist(std::cout,cnu.dist,cnu.n); 
			std::cout << std::endl;
		}

		set_op(pre_places,0,cnu.dist);
		if(show_details) {
			std::cout << "- set_pre -" << std::endl;
			print_dist(std::cout,cnu.dist,cnu.n); 
			std::cout << std::endl;
		}

		set_op(post_places,1,cnu.dist);
		if(show_details) {
			std::cout << "- set_post -" << std::endl;
			print_dist(std::cout,cnu.dist,cnu.n); 
			std::cout << std::endl;
		}

		return;
	}

	if(command_line.substr(0,4) == "dist")
	{
		std::cout << std::endl;
		print_dist(std::cout, cnu.dist, cnu.n);
		std::cout << std::endl;
		return;
	}

	if(command_line.substr(0,3) == "cnu")
	{
		std::cout << std::endl;
		print_cnu_details(std::cout, cnu);
		std::cout << std::endl;
		return;
	}
}

int main(int argc, char** argv)
{
	try {
		if(argc != 3)
			throw std::logic_error("Wrong number of arguments.");

		std::ifstream cnu_file(argv[1]);
		std::ifstream dist_file(argv[2]);

		auto cnu = read_cnu(cnu_file, dist_file);

		std::string line;
		while(true) 
		{
			std::getline(std::cin, line);
			do_command(line, cnu);
		}
	}
	catch(const std::logic_error& e)
	{
		std::cout << "FATAL ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
