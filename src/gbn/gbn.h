#pragma once

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "matrix.h"

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
using GBN = std::tuple<std::pair<int, int>, GBNGraph>;

GBNGraph& graph(GBN& gbn);
const GBNGraph& graph(const GBN& gbn);

std::pair<int,int>& type(GBN& gbn);
const std::pair<int,int>& type(const GBN& gbn);

std::pair<int,int> type(const GBNGraph::vertex_descriptor& v, const GBNGraph& g);
VertexType node_type(const GBNGraph::vertex_descriptor& v, const GBNGraph& g);

int port_from(const GBNGraph::edge_descriptor& e, const GBNGraph& g);
int port_to(const GBNGraph::edge_descriptor& e, const GBNGraph& g);
std::string& name(const GBNGraph::vertex_descriptor& v, GBNGraph& g);
const std::string& name(const GBNGraph::vertex_descriptor& v, const GBNGraph& g);

void check_gbn_integrity(const GBN& gbn);
