#pragma once

#include "matrix.h"
#include <iostream>
#include <vector>
#include <memory>

std::unique_ptr<Matrix> read_matrix(std::vector<std::string> lines);
void print_matrix(std::ostream& ostr, const Matrix& matrix);
