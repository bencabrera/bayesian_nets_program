#include "matrix_io.h"

#include <regex>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cmath>
#include <iomanip>

#include "../../helpers.hpp"

MatrixPtr read_matrix(std::vector<std::string> lines)
{
	if(lines.empty())
		throw std::logic_error("Reading in matrix failed because lines vector is empty.");

	std::regex dynamic_regex("^dynamic ([0-9]+) ([0-9]+) ?\\[?([0-9,;\\/]*)\\]?", std::regex_constants::icase);
	std::regex f_regex("^F_\\{?([0-9]+),([0-9]+)\\}?", std::regex_constants::icase);
	std::regex one_b_regex("^1_\\{?([0-9])\\}?", std::regex_constants::icase);
	std::regex terminator_regex("^T", std::regex_constants::icase);

	// dynamic matrix
	std::smatch matches;
	if(std::regex_match(lines[0], matches, dynamic_regex))
	{
		int n = std::stoi(matches[1]);
		int m = std::stoi(matches[2]);

		if(n > 63 || m > 63)
			throw std::logic_error("Dimensions of matrix too big.");

		std::string matrix_str = matches[3];

		std::unique_ptr<DynamicMatrix> rtn(new DynamicMatrix(n,m));
		auto& matrix = *rtn;
		// [1,2,3; 4,5,6; 7,8,9] format
		if(!matrix_str.empty())
		{
			boost::trim(matrix_str);

			std::vector<std::string> row_strs;
			boost::split(row_strs, matrix_str, boost::is_any_of(";"));
			if(row_strs.size() != (1 << m))
				throw std::logic_error(std::string("Rows: Provided array") + std::to_string(row_strs.size()) + " does not have right dimension " + std::to_string((1 << m)));

			unsigned long long i_row = 0;
			for(auto row_str : row_strs)
			{
				boost::trim_if(row_str,boost::is_any_of(" ,"));

				std::vector<std::string> value_strs;
				boost::split(value_strs, row_str, boost::is_any_of(","));

				if(value_strs.size() != (1 << n))
					throw std::logic_error(std::string("Cols: Provided array") + std::to_string(value_strs.size()) + " does not have right dimension " + std::to_string((1 << n)));

				unsigned long long i_col = 0;
				for(auto val : value_strs)
					matrix.set(i_row, i_col++, read_double(val));

				i_row++;
			}

		}

		if(!is_stochastic(*rtn))
			rtn->is_stochastic = false;

		return rtn;
	}

	// F matrix
	if(std::regex_match(lines[0], matches, f_regex))
	{
		int k = std::stoi(matches[1].str());
		int b = std::stoi(matches[2].str());

		return MatrixPtr(new FMatrix(k,b));
	}

	// One b matrix
	if(std::regex_match(lines[0], matches, one_b_regex))
	{
		int b = std::stoi(matches[1].str());

		return MatrixPtr(new OneBMatrix(b));
	}

	// terminator matrix
	if(std::regex_match(lines[0], matches, terminator_regex))
	{
		return MatrixPtr(new TerminatorMatrix());
	}

	throw std::logic_error(std::string("Matrix could not be read. Not able to parse line")+ "'" + lines[0] + "'.");
}

void print_matrix(std::ostream& ostr, const Matrix& matrix)
{
	ostr << "n: " << matrix.n << " m: " << matrix.m << std::endl;
	unsigned long long i_max_row = 1;
	unsigned long long i_max_col = 1;
	i_max_col = i_max_col << matrix.n;
	i_max_row = i_max_row << matrix.m;

	ostr.precision(4);


	ostr << std::setw(matrix.m+3) << " ";
	for(unsigned long long i_col = 0; i_col < i_max_col; i_col++)
		ostr << std::setw(6) << BitVec(i_col).to_string().substr(MAX_PLACES-matrix.n) << " ";	
	ostr << std::endl;

	ostr << std::setw(matrix.m+3) << " ";
	for(unsigned long long i_col = 0; i_col < i_max_col; i_col++)
		ostr << std::setw(5) << std::fixed << "------" << " ";	
	ostr << std::endl;

	for(unsigned long long i_row = 0; i_row < i_max_row; i_row++)
	{
		ostr << BitVec(i_row).to_string().substr(MAX_PLACES-matrix.m) << " | ";
		for(unsigned long long i_col = 0; i_col < i_max_col; i_col++)
		{
			ostr << std::setw(5) << std::fixed << matrix.get(i_row, i_col) << " ";	
		}
		ostr << "\n";	
	}

	ostr << std::endl;
}
