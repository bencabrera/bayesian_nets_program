#include "operations.h"
#include <iostream>

void set_op(const std::vector<int> places, bool b, JointDist& dist)
{
	Marking mask;
	if(!b)
		mask.set(); // if b = 0 => set m = 1....1

	for(auto& s : places)
		mask.flip(s);

	for(auto& [marking,p] : dist)
	{
		auto target = b ? marking | mask : marking & mask;
		if(target != marking)
		{
			dist[target] += p;
			p = 0;
		}
	}
}

void assert_op(const std::vector<int> places, bool b, JointDist& dist)
{
	Marking mask;
	if(!b)
		mask.set(); // if b = 0 => set m = 1....1

	for(auto& s : places)
		mask.flip(s);

	double sum = 0;
	for(auto& [marking,p] : dist)
	{
		auto target = b ? marking | mask : marking & mask;
		if(target != marking)
		{
			sum += p;
			p = 0;
		}
	}

	for(auto& [marking,p] : dist)
		p = p/(1-sum);
}

void nassert_op(const std::vector<int> places, bool b, JointDist& dist)
{

}

void success_op(const std::vector<int> pre_places, const std::vector<int> post_places, bool b, JointDist& dist)
{

}
void fail_pre_op(const std::vector<int> pre_places, const std::vector<int> post_places, bool b, JointDist& dist)
{

}
void fail_post_op(const std::vector<int> pre_places, const std::vector<int> post_places, bool b, JointDist& dist)
{

}
