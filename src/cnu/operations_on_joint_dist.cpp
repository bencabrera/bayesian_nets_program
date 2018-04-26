#include "operations_on_joint_dist.h"
#include <iostream>
#include "../cn/cn.h"
#include "../cn/cn_io.h"
#include "../helpers.hpp"

namespace {
	std::vector<bool> build_target_marking(const std::vector<bool>& m, std::vector<std::size_t> places, bool b)
	{
		auto vec = m;
		for(auto p : places)
			vec[p] = b;

		return vec;
	}

	bool is_equal(const std::vector<bool>& m1, const std::vector<bool>& m2)
	{
		if(m1.size() != m2.size())
			return false;
		for(std::size_t i = 0; i < m1.size(); i++)
			if(m1[i] != m2[i])
				return false;

		return true;
	}
}

void set_op(const std::vector<std::size_t> places, bool b, JointDist& dist)
{
	for(auto& [marking,p] : dist)
	{
		auto target = build_target_marking(marking, places, b);
		if(target != marking)
		{
			dist[target] += p;
			p = 0;
		}
	}
}

void assert_op(const std::vector<std::size_t> places, bool b, JointDist& dist)
{
	double sum = 0;
	for(auto& [marking,p] : dist)
	{
		auto target = build_target_marking(marking, places, b);
		if(target != marking)
		{
			sum += p;
			p = 0;
		}
	}

	if(sum > 1e-40)
		for(auto& t : dist)
			t.second = t.second/(1-sum);
}

void nassert_op(const std::vector<std::size_t> places, bool b, JointDist& dist)
{
	double sum = 0;
	for(auto& [marking,p] : dist)
	{
		auto target = build_target_marking(marking, places, b);
		if(target == marking)
		{
			sum += p;
			p = 0;
		}
	}

	if(sum > 1e-40)
		for(auto& t : dist)
			t.second = t.second/(1-sum);
}
