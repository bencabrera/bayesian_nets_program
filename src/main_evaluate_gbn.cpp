#include <iostream>
#include <fstream>

#include "gbn/gbn.h"
#include "gbn/gbn_io.h"
#include "gbn/gbn_evaluation.h"
#include "gbn/gbn_check.h"
#include "gbn/matrix_io.h"

int main(int argc, char** argv)
{
	// try {
		if(argc != 2)
			throw std::logic_error("Wrong number of arguments.");

		std::ifstream gbn_file(argv[1]);

		auto gbn = read_gbn(gbn_file);
		check_gbn_integrity(gbn);

		// std::vector<Vertex> vertices_vec;
		// for(auto v : vertices(gbn))
		// {
			// if(type(v,gbn.graph) == NODE)
				// vertices_vec.push_back(v);
		// }
	
		auto m = evaluate_gbn(gbn, {});
		print_matrix(std::cout, *m);

	// }
	// catch(const std::logic_error& e)
	// {
		// std::cout << "FATAL ERROR: " << e.what() << std::endl;
		// return 1;
	// }

	return 0;
}
