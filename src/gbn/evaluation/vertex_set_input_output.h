#pragma once

#include "../general/gbn.h"

std::pair<std::vector<Port>,std::vector<Port>> build_inputs_outputs_for_vertices(const GBN& gbn, std::vector<Vertex> inside_vertices);
