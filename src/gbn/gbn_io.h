#pragma once

#include "gbn.h"
#include <iostream>
#include <vector>

GBN read_gbn(std::istream& istr);
void draw_gbn_graph(std::ostream& ostr, const GBN& gbn);
