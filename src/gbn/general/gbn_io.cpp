#include "gbn_io.h"
#include "../matrix/matrix_io.h"

#include <regex>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/graph/graphviz.hpp>
#include <cmath>
#include <boost/graph/filtered_graph.hpp>
#include "../../helpers.hpp"

void write_gbn(std::ostream& ostr, const GBN& gbn)
{
	auto& g = gbn.graph;

	auto inside_vertices = ::inside_vertices(gbn);
	ostr << inside_vertices.size() << " " << gbn.n << " " << gbn.m << std::endl;
	for(auto v : inside_vertices)
	{
		ostr << "v_" << v << " ";
		write_matrix(ostr, *matrix(v,g));
		ostr << std::endl;
	}

	auto input_vertices_vec = ::input_vertices(gbn);

	for(auto v : inside_vertices)
	{
		if(boost::in_degree(v,g) == 0)
			continue;

		std::set<std::tuple<std::size_t, Vertex, std::size_t>> tmp; // helper to sort wrt. port_to
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			auto u = boost::source(e,g);
			auto p = port_from(e,g);

			tmp.insert({ port_to(e,g), u, p });
		}

		ostr << "v_" << v << " ";
		bool first = true;
		for(auto [p_to, u, p_from] : tmp)
		{
			if(first)
				first = false;
			else
				ostr << " ";

			if(is_in(u, input_vertices_vec))
				ostr << "i_" << input_idx(u,gbn);
			else
				ostr << "v_" << u << "(" << p_from << ")";
		}
		ostr << std::endl;
	}
	for(auto v : ::output_vertices(gbn))
	{
		if(boost::in_degree(v,g) == 0)
			continue;

		std::set<std::tuple<std::size_t, Vertex, std::size_t>> tmp; // helper to sort wrt. port_to
		for(auto e : boost::make_iterator_range(boost::in_edges(v,g)))
		{
			auto u = boost::source(e,g);
			auto p = port_from(e,g);

			tmp.insert({ port_to(e,g), u, p });
		}

		ostr << "o_" << output_idx(v,gbn) << " ";
		bool first = true;
		for(auto [p_to, u, p_from] : tmp)
		{
			if(first)
				first = false;
			else
				ostr << " ";

			if(is_in(u, input_vertices_vec))
				ostr << "i_" << input_idx(u,gbn);
			else
				ostr << "v_" << u << "(" << p_from << ")";
		}
		ostr << std::endl;
	}
}

GBN read_gbn(std::istream& istr)
{
	std::string line;
	std::getline(istr, line);
	boost::trim(line);
	std::vector<std::string> first_line_split;
	boost::split(first_line_split, line, boost::is_any_of(" "));
	std::size_t n_vertices = std::stol(first_line_split[0]);
	Index n = std::stol(first_line_split[1]);
	Index m = std::stol(first_line_split[2]);
	GBN gbn(n, m, n_vertices);

	// read in vertices and matrices
	std::map<std::string, std::size_t> vertex_label_map;
	for(Vertex v = 0; v < n_vertices; v++)
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

		try {
			auto p_matrix = read_matrix({ remainder });
			put(vertex_matrix, gbn.graph, v, p_matrix);
			put(vertex_type, gbn.graph, v, NODE);
			put(vertex_name, gbn.graph, v, vertex_label);
		}
		catch(const std::logic_error& e)
		{
			throw std::logic_error(std::string("Matrix error in line: '") + line + "\n" + e.what());
		}
	}

	for(Index i_input = 0; i_input < n; i_input++)
	{
		auto v = n_vertices+i_input;
		vertex_label_map.insert({ std::string("i_")+std::to_string(i_input), v });
		put(vertex_type, gbn.graph, v, INPUT);
		put(vertex_name, gbn.graph, v, std::string("i_")+std::to_string(i_input));
	}

	for(Index i_output = 0; i_output < m; i_output++)
	{
		auto v = n_vertices+n+i_output;
		vertex_label_map.insert({ std::string("o_")+std::to_string(i_output), v });
		put(vertex_type, gbn.graph, v, OUTPUT);
		put(vertex_name, gbn.graph, v, std::string("o_")+std::to_string(i_output));
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

			auto res = boost::add_edge(v_from, v_to, gbn.graph);
			auto& e = res.first;
			put(edge_position, gbn.graph, e, std::pair<std::size_t, std::size_t>{ from_pos, to_pos });
		}
	}

	return gbn;
}

namespace {
	class VertexWriter {
		public:
			VertexWriter(const GBNGraph& g, const bool vertex_name_is_matrix_type) : g(g), vertex_name_is_matrix_type(vertex_name_is_matrix_type) {}

			void operator()(std::ostream& out, const GBNGraph::vertex_descriptor& v) const {
				if(type(v,g) == NODE)
				{
					if(vertex_name_is_matrix_type)
					{
						if(matrix(v,g)->type == DYNAMIC)
							out << "[label=\"v_" << v << "\", shape=\"box\"]";
						else
						{
							out << "[label=\""; 
							print_matrix_label(out, *matrix(v,g));
							out << "\", shape=\"box\"]";
						}
					}
					else
					{
						out << "[label=\"" << name(v,g) << "\", shape=\"box\"]";
					}
				}
				if(type(v,g) == INPUT)
					out << "[xlabel=\"" << name(v,g) << "\", shape=\"point\"]";
				if(type(v,g) == OUTPUT)
					out << "[xlabel=\"" << name(v,g) << "\", shape=\"point\"]";
			}
		private:
			const GBNGraph& g;
			const bool vertex_name_is_matrix_type;
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
		GraphWriter(const GBN& gbn, std::string title = "")
			:gbn(gbn),
			title(title)
			{}

		void operator()(std::ostream& out) const {
			if(!title.empty())
				out << "graph[label=\"" << title << "\"," << "labelloc=top," << "labeljust=c];" << std::endl;
			out << "rankdir=LR;" << std::endl;

			out << "{ rank=same; ";
			for(auto v : input_vertices(gbn))
				out << v << ";";
			out << "}" << std::endl;

			out << "{ rank=same; ";
			for(auto v : output_vertices(gbn))
				out << v << ";";
			out << "}" << std::endl;
		}

		const GBN& gbn;
		std::string title;
	};

	struct AllEdgesTruePredicate {
		inline bool operator()(const Edge& /*e*/) const { return true; }
	};

	struct VisibleVerticesPredicate {
		inline VisibleVerticesPredicate()
		{}
		inline VisibleVerticesPredicate(const std::vector<Vertex>& visible_vertices)
			:visible_vertices(visible_vertices.begin(), visible_vertices.end())
		{}

		bool operator()(const Vertex& v) const { 
			return visible_vertices.count(v) > 0;
		}

		std::set<Vertex> visible_vertices;
	};
}
void draw_gbn_graph(std::ostream& ostr, const GBN& gbn, std::string title, bool vertex_name_is_matrix_type)
{
	boost::filtered_graph<GBNGraph, AllEdgesTruePredicate, VisibleVerticesPredicate> fg(gbn.graph,AllEdgesTruePredicate(), VisibleVerticesPredicate(all_vertices(gbn)));
	boost::write_graphviz(ostr, fg, VertexWriter(gbn.graph, vertex_name_is_matrix_type), EdgeWriter(gbn.graph), GraphWriter(gbn, title));
}
