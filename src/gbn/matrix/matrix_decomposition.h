#pragma once

#include "matrix.h"

std::pair<MatrixPtr, MatrixPtr> matrix_decomposition(MatrixPtr matrix);

BitVec bitvec_append(BitVec x, BitVec y, std::size_t x_length);
