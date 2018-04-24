#include "cn_io.h"

#include <iomanip>
#include <string>
#include <boost/algorithm/string_regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/regex.hpp>
#include <regex>
#include <cmath>

#include "../helpers.hpp"

CN read_cn(std::istream& istr)
{
	CN cn;
	std::string line;

	// read in number of places
	std::getline(istr, line);
	boost::trim(line);
	if(line.empty())
		throw std::logic_error("Wrong format: n");
	std::stringstream ss1(line);
	ss1 >> cn.n;
	
	// read in initial marking
	std::getline(istr, line);
	boost::trim(line);
	if(line.empty())
		throw std::logic_error("Wrong format: initial marking");
	std::stringstream ss2(line);
	while(!ss2.eof())
	{
		int place = -1;
		ss2 >> place;
		if(place != -1)
		{
			if(place >= cn.n || place < 0)
				throw std::logic_error("Marking contains place that is larger than n (or lower than zero).");
			cn.m.set(place);
		}
	}
	
	// read transitions
	while(!istr.eof())
	{
		Transition t;
		std::getline(istr, line);
		boost::trim(line);
		if(line.empty())
			continue;

		boost::regex split_regex("->");
		std::vector<std::string> res;
		boost::split_regex(res, line, split_regex);

		// read in pre
		std::stringstream ss4(res.at(0));
		while(!ss4.eof())
		{
			int tmp = -1;
			ss4 >> tmp;
			if(tmp != -1)
			{
				if(tmp >= cn.n || tmp < 0)
					throw std::logic_error("Transition contains place that is larger than n (or lower than zero).");

				t.pre.push_back(tmp);
			}
		}

		// read in post
		std::stringstream ss5(res.at(1));
		while(!ss5.eof())
		{
			int tmp = -1;
			ss5 >> tmp;
			if(tmp != -1)
			{
				if(tmp >= cn.n || tmp < 0)
					throw std::logic_error("Transition contains place that is larger than n (or lower than zero).");
				t.post.push_back(tmp);
			}
		}
		cn.transitions.push_back(t);
	}

	return cn;
}


std::ostream& print_cn_details(std::ostream& ostr, const CN& cn)
{
	ostr << "----------------------------------------\n";
	ostr << "n: " << cn.n << "\n";
	ostr << "m: ";
	print_bitvec(ostr, cn.m, cn.n);
	ostr << "\n";
	ostr << "----------------------------------------\n";
	ostr << "transitions: " << "\n";
	std::size_t i = 0;
	for(auto t : cn.transitions)
	{
		ostr << "t" << i << ": ";
		for(auto p : t.pre)
			ostr << p << " ";

		ostr << "-> ";

		for(auto p : t.post)
			ostr << p << " ";

		ostr << "\n";
		i++;
	}

	ostr.flush();

	return ostr;
}

namespace {
	using vertex_name_t = boost::vertex_name_t;
	using vertex_type_t = boost::vertex_owner_t;

	const auto vertex_name = boost::vertex_name;
	const auto vertex_type = boost::vertex_owner;
	enum VertexType {
		PLACE,
		TRANSITION
	};

	using VertexProperty = boost::property<vertex_name_t, std::string, boost::property<vertex_type_t, VertexType>>;
	using CNGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexProperty, boost::no_property, boost::vecS>;

	class VertexWriter {
		public:
			VertexWriter(const CNGraph& g) : g(g) {}

			void operator()(std::ostream& out, const CNGraph::vertex_descriptor& v) const {
				if(get(vertex_type,g,v) == PLACE)
					out << "[label=\"" << get(vertex_name,g,v) << "\", shape=\"circle\"]";
				else
					out << "[label=\"" << get(vertex_name,g,v) << "\", shape=\"box\", height=0.15, width=0.6]";
			}
		private:
			const CNGraph& g;
	};

	CNGraph build_cn_graph(const CN& cn)
	{
		CNGraph g(cn.n + cn.transitions.size());
		for(std::size_t i = 0; i < cn.n; i++)
		{
			put(vertex_name, g, i, std::string("S") + std::to_string(i));
			put(vertex_type, g, i, PLACE);
		}

		for(std::size_t i = 0; i < cn.transitions.size(); i++)
		{
			put(vertex_name, g, cn.n+i, std::string("t") + std::to_string(i));
			put(vertex_type, g, cn.n+i, TRANSITION);
		}
		for(std::size_t i = 0; i < cn.transitions.size(); i++)
		{
			auto& t = cn.transitions[i];
			for(auto s : t.pre)
				boost::add_edge(s,cn.n+i,g);
			for(auto s : t.post)
				boost::add_edge(cn.n+i,s,g);
		}

		return g;
	}
}

std::ostream& draw_cn(std::ostream& ostr, const CN& cn)
{
	auto g = build_cn_graph(cn);

	boost::write_graphviz(ostr, g, VertexWriter(g));//, EdgeWriter(gbn.graph), GraphWriter(title));

	return ostr;
}
