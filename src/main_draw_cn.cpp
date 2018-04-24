#include <iostream>
#include <fstream>

#include "cn/cn_io.h"

int main(int argc, char** argv)
{
	if(argc != 3)
		throw std::logic_error("Wrong number of arguments.");

	std::ifstream cn_file(argv[1]);

	auto gbn = read_cn(cn_file);

	std::ofstream out_file(argv[2]);
	draw_cn(out_file, gbn);

	return 0;
}
