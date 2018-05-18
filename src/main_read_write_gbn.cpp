#include <iostream>
#include <fstream>

#include "gbn/general/gbn.h"
#include "gbn/general/gbn_io.h"
#include "gbn/general/check.h"

int main(int argc, char** argv)
{
	if(argc != 3)
		throw std::logic_error("Wrong number of arguments.");

	std::ifstream gbn_file(argv[1]);

	auto gbn = read_gbn(gbn_file);
	check_gbn_integrity(gbn);

	std::ofstream out_file(argv[2]);
	write_gbn(out_file, gbn);

	return 0;
}
