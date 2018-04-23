#pragma once

#include "../general/gbn.h"

Vertex merge_vertices(GBN& gbn, std::vector<Vertex> vertices, std::string new_node_label = "new");
