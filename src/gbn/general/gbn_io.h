#pragma once

#include "gbn.h"
#include <iostream>
#include <vector>

GBN read_gbn(std::istream& istr);
void write_gbn(std::ostream& ostr, const GBN& gbn);
void draw_gbn_graph(std::ostream& ostr, const GBN& gbn, std::string title = "", bool vertex_name_is_matrix_type = false);
