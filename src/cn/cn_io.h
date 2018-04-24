#pragma once

#include "cn.h"

#include <iostream>

CN read_cn(std::istream& istr);
std::ostream& print_cn_details(std::ostream& ostr, const CN& cn);
std::ostream& draw_cn(std::ostream& ostr, const CN& cn);
