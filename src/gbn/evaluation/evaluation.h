#pragma once

#include "../general/gbn.h"
#include "vertex_set_input_output.h"
#include "wire_structure.h"

std::vector<Vertex> flip_wire(Wire& wire);
MatrixPtr evaluate(const GBN& gbn, const std::vector<Vertex> vertices = {});
