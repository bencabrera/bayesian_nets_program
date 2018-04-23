#pragma once

#include "gbn.h"

std::pair<Vertex, Vertex> node_splitting(GBN& gbn, Vertex v);
void recursive_node_splitting(GBN& gbn, Vertex v);
