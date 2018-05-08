#include <iostream>
#include <fstream>

#include "gbn/general/gbn.h"
#include "gbn/general/gbn_io.h"
#include "gbn/general/check.h"
#include "gbn/evaluation/evaluation.h"
#include "gbn/matrix/matrix_io.h"
#include "gbn/modification/merging.h"
#include "gbn/modification/splitting.h"
#include "gbn/modification/simplification.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

int main(int argc, char** argv)
{
	if(argc != 2)
		throw std::logic_error("Wrong number of arguments.");

	std::ifstream gbn_file(argv[1]);
	auto gbn = read_gbn(gbn_file);
	check_gbn_integrity(gbn);

	auto m_before = evaluate(gbn);
	print_matrix(std::cout, *m_before);

	std::ofstream out_file1("before.dot");
	draw_gbn_graph(out_file1, gbn);

	gbn_simplification(gbn);
	check_gbn_integrity(gbn);

	std::ofstream out_file2("after.dot");
	draw_gbn_graph(out_file2, gbn);



	auto m_after = evaluate(gbn);
	print_matrix(std::cout, *m_after);

	return 0;
}
