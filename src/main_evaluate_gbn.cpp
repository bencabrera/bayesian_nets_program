#include <iostream>
#include <fstream>

#include "gbn/general/gbn.h"
#include "gbn/general/gbn_io.h"
#include "gbn/general/evaluation.h"
#include "gbn/general/check.h"
#include "gbn/matrix/matrix_io.h"

int main(int argc, char** argv)
{
	if(argc != 2)
		throw std::logic_error("Wrong number of arguments.");

	std::ifstream gbn_file(argv[1]);

	auto gbn = read_gbn(gbn_file);
	check_gbn_integrity(gbn);

	auto m = evaluate_gbn(gbn, {});
	print_matrix(std::cout, *m);

	return 0;
}
