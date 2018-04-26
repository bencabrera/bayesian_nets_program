#pragma once

#include "../general/gbn.h"

// void gbn_eliminate_without_outputs(GBN& gbn);
void gbn_simplification(GBN& gbn);

bool check_and_apply_F1(GBN& gbn, Vertex v);
bool check_and_apply_F2(GBN& gbn, Vertex v);
bool check_and_apply_CoUnit(GBN& gbn, Vertex v);
bool check_and_apply_F3(GBN& gbn, Vertex v);
bool check_and_apply_F4(GBN& gbn, Vertex v);
bool check_and_apply_F5(GBN& gbn, Vertex v);

bool gbn_eliminate_without_outputs(GBN& gbn);
bool gbn_switch_substoch_to_front(GBN& gbn);
bool normalize_substoch_front_vertices(GBN& gbn);
