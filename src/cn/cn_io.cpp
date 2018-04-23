#include "cn_io.h"

#include <iomanip>
#include <string>
#include <boost/algorithm/string_regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <regex>
#include <cmath>

#include "../helpers.hpp"

CN read_cn(std::istream& istr)
{
	CN cn;
	std::string line;

	// read in number of places
	std::getline(istr, line);
	boost::trim(line);
	if(line.empty())
		throw std::logic_error("Wrong format: n");
	std::stringstream ss1(line);
	ss1 >> cn.n;
	
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
			if(place >= cn.n || place < 0)
				throw std::logic_error("Marking contains place that is larger than n (or lower than zero).");
			cn.m.set(place);
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
				if(tmp >= cn.n || tmp < 0)
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
				if(tmp >= cn.n || tmp < 0)
					throw std::logic_error("Transition contains place that is larger than n (or lower than zero).");
				t.post.push_back(tmp);
			}
		}
		cn.transitions.push_back(t);
	}

	return cn;
}


std::ostream& print_cn_details(std::ostream& ostr, const CN& cn)
{
	ostr << "----------------------------------------\n";
	ostr << "n: " << cn.n << "\n";
	ostr << "m: ";
	print_bitvec(ostr, cn.m, cn.n);
	ostr << "\n";
	ostr << "----------------------------------------\n";
	ostr << "transitions: " << "\n";
	std::size_t i = 0;
	for(auto t : cn.transitions)
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

	ostr.flush();

	return ostr;
}


