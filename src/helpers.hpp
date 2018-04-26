#pragma once

#include <string>

#include <vector>
#include <sstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

inline double read_double(std::string str)
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

	return p;
}

inline bool prob_is_zero(double val)
{
	return val < 1e-40;
}

inline std::vector<bool> read_bool_vector(std::string str)
{
	std::vector<bool> vec;

	boost::trim(str);
	for(std::size_t i = 0; i < str.length(); i++)
	{
		if(str[i] == '1')
			vec.push_back(true);
		else if(str[i] == '0')
			vec.push_back(false);
		else 
			throw std::logic_error(str + " is not a bit string.");
	}

	return vec;
}

inline void print_bool_vector(std::ostream& ostr, const std::vector<bool>& vec)
{
	for(auto t : vec)
		ostr << static_cast<int>(t);
}


inline std::string to_string(const std::vector<bool>& vec)
{
	std::stringstream ss;
	print_bool_vector(ss, vec);
	return ss.str();
}
