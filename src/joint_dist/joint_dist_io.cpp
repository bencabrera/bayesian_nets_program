#include "joint_dist_io.h"

#include <iomanip>
#include <string>
#include <boost/algorithm/string_regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <regex>
#include <cmath>
#include <algorithm>

#include "../helpers.hpp"
#include "joint_dist_check.h"

JointDist read_joint_dist(std::istream& istr)
{
	JointDist dist;
	std::string line;

	while(!istr.eof())
	{
		std::getline(istr, line);
		boost::trim(line);
		if(line.empty())
			continue;

		std::stringstream ss(line);
		std::string marking_str = "";
		std::string p_str;
		ss >> marking_str >> p_str;
		if(marking_str.empty() || p_str.empty())
			throw std::logic_error("Joint distribution could not be read. Wrong format.");

		double p = read_double(p_str);

		auto m = read_bool_vector(marking_str);
		std::reverse(m.begin(), m.end());
		auto r = dist.insert({ m, p });
		if(!r.second)
			throw std::logic_error(std::string("Joint distribution could not be read. Marking ") + marking_str + "defined multiple times.");
	}

	if(!is_valid_joint_dist(dist))
		throw std::logic_error("Joint distribution does not add up to 1.");

	return dist;
}


void print_dist(std::ostream& ostr, const JointDist& dist)
{
	for(auto p : dist)
	{
		print_bool_vector(ostr, p.first);
		ostr << " " << p.second << "\n";
	}
}
