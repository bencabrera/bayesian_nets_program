#pragma once

#include "../general/gbn.h"
#include "wire_structure.h"

std::vector<Vertex> flip_wire(Wire& wire);
MatrixPtr evaluate(const GBN& gbn);
