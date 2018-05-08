#include <iostream>
#include <fstream>

#include "gbn/general/gbn.h"
#include "gbn/general/gbn_io.h"
#include "gbn/general/check.h"
#include "gbn/general/path_closing.h"
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

	std::ofstream out_file1("before.dot");
	draw_gbn_graph(out_file1, gbn);

	std::cout << "Unclosed: " << std::endl;
	for(auto v : vertices)
		std::cout << v << " ";
	std::cout << std::endl;

	auto closed_vertices = path_closing(gbn, vertices);

	std::cout << "Closed: " << std::endl;
	for(auto v : closed_vertices)
		std::cout << v << " ";
	std::cout << std::endl;

	return 0;
}
