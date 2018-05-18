#pragma once

#include "../general/gbn.h"

bool check_and_apply_F1(GBN& gbn, Vertex v, std::string& op);
bool check_and_apply_F2(GBN& gbn, Vertex v, std::string& op);
bool check_and_apply_CoUnit(GBN& gbn, Vertex v, std::string& op);
bool check_and_apply_F3(GBN& gbn, Vertex v, std::string& op);
bool check_and_apply_F4(GBN& gbn, Vertex v, std::string& op);
bool check_and_apply_F5(GBN& gbn, Vertex v, std::string& op);

bool split_vertex_if_multiple_outputs(GBN& gbn, Vertex v, std::string& op);
bool simplify_matrix_for_duplicate_inputs(GBN& gbn, Vertex v, std::string& op);
