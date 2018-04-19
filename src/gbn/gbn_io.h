#pragma once

#include "gbn.h"
#include <iostream>
#include <vector>

GBN read_gbn(std::istream& istr);
void print_gbn(std::ostream& ostr, const GBN& gbn);
