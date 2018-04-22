#include <iostream>
#include <fstream>

#include "gbn/gbn.h"
#include "gbn/gbn_io.h"
#include "gbn/gbn_check.h"

int main(int argc, char** argv)
{
	// try {
		if(argc != 3)
			throw std::logic_error("Wrong number of arguments.");

		std::ifstream gbn_file(argv[1]);

		auto gbn = read_gbn(gbn_file);
		check_gbn_integrity(gbn);
	
		std::ofstream out_file(argv[2]);
		draw_gbn_graph(out_file, gbn);
	// }
	// catch(const std::logic_error& e)
	// {
		// std::cout << "FATAL ERROR: " << e.what() << std::endl;
		// return 1;
	// }

	return 0;
}
