
#include <iostream>
#include <fstream>

#include "gbn/general/gbn.h"
#include "gbn/general/gbn_io.h"
#include "gbn/general/check.h"
#include "gbn/evaluation/evaluation.h"
#include "gbn/matrix/matrix_io.h"
#include "gbn/modification/merging.h"
#include "gbn/general/subgbn.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

int main(int argc, char** argv)
{
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

	auto sub_gbn = SubGBN::make_from_vertices(gbn, vertices);

	std::ofstream f("subgbn.dot");
	draw_gbn_graph(f, sub_gbn.gbn);

	check_gbn_integrity(sub_gbn.gbn);

	return 0;
}
