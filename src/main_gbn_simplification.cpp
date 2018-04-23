#include <iostream>
#include <fstream>

#include "gbn/gbn.h"
#include "gbn/gbn_io.h"
#include "gbn/gbn_check.h"
#include "gbn/gbn_evaluation.h"
#include "gbn/matrix_io.h"
#include "gbn/node_elimination.h"
#include "gbn/node_splitting.h"
#include "gbn/gbn_simplification.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

int main(int argc, char** argv)
{
	if(argc != 2)
		throw std::logic_error("Wrong number of arguments.");

	std::ifstream gbn_file(argv[1]);
	auto gbn = read_gbn(gbn_file);
	check_gbn_integrity(gbn);

	auto m_before = evaluate_gbn(gbn);
	print_matrix(std::cout, *m_before);

	std::ofstream out_file1("before.dot");
	draw_gbn_graph(out_file1, gbn);

	gbn_simplification(gbn);

	check_gbn_integrity(gbn);

	auto m_after = evaluate_gbn(gbn);
	print_matrix(std::cout, *m_after);

	std::ofstream out_file2("after.dot");
	draw_gbn_graph(out_file2, gbn);

	return 0;
}
