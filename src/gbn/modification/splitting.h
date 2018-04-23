#pragma once

#include "../general/gbn.h"

std::pair<Vertex, Vertex> split_vertex(GBN& gbn, Vertex v);
void recursively_split_vertex(GBN& gbn, Vertex v);
