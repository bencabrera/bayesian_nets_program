#include "gbn.h"

GBN::GBN(Index n, Index m, Index n_inside_vertices)
	:n(n), m(m), n_vertices(n_inside_vertices + n + m), graph(n_vertices)
{
	for(Index i = 0; i < n_vertices; i++)
		visible_vertices.insert(i);

	for(Index i = 0; i < n; i++)
		input_vertices.push_back(n_inside_vertices+i);
	for(Index i = 0; i < m; i++)
		output_vertices.push_back(n_inside_vertices+n+i);
}

Index n(const GBNGraph::vertex_descriptor& v, const GBNGraph& g)
{
	return get(vertex_matrix, g, v)->n;
}

Index m(const GBNGraph::vertex_descriptor& v, const GBNGraph& g)
{
	return get(vertex_matrix, g, v)->m;
}


Index port_from(const GBNGraph::edge_descriptor& e, const GBNGraph& g)
{
	return get(edge_position, g, e).first;
}

Index port_to(const GBNGraph::edge_descriptor& e, const GBNGraph& g)
{
	return get(edge_position, g, e).second;
}

const std::string& name(const GBNGraph::vertex_descriptor& v, const GBNGraph& g)
{
	return get(vertex_name, g, v);
}

VertexType type(const GBNGraph::vertex_descriptor& v, const GBNGraph& g)
{
	return get(vertex_type, g, v);
}

MatrixPtr matrix(const GBNGraph::vertex_descriptor& v, const GBNGraph& g)
{
	return get(vertex_matrix, g, v);
}




std::vector<Vertex> all_vertices(const GBN& gbn)
{
	return std::vector<Vertex>(gbn.visible_vertices.begin(), gbn.visible_vertices.end());
}

std::vector<Vertex> inside_vertices(const GBN& gbn)
{
	std::vector<Vertex> rtn;
	std::copy_if(gbn.visible_vertices.begin(), gbn.visible_vertices.end(), std::back_inserter(rtn), [&gbn](const Vertex& v) {
		return type(v,gbn.graph) == NODE;
	});

	return rtn;
}

const std::vector<Vertex>& input_vertices(const GBN& gbn)
{
	return gbn.input_vertices;
}

const std::vector<Vertex>& output_vertices(const GBN& gbn)
{
	return gbn.output_vertices;
}
