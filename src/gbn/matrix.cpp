#include "matrix.h"

// abstract Matrix class
Matrix::Matrix(const int n, const int m, const MatrixType type)
: n(n), m(m), type(type)//, one_mask_n(init_one_mask(n)), one_mask_m(init_one_mask(m))
{}


// DynamicMatrix
DynamicMatrix::DynamicMatrix(const int n, const int m): Matrix(n,m,DYNAMIC)
{}

double DynamicMatrix::get(const BitVec& to, const BitVec& from) const 
{
	auto it = data.find({ to, from });
	if(it == data.cend())
		return 0;
	else
		return it->second;
}

double& DynamicMatrix::set(const BitVec& to, const BitVec& from, double val) 
{
	data[{ to, from }] = val;
}



// FMatrix
FMatrix::FMatrix(const int n, const int m, const bool b): Matrix(n,m,F), k(n), b(b)
{
	if(n != m)
		throw std::logic_error("Tried to create F matrix where n != m");	
}

double FMatrix::get(const BitVec& to, const BitVec& from) const 
{
	if(to != from)
		return 0;

	if((b && to.all()) || (!b && to.none()))
		return 0;

	return 1;
}

double& FMatrix::set(const BitVec& /*to*/, const BitVec& /*from*/, double /*val*/) 
{
	throw std::logic_error("Tried to set entry of F-matrix.");
}



// OneBMatrix
OneBMatrix::OneBMatrix(const bool b): Matrix(0,1,ONE_B), b(b)
{}

double OneBMatrix::get(const BitVec& to, const BitVec& /*from*/) const 
{
	return (to.test(0) == b) ? 1 : 0;
}

double& OneBMatrix::set(const BitVec& /*to*/, const BitVec& /*from*/, double /*val*/) 
{
	throw std::logic_error("Tried to set entry of OneB-matrix.");
}
