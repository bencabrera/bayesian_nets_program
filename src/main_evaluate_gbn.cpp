#include <iostream>
#include <fstream>

#include "gbn/general/gbn.h"
#include "gbn/general/gbn_io.h"
#include "gbn/general/evaluation.h"
#include "gbn/general/check.h"
#include "gbn/matrix/matrix_io.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

int main(int argc, char** argv)
{
	if(argc != 3)
		throw std::logic_error("Wrong number of arguments.");

	std::ifstream gbn_file(argv[1]);

	std::string vertices_str(argv[2]);
	std::vector<std::string> vertex_strs;
	boost::split(vertex_strs, vertices_str, boost::is_any_of(","));
	std::vector<Vertex> vertices;
	for(auto v_str : vertex_strs)
		vertices.push_back(std::stoul(v_str));
		
	auto gbn = read_gbn(gbn_file);
	check_gbn_integrity(gbn);

	auto [p_m, vertex_set_input_outputs] = evaluate_vertices(gbn, vertices);
	print_matrix(std::cout, *p_m);

	return 0;
}
