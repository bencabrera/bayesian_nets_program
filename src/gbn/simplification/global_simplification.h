#pragma once

#include "../general/gbn.h"

bool eliminate_stochastic_vertex_without_outputs(GBN& gbn, Vertex v, std::string& op);
bool switch_substoch_to_front(GBN& gbn, Vertex v, std::string& op);
bool normalize_substoch_front_vertices_without_inputs(GBN& gbn, Vertex v, std::string& op);
