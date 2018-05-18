#include "fire_transition.h"

#include "../cn/cn_operations.h"
#include "../cnu/operations_on_gbn.h"
#include "../cnu/operations_on_joint_dist.h"
#include "../gbn/general/check.h"
#include <iostream>

void fire_transition_on_gbn(CN& cn, GBN& gbn, std::size_t i_transition, std::function<void(std::string,std::string)> status_callback)
{
	const auto& transition = cn.transitions[i_transition];
	const auto& pre_places = transition.pre;
	const auto& post_places = transition.post;

	// fail_pre case
	if(!check_pre_condition(transition,cn.m))
	{
		nassert_op(transition.pre, 1, gbn);
		check_gbn_integrity(gbn);

		if(status_callback)
			status_callback(std::string("fail-pre_{t")+std::to_string(i_transition)+"}", std::string("nassert"));
		return;
	}

	// fail_post case
	if(!check_post_condition(transition,cn.m))
	{
		nassert_op(transition.post, 0, gbn);
		check_gbn_integrity(gbn);

		if(status_callback)
			status_callback(std::string("fail-post_{t")+std::to_string(i_transition)+"}", std::string("nassert"));
		return;
	}

	assert_op(pre_places,1, gbn);
	check_gbn_integrity(gbn);
	if(status_callback)
		status_callback(std::string("success_{t")+std::to_string(i_transition)+"}", std::string("assert_pre"));

	assert_op(post_places,0, gbn);
	check_gbn_integrity(gbn);
	if(status_callback)
		status_callback(std::string("success_{t")+std::to_string(i_transition)+"}", std::string("assert_post"));

	set_op(pre_places,0,gbn);
	check_gbn_integrity(gbn);
	if(status_callback)
		status_callback(std::string("success_{t")+std::to_string(i_transition)+"}", std::string("set_pre"));

	set_op(post_places,1,gbn);
	check_gbn_integrity(gbn);
	if(status_callback)
		status_callback(std::string("success_{t")+std::to_string(i_transition)+"}", std::string("set_post"));

	// update marking in CNU
	for(const auto& p : transition.pre)
		cn.m.at(p) = 0;
	for(const auto& p : transition.post)
		cn.m.at(p) = 1;
}

void fire_transition_on_joint_dist(CN& cn, JointDist& dist, std::size_t i_transition, std::function<void(std::string,std::string)> status_callback)
{
	const auto& transition = cn.transitions[i_transition];
	const auto& pre_places = transition.pre;
	const auto& post_places = transition.post;

	// fail_pre case
	if(!check_pre_condition(transition,cn.m))
	{
		nassert_op(transition.pre, 1, dist);

		if(status_callback)
			status_callback(std::string("fail-pre_{t")+std::to_string(i_transition)+"}", std::string("nassert"));
		return;
	}

	// fail_post case
	if(!check_post_condition(transition,cn.m))
	{
		nassert_op(transition.post, 0, dist);

		if(status_callback)
			status_callback(std::string("fail-post_{t")+std::to_string(i_transition)+"}", std::string("nassert"));
		return;
	}

	assert_op(pre_places,1, dist);
	if(status_callback)
		status_callback(std::string("success_{t")+std::to_string(i_transition)+"}", std::string("assert_pre"));

	assert_op(post_places,0, dist);
	if(status_callback)
		status_callback(std::string("success_{t")+std::to_string(i_transition)+"}", std::string("assert_post"));

	set_op(pre_places,0,dist);
	if(status_callback)
		status_callback(std::string("success_{t")+std::to_string(i_transition)+"}", std::string("set_pre"));

	set_op(post_places,1,dist);
	if(status_callback)
		status_callback(std::string("success_{t")+std::to_string(i_transition)+"}", std::string("set_post"));

	// update marking in CNU
	for(const auto& p : transition.pre)
		cn.m[p] = 0;
	for(const auto& p : transition.post)
		cn.m[p] = 1;
}
