#pragma once

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "matrix.h"

using VertexIndexProperty = boost::property<boost::vertex_index_t, std::size_t>;
using EdgeIndexProperty = boost::property<boost::edge_index_t, std::size_t>;
using CitationNetwork = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexIndexProperty, EdgeIndexProperty, boost::vecS>;
