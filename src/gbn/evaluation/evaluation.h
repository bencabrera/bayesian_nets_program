#pragma once

#include "../general/gbn.h"
#include "vertex_set_input_output.h"
#include "wire_structure.h"

std::vector<Vertex> flip_wire(Wire& wire);
std::pair<MatrixPtr,VertexSetInputOutputs> evaluate_vertices(const GBN& gbn, const std::vector<Vertex> vertices);
MatrixPtr evaluate_gbn(const GBN& gbn);
