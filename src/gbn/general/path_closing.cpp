#include "path_closing.h"

#include <boost/graph/topological_sort.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <queue>
#include <iostream>

namespace {
	template<typename T>
		inline bool is_in(const T& val, const std::set<T>& set)
		{
			return set.count(val) != 0;
		}
}

// using Queue = std::queue<std::tuple<Vertex,typename GBNGraph::out_edge_iterator,typename GBNGraph::out_edge_iterator>>;
using Queue = std::list<Vertex>;

void recursive_traversal(
	Queue q, 
	GBN& gbn, 
	const std::map<Vertex,Index>& v_to_topo_pos, 
	const Index topo_max, 
	const std::set<Vertex>& vertex_set, 
	std::set<Vertex>& additional_vertices, 
	std::set<Vertex>& vertices_still_to_reach
)
{
	auto& g = gbn.graph;
	if(q.empty())
		return;

	auto& v = q.back();

	if(is_in(v,vertex_set))
	{
		vertices_still_to_reach.erase(v);
		for(auto v_path : q)
			if(!is_in(v_path, vertex_set))
				additional_vertices.insert(v_path);
	}

	for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
	{
		auto v_to = boost::target(e,g);
		if(v_to_topo_pos.at(v_to) <= topo_max)
		{
			Queue q_copy = q;
			q_copy.push_back(v_to);
			recursive_traversal(q_copy, gbn, v_to_topo_pos, topo_max, vertex_set, additional_vertices, vertices_still_to_reach);
			q_copy.pop_back();
		}
	}
}

std::vector<Vertex> path_closing(GBN& gbn, const std::vector<Vertex> vertices)
{
	auto& g = gbn.graph;

	// prepare topological ordering
	std::vector<Vertex> topo_order;	
	boost::topological_sort(g, std::back_inserter(topo_order));
	// for(auto v : topo_order)
		// std::cout << name(v,g) << std::endl;

	std::map<Vertex,Index> v_to_topo_pos;
	{
		Index i = 0;
		for(auto v : boost::adaptors::reverse(topo_order))
			v_to_topo_pos.insert({ v, i++ });
	}
	// for(auto t : v_to_topo_pos)
		// std::cout << name(t.first,g) << " " << t.second << std::endl;
	std::set<Vertex> vertex_set(vertices.begin(), vertices.end());

	// order vertices in topological order
	std::vector<std::pair<Vertex, std::size_t>> ordered_vertices;
	for(auto v : vertices)
		ordered_vertices.push_back({ v, v_to_topo_pos[v] });
	std::sort(ordered_vertices.begin(), ordered_vertices.end(), [](const auto& p1, const auto& p2) {
		return p1.second < p2.second;
	});
	Index topo_max = ordered_vertices.back().second;
	// for(auto t : ordered_vertices)
		// std::cout << name(t.first,g) << " " << t.second << std::endl;

	// std::cout << "topo_max: " << topo_max << std::endl;

	std::set<Vertex> vertices_still_to_reach(vertices.begin(), vertices.end());
	std::set<Vertex> additional_vertices;

	while(!vertices_still_to_reach.empty())
	{
		auto it = std::max_element(vertices_still_to_reach.begin(), vertices_still_to_reach.end(), [&v_to_topo_pos] (const auto& v1, const auto v2) {
			return v_to_topo_pos[v1] < v_to_topo_pos[v2];
		});
		auto v = *it;
		// std::cout << "max_element: " << v << std::endl;
		Queue q{v};
		recursive_traversal(q, gbn, v_to_topo_pos, topo_max, vertex_set, additional_vertices, vertices_still_to_reach);
	}

	vertex_set.insert(additional_vertices.begin(), additional_vertices.end());

	return std::vector<Vertex>(vertex_set.begin(), vertex_set.end());
}
