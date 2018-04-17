// Next TODOs:
// - make reading in prob dist possible
// - check if dist
// - check if net is proper 

#include <iostream>
#include <bitset>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <fstream>
#include <string>
#include <boost/algorithm/string_regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <regex>
#include <cmath>

constexpr int MAX_PLACES = 64;

using Marking = std::bitset<MAX_PLACES>;
using JointDist = std::unordered_map<Marking, double>;

struct Transition {
	std::vector<int> pre;
	std::vector<int> post;
};

struct CNU {
	int n; // number of places
	Marking m; // (initial) marking
	std::vector<Transition> transitions; 
	JointDist dist; // (initial) dist
};

// void set_op(const std::vector<int> places, JointDist& dist)
// {
	// for(auto& [marking,p] : places)
	// {
		// bool all_set = true;
		// for(auto& s : places)
			// if(!marking.test(s)) {
				// all_set = false;
				// break;
			// }

		// if(!all_set)
		// {
			// auto target
		// }
	// }
// }

std::ostream& print_marking(std::ostream& ostr, const Marking& m, const int n)
{
	ostr << m.to_string().substr(MAX_PLACES-n);	

	return ostr;
}

void set_op(const std::vector<int> places, JointDist& dist)
{
	Marking mask;
	for(auto& s : places)
		mask.set(s);

	for(auto& [marking,p] : dist)
	{
		auto target = marking ^ mask;
		if(target != marking)
		{
			dist[target] += p;
			p = 0;
		}
	}
}

bool is_valid_joint_dist(const JointDist& dist)
{
	double sum = 0;
	for(auto p : dist)
		sum += p.second;

	return std::abs(1-sum) < 1e-8;
}

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

		double p = 0;
		if(p_str.find_first_of("/") != std::string::npos)
		{
			// fraction
			std::vector<std::string> strs;
			boost::split(strs,p_str,boost::is_any_of("/"));
			p = static_cast<double>(std::stod(strs[0])) / std::stod(strs[1]);
		}
		else
		{
			// floting point number
			p = std::stod(p_str);	
		}

		Marking m(marking_str);
		auto r = dist.insert({ m, p });
		if(!r.second)
			throw std::logic_error(std::string("Joint distribution could not be read. Marking ") + marking_str + "defined multiple times.");
	}

	if(!is_valid_joint_dist(dist))
		throw std::logic_error("Joint distribution does not add up to 1.");

	return dist;
}

CNU read_cnu(std::istream& istr, std::istream& istr_dist)
{
	CNU cnu;
	std::string line;

	// read in number of places
	std::getline(istr, line);
	boost::trim(line);
	if(line.empty())
		throw std::logic_error("Wrong format: n");
	std::stringstream ss1(line);
	ss1 >> cnu.n;
	
	// read in initial marking
	std::getline(istr, line);
	boost::trim(line);
	if(line.empty())
		throw std::logic_error("Wrong format: initial marking");
	std::stringstream ss2(line);
	while(!ss2.eof())
	{
		int place = -1;
		ss2 >> place;
		if(place != -1)
		{
			if(place >= cnu.n || place < 0)
				throw std::logic_error("Marking contains place that is larger than n (or lower than zero).");
			cnu.m.set(place);
		}
	}
	
	// read transitions
	while(!istr.eof())
	{
		Transition t;
		std::getline(istr, line);
		boost::trim(line);
		if(line.empty())
			continue;

		boost::regex split_regex("->");
		std::vector<std::string> res;
		boost::split_regex(res, line, split_regex);

		// read in pre
		std::stringstream ss4(res.at(0));
		while(!ss4.eof())
		{
			int tmp = -1;
			ss4 >> tmp;
			if(tmp != -1)
			{
				if(tmp >= cnu.n || tmp < 0)
					throw std::logic_error("Transition contains place that is larger than n (or lower than zero).");

				t.pre.push_back(tmp);
			}
		}

		// read in post
		std::stringstream ss5(res.at(1));
		while(!ss5.eof())
		{
			int tmp = -1;
			ss5 >> tmp;
			if(tmp != -1)
			{
				if(tmp >= cnu.n || tmp < 0)
					throw std::logic_error("Transition contains place that is larger than n (or lower than zero).");
				t.post.push_back(tmp);
			}
		}
		cnu.transitions.push_back(t);
	}

	cnu.dist = read_joint_dist(istr_dist);

	return cnu;
}

std::ostream& print_cnu_details(std::ostream& ostr, const CNU& cnu)
{
	ostr << "----------------------------------------\n";
	ostr << "n: " << cnu.n << "\n";
	ostr << "m: ";
	print_marking(ostr, cnu.m, cnu.n);
	ostr << "\n";
	ostr << "----------------------------------------\n";
	ostr << "transitions: " << "\n";
	std::size_t i = 0;
	for(auto t : cnu.transitions)
	{
		ostr << "t" << i << ": ";
		for(auto p : t.pre)
			ostr << p << " ";

		ostr << "-> ";

		for(auto p : t.post)
			ostr << p << " ";

		ostr << "\n";
		i++;
	}

	ostr << "----------------------------------------\n";
	ostr << "distribution: \n";
	std::vector<std::pair<Marking, double>> tmp(cnu.dist.begin(), cnu.dist.end());
	std::sort(tmp.begin(), tmp.end(), [](const auto& p1, const auto& p2) {
		return p1.first.to_string() < p2.first.to_string();
	});

	for(auto p : tmp)
	{
		print_marking(ostr, p.first, cnu.n);
		ostr << " " << p.second << "\n";
	}

	ostr << "----------------------------------------\n";

	ostr.flush();

	return ostr;
}

int main(int argc, char** argv)
{
	try {
		if(argc != 3)
			throw std::logic_error("Wrong number of arguments.");

		std::ifstream cnu_file(argv[1]);
		std::ifstream dist_file(argv[2]);

		auto cnu = read_cnu(cnu_file, dist_file);

		print_cnu_details(std::cout, cnu);

	}
	catch(const std::logic_error& e)
	{
		std::cout << "FATAL ERROR: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
