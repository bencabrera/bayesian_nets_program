#pragma once

#include "cn.h"

#include <iostream>

CN read_cn(std::istream& istr);
void print_cn_details(std::ostream& ostr, const CN& cn);
void draw_cn(std::ostream& ostr, const CN& cn);
void export_cn(std::ostream& ostr, const CN& cn);
