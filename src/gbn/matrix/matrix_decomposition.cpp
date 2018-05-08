#include "matrix_decomposition.h"
#include "../../helpers.hpp"
#include <iostream>

// should take two bitvecs and append them: x, y => xy
BitVec bitvec_append(BitVec x, BitVec y, std::size_t y_length)
{
	BitVec xy = x;
	xy <<= y_length;
	xy |= y;

	return xy;
}

std::pair<MatrixPtr, MatrixPtr> matrix_decomposition(MatrixPtr p_matrix)
{
	auto& m = *p_matrix;
	if(m.m < 2)
		throw std::logic_error("Can not split matrix with m < 2");

	// build m_front
	MatrixPtr p_m_front = std::make_shared<DynamicMatrix>(m.n, m.m-1);
	auto& m_front = *p_m_front;
	unsigned long long i_max_row = 1;
	unsigned long long i_max_col = 1;
	i_max_col = i_max_col << m.n;
	i_max_row = i_max_row << (m.m-1);
	for(Index i_row = 0; i_row < i_max_row; i_row++)
		for(Index i_col = 0; i_col < i_max_col; i_col++)
			m_front.set(i_row, i_col, m.get(2*i_row, i_col) + m.get(2*i_row+1, i_col));

	// build m_back
	MatrixPtr p_m_back = std::make_shared<DynamicMatrix>(m.n+m.m-1, 1);
	auto& m_back = *p_m_back;
	unsigned long long x_max = 1, z_max = 1;
	x_max = x_max << (m.m-1);
	z_max = z_max << (m.n);
	for(Index x = 0; x < x_max; x++)
		for(Index z = 0; z < z_max; z++)
		{
			BitVec x_bitvec(x), z_bitvec(z);
			BitVec xz = bitvec_append(x,z,m.n);

			auto denom = m_front.get(x,z);

			if(prob_is_zero(denom))
			{
				m_back.set(0, xz, 0.5);
				m_back.set(1, xz, 0.5);
			}
			else
			{
				{
					BitVec y(0);
					BitVec xy = bitvec_append(x,y,1);
					m_back.set(0, xz, m.get(xy, z) / denom);
				}
				{
					BitVec y(1);
					BitVec xy = bitvec_append(x,y,1);
					m_back.set(1, xz, m.get(xy, z) / denom);
				}
			}
		}

	if(!p_matrix->is_stochastic)
		p_m_front->is_stochastic = false;

	return { p_m_front, p_m_back };
}
