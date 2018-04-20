#include "gbn_io.h"
#include "matrix_io.h"

#include <regex>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/graph/graphviz.hpp>
#include <cmath>


GBN read_gbn(std::istream& istr)
{
	std::string line;
	std::getline(istr, line);
	boost::trim(line);
	std::vector<std::string> first_line_split;
	boost::split(first_line_split, line, boost::is_any_of(" "));
	int n_vertices = std::stol(first_line_split[0]);
	int n = std::stol(first_line_split[1]);
	int m = std::stol(first_line_split[2]);
	GBN gbn({ n, m }, n_vertices+n+m);

	// read in vertices and matrices
	std::map<std::string, std::size_t> vertex_label_map;
	for(int v = 0; v < n_vertices; v++)
	{
		std::getline(istr, line);

		auto first_space = line.find_first_of(" ");
		std::string vertex_label = line.substr(0, first_space);
		boost::trim(vertex_label);
		if(vertex_label[0] != 'v')
			throw std::logic_error("Vertices must always start with 'v'.");
		vertex_label_map.insert({ vertex_label, v });

		std::string remainder = line.substr(first_space+1);
		boost::trim(remainder);

		auto p_matrix = read_matrix({ remainder });
		put(vertex_matrix, graph(gbn), v, p_matrix);
		put(vertex_type, graph(gbn), v, NODE);
		name(v, graph(gbn)) = vertex_label;
	}

	for(int i = 0; i < n; i++)
	{
		auto v = n_vertices+i;
		vertex_label_map.insert({ std::string("i_")+std::to_string(i), v });
		put(vertex_type, graph(gbn), v, INPUT);
		name(v, graph(gbn)) = std::string("i_")+std::to_string(i);
	}

	for(int i = 0; i < m; i++)
	{
		auto v = n_vertices+n+i;
		vertex_label_map.insert({ std::string("o_")+std::to_string(i), v });
		put(vertex_type, graph(gbn), v, OUTPUT);
		name(v, graph(gbn)) = std::string("o_")+std::to_string(i);
	}

	while(!istr.eof())
	{
		std::getline(istr, line);
		boost::trim(line);
		if(line.empty())
			continue;

		std::vector<std::string> split_strs;
		boost::split(split_strs, line, boost::is_any_of(" "));
		std::string vec_id_str = split_strs[0];

		if(vec_id_str[0] != 'v' && vec_id_str[0] != 'o')
			throw std::logic_error(std::string("Vertex identifier '") + vec_id_str + "' in edge list has to start with either v or o.");
		if(!vertex_label_map.count(vec_id_str))
			throw std::logic_error(std::string("Vertex identifier '") + vec_id_str + "' in edge list was not defined before.");

		std::size_t v_to = vertex_label_map.at(vec_id_str);
		for(std::size_t i = 1; i < split_strs.size(); i++)
		{
			std::regex edge_from_regex("^([\\w_]+)\\(?([0-9]*)\\)?$");

			std::smatch matches;
			if(!std::regex_match(split_strs[i], matches, edge_from_regex))
				throw std::logic_error(std::string("Edge from entry '") + split_strs[i] + "' does not have the right format.");

			if(matches[1].str()[0] != 'v' && matches[1].str()[0] != 'i')
				throw std::logic_error(std::string("Edge from entry '") + split_strs[i] + "' has to start with either v or i.");

			if(matches[1].str()[0] != 'v' && matches[1].str().empty())
				throw std::logic_error(std::string("Edge from entry '") + split_strs[i] + "' with starting v has to be followed by parathesis and the port number.");

			if(!vertex_label_map.count(matches[1].str()))
				throw std::logic_error(std::string("Precessor '") + matches[1].str() + "' not found.");

			std::size_t v_from = vertex_label_map.at(matches[1].str());
			std::size_t from_pos;
			if(matches[1].str()[0] == 'i')
				from_pos = 0;
			else
		   		from_pos = std::stoi(matches[2]);
			std::size_t to_pos = i-1;

			auto res = boost::add_edge(v_from, v_to, graph(gbn));
			auto& e = res.first;
			put(edge_position, graph(gbn), e, std::pair<std::size_t, std::size_t>{ from_pos, to_pos });
		}
	}

	return gbn;
}

namespace {
	class VertexWriter {
		public:
			VertexWriter(const GBNGraph& g) : g(g) {}

			void operator()(std::ostream& out, const GBNGraph::vertex_descriptor& v) const {
				if(node_type(v,g) == NODE)
					out << "[label=\"" << name(v,g) << "\", shape=\"box\"]";
				if(node_type(v,g) == INPUT)
					out << "[xlabel=\"" << name(v,g) << "\", shape=\"point\"]";
				if(node_type(v,g) == OUTPUT)
					out << "[xlabel=\"" << name(v,g) << "\", shape=\"point\"]";
			}
		private:
			const GBNGraph& g;
	};
	class EdgeWriter {
		public:
			EdgeWriter(const GBNGraph& g) : g(g) {}

			void operator()(std::ostream& out, const GBNGraph::edge_descriptor& e) const {
				out << "[label=\"(" << port_from(e,g) << "," << port_to(e,g) << ")\"]";
			}
		private:
			const GBNGraph& g;
	};
	struct GraphWriter {
		void operator()(std::ostream& out) const {
			out << "rankdir=LR;" << std::endl;
		}
	};
}
void draw_gbn_graph(std::ostream& ostr, const GBNGraph& g)
{
	boost::write_graphviz(ostr, g, VertexWriter(g), EdgeWriter(g), GraphWriter());
}
