#pragma once

#include "gbn.h"

Vertex replace_nodes_by_matrix(GBN& gbn, std::vector<Vertex> vertices, std::string new_node_label = "new");
