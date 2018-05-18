#include <iostream>
#include <fstream>

#include "cn/cn.h"
#include "cn/cn_io.h"
#include "cn/cn_operations.h"
#include "joint_dist/joint_dist.h"
#include "joint_dist/joint_dist_io.h"
#include "cnu/operations_on_joint_dist.h"
#include "cnu/fire_transition.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

void do_command(std::string command_line, CN& cn, JointDist& dist, std::function<void(std::string, std::string)> status_callback = std::function<void(std::string, std::string)>())
{
	boost::trim(command_line);
	if(command_line[0] == 't')
	{
		std::vector<std::string> split_command_line;
		boost::split(split_command_line,command_line,boost::is_any_of(" "));

		auto i_transition = std::stoi(split_command_line[0].substr(1));
		fire_transition_on_joint_dist(cn, dist, i_transition, status_callback);

		return;
	}

	if(command_line.substr(0,4) == "dist")
	{
		print_dist(std::cout, dist);
		return;
	}

	if(command_line.substr(0,3) == "cn")
	{
		print_cn_details(std::cout, cn);
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

		std::function<void(std::string, std::string)> callback = [] (std::string high_level_op_string, std::string low_level_op_string) {
			std::cout << high_level_op_string << " " << low_level_op_string << std::endl;
		};

		std::string line;
		while(true) 
		{
			std::getline(std::cin, line);
			do_command(line, cn, dist, callback);
			std::cout << std::endl << std::endl;
		}
	}
	catch(const std::logic_error& e)
	{
		std::cout << "FATAL ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
