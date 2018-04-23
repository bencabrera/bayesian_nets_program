#include "vertex_add_remove.h"

Vertex add_vertex(GBN& gbn)
{
	Vertex v_new;
	if(gbn.hidden_vertices.size() > 0)
	{
		auto it = gbn.hidden_vertices.begin();
		v_new = *it;
		gbn.visible_vertices.insert(v_new);
		gbn.hidden_vertices.erase(it);
	}
	else
	{
		v_new = boost::add_vertex(gbn.graph);
		gbn.visible_vertices.insert(v_new);
	}

	gbn.n_vertices++;
	return v_new;
}

void remove_vertex(Vertex v, GBN& gbn)
{
	boost::clear_vertex(v, gbn.graph);

	auto it = gbn.visible_vertices.find(v);	
	if(it == gbn.visible_vertices.end())
		throw std::logic_error("Vertex was not found.");

	gbn.visible_vertices.erase(it);
	gbn.hidden_vertices.insert(v);
	gbn.n_vertices--;
}
