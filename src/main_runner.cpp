// Next TODOs:
// - implement and check set, assert and nassert operations

#include <iostream>
#include <fstream>

#include "cnu.h"
#include "io.h"
#include "operations.h"

int main(int argc, char** argv)
{
	try {
		if(argc != 3)
			throw std::logic_error("Wrong number of arguments.");

		std::ifstream cnu_file(argv[1]);
		std::ifstream dist_file(argv[2]);

		auto cnu = read_cnu(cnu_file, dist_file);

		print_cnu_details(std::cout, cnu);

	}
	catch(const std::logic_error& e)
	{
		std::cout << "FATAL ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
