#pragma once

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "../matrix/matrix.h"

using vertex_matrix_t = boost::vertex_rank_t;
using vertex_name_t = boost::vertex_name_t;
using vertex_type_t = boost::vertex_owner_t;
using edge_position_t = boost::edge_name_t;

const auto vertex_matrix = boost::vertex_rank;
const auto vertex_name = boost::vertex_name;
const auto vertex_type = boost::vertex_owner;
const auto edge_position = boost::edge_name;

enum VertexType {
	INPUT,
	NODE,
	OUTPUT
};

using VertexProperty = boost::property<vertex_matrix_t, MatrixPtr, boost::property<vertex_type_t, VertexType, boost::property<vertex_name_t,std::string>>>;
using EdgeProperty = boost::property<boost::edge_index_t, std::size_t, boost::property<edge_position_t,std::pair<std::size_t,std::size_t>>>;
using GBNGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperty, EdgeProperty, boost::vecS>;

using Vertex = GBNGraph::vertex_descriptor;
using Edge = GBNGraph::edge_descriptor;
using Port = std::pair<Vertex, std::size_t>;

struct GBN {
	public:
		GBN(Index n, Index m, Index n_inside_vertices);
		GBN(const GBN& gbn) = default;

		Index n;
		Index m;
		Index n_vertices;

		GBNGraph graph;

		friend std::vector<Vertex> all_vertices(const GBN& gbn);
		friend std::vector<Vertex> inside_vertices(const GBN& gbn);
		friend const std::vector<Vertex>& input_vertices(const GBN& gbn);
		friend const std::vector<Vertex>& output_vertices(const GBN& gbn);

		friend Index input_idx(const Vertex& v, const GBN& gbn);
		friend Index output_idx(const Vertex& v, const GBN& gbn);

		friend const std::vector<Vertex>& output_vertices(const GBN& gbn);

		friend Vertex add_vertex(GBN& gbn, MatrixPtr p_m, std::string vertex_label);
		friend void remove_vertex(Vertex v, GBN& gbn);

	private:
		std::set<Vertex> visible_vertices;
		std::set<Vertex> hidden_vertices;

		std::vector<Vertex> input_vertices;
		std::vector<Vertex> output_vertices;

		std::size_t n_initial_inside_vertices;
		std::size_t n_initial_n; // TODO: make all this stuff non mutable again to that input_idx etc gets simpler, fix simplifications with F_k
		std::size_t n_initial_m;
};

// vertex / edge specific accessors
Index n(const GBNGraph::vertex_descriptor& v, const GBNGraph& g);
Index m(const GBNGraph::vertex_descriptor& v, const GBNGraph& g);

VertexType type(const GBNGraph::vertex_descriptor& v, const GBNGraph& g);

MatrixPtr matrix(const GBNGraph::vertex_descriptor& v, const GBNGraph& g);

Index port_from(const GBNGraph::edge_descriptor& e, const GBNGraph& g);
Index port_to(const GBNGraph::edge_descriptor& e, const GBNGraph& g);

Index input_idx(const Vertex& v, const GBN& gbn);
Index output_idx(const Vertex& v, const GBN& gbn);

const std::string& name(const GBNGraph::vertex_descriptor& v, const GBNGraph& g);

std::vector<Vertex> all_vertices(const GBN& gbn);
std::vector<Vertex> inside_vertices(const GBN& gbn);
const std::vector<Vertex>& input_vertices(const GBN& gbn);
const std::vector<Vertex>& output_vertices(const GBN& gbn);
