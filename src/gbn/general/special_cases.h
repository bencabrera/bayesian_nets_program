#pragma once

#include "gbn.h"

GBN build_uniform_independent_obn(std::size_t n_vertices);
GBN build_independent_obn(std::vector<std::pair<double,double>> rv_dists);
