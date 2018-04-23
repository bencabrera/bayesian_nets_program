#include "bitvec.h"

std::ostream& print_bitvec(std::ostream& ostr, const BitVec& m, const int n)
{
	ostr << m.to_string().substr(MAX_PLACES-n);	

	return ostr;
}
