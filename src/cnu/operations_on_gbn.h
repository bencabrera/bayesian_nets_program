#pragma once

#include "../gbn/general/gbn.h"

void set_op(const std::vector<int> places, bool b, GBN& gbn);
void assert_op(const std::vector<int> places, bool b, GBN& gbn);
void nassert_op(const std::vector<int> places, bool b, GBN& gbn);
