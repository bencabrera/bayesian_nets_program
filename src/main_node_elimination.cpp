#include <iostream>
#include <fstream>

#include "gbn/gbn.h"
#include "gbn/gbn_io.h"
#include "gbn/gbn_check.h"
#include "gbn/gbn_evaluation.h"
#include "gbn/matrix_io.h"
#include "gbn/node_elimination.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

int main(int argc, char** argv)
{
	// try {
		if(argc != 3)
			throw std::logic_error("Wrong number of arguments.");

		std::ifstream gbn_file(argv[1]);

		std::string str(argv[2]);
		std::vector<std::string> vertices_strs;
		boost::split(vertices_strs, str, boost::is_any_of(","));
		std::vector<Vertex> vertices;
		for(auto v_str : vertices_strs)
			vertices.push_back(std::stoul(v_str));

		auto gbn = read_gbn(gbn_file);
		check_gbn_integrity(gbn);

		std::ofstream out_file1("before.dot");
		draw_gbn_graph(out_file1, gbn);

		replace_nodes_by_matrix(gbn, vertices);
		// print_matrix(std::cout, *m);

		std::ofstream out_file2("after.dot");
		draw_gbn_graph(out_file2, gbn);

	// }
	// catch(const std::logic_error& e)
	// {
		// std::cout << "FATAL ERROR: " << e.what() << std::endl;
		// return 1;
	// }

	return 0;
}
