#include "gbn.h"

GBN::GBN(Index n, Index m, Index n_inside_vertices)
	:n(n), m(m), n_vertices(n_inside_vertices + n + m), graph(n_vertices), n_initial_inside_vertices(n_inside_vertices), n_initial_n(n), n_initial_m(m)
{
	for(Index i = 0; i < n_vertices; i++)
	{
		put(vertex_type, graph, i, NODE);
		visible_vertices.insert(i);
	}

	for(Index v = 0; v < n_inside_vertices; v++)
		put(vertex_name, graph, v, std::string("v_")+std::to_string(v));

	for(Index i = 0; i < n; i++)
	{
		put(vertex_type, graph, n_inside_vertices+i, INPUT);
		put(vertex_name, graph, n_inside_vertices+i, std::string("i_")+std::to_string(i));
		input_vertices.push_back(n_inside_vertices+i);
	}
	for(Index i = 0; i < m; i++)
	{
		put(vertex_type, graph, n_inside_vertices+n+i, OUTPUT);
		put(vertex_name, graph, n_inside_vertices+n+i, std::string("o_")+std::to_string(i));
		output_vertices.push_back(n_inside_vertices+n+i);
	}
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

Index input_idx(const Vertex& v, const GBN& gbn)
{
	return v-gbn.n_initial_inside_vertices;
}

Index output_idx(const Vertex& v, const GBN& gbn)
{
	return v-gbn.n_initial_inside_vertices-gbn.n_initial_n;
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
