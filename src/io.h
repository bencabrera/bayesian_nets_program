#pragma once

#include "cnu.h"

#include <iostream>

bool is_valid_joint_dist(const JointDist& dist);
std::ostream& print_marking(std::ostream& ostr, const Marking& m, const int n);
JointDist read_joint_dist(std::istream& istr);
CNU read_cnu(std::istream& istr, std::istream& istr_dist);
std::ostream& print_cnu_details(std::ostream& ostr, const CNU& cnu);
std::ostream& print_dist(std::ostream& ostr, const JointDist& dist, const int n = MAX_PLACES);
