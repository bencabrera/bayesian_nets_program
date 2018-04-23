#include "joint_dist_io.h"

#include <iomanip>
#include <string>
#include <boost/algorithm/string_regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <regex>
#include <cmath>

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

		BitVec m(marking_str);
		auto r = dist.insert({ m, p });
		if(!r.second)
			throw std::logic_error(std::string("Joint distribution could not be read. Marking ") + marking_str + "defined multiple times.");
	}

	if(!is_valid_joint_dist(dist))
		throw std::logic_error("Joint distribution does not add up to 1.");

	return dist;
}


std::ostream& print_dist(std::ostream& ostr, const JointDist& dist, const int n)
{
	std::vector<std::pair<BitVec, double>> tmp(dist.begin(), dist.end());
	std::sort(tmp.begin(), tmp.end(), [](const auto& p1, const auto& p2) {
		return p1.first.to_string() < p2.first.to_string();
	});

	for(auto p : tmp)
	{
		print_bitvec(ostr, p.first, n);
		ostr << " " << p.second << "\n";
	}
}
