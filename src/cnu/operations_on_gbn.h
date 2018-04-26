#pragma once

#include "../gbn/general/gbn.h"

void set_op(const std::vector<std::size_t> places, bool b, GBN& gbn);
void assert_op(const std::vector<std::size_t> places, bool b, GBN& gbn);
void nassert_op(const std::vector<std::size_t> places, bool b, GBN& gbn);
