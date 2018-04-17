#include "operations.h"

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
