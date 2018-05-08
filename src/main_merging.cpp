#include <iostream>
#include <fstream>

#include "gbn/general/gbn.h"
#include "gbn/general/gbn_io.h"
#include "gbn/general/check.h"
#include "gbn/evaluation/evaluation.h"
#include "gbn/matrix/matrix_io.h"
#include "gbn/modification/merging.h"
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

	auto m_before = evaluate_gbn(gbn);
	print_matrix(std::cout, *m_before);

	std::ofstream out_file1("before.dot");
	draw_gbn_graph(out_file1, gbn);

	merge_vertices(gbn, vertices, "A");

	check_gbn_integrity(gbn);

	auto m_after = evaluate_gbn(gbn);
	print_matrix(std::cout, *m_after);

	std::ofstream out_file2("after.dot");
	draw_gbn_graph(out_file2, gbn);

	return 0;
}
