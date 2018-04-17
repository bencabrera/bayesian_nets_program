// Next TODOs:
// - implement and check set, assert and nassert operations

#include <iostream>
#include <fstream>

#include "cnu.h"
#include "io.h"
#include "operations.h"
#include "deterministic_operations.h"

void do_command(const std::string command_line, CNU& cnu)
{
	if(command_line[0] == 't')
	{
		auto i_transition = std::stoi(command_line.substr(1));
		const auto& transition = cnu.transitions[i_transition];

		// fail_pre case
		if(!check_pre_condition(transition,cnu.m))
		{
			fail_pre_op(transition.pre, transition.post, cnu.dist);
			return;
		}

		// fail_post case
		if(!check_post_condition(transition,cnu.m))
		{
			fail_post_op(transition.pre, transition.post, cnu.dist);
			return;
		}

		success_op(transition.pre, transition.post, cnu.dist);
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
