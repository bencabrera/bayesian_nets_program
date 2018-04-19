#include "io_helper.h"

#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

double read_double(std::string str)
{
		double p = 0;
		if(str.find_first_of("/") != std::string::npos)
		{
			// fraction
			std::vector<std::string> strs;
			boost::split(strs,str,boost::is_any_of("/"));
			p = static_cast<double>(std::stod(strs[0])) / std::stod(strs[1]);
		}
		else
		{
			// floting point number
			p = std::stod(str);	
		}
}
