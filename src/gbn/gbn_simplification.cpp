#include "gbn_simplification.h"

#include <iostream>
#include "matrix_io.h"
#include "gbn_modification.h"

namespace {
	using Port = std::pair<Vertex, int>;

	void check_and_apply_F1(GBN& gbn, Vertex v)
	{
		auto& g = gbn.graph;

		if(type(v,g) != NODE)
			return;

		if(matrix(v,g)->type == ONE_B)
		{
			if(boost::out_degree(v,g) > 1)
			{
				auto& m = dynamic_cast<OneBMatrix&>(*matrix(v,g));

				std::vector<Port> sucessor_ports;	
				for(auto e : boost::make_iterator_range(boost::out_edges(v,g)))
					sucessor_ports.push_back({ boost::target(e,g), port_to(e,g) });

				remove_vertex(v,gbn);

				for(auto p : sucessor_ports)
				{
					auto v_new = add_vertex(gbn);
					put(vertex_name, g, v_new, std::string("1_") + std::to_string(m.b));
					put(vertex_type, g, v_new, NODE);
					put(vertex_matrix, g, v_new, MatrixPtr(new OneBMatrix(m.b)));
					auto e = boost::add_edge(v_new, p.first, g).first;
					put(edge_position, g, e, std::pair<std::size_t, std::size_t>{ 0, p.second });
				}
			}
		}
	}
}


void gbn_simplification(GBN& gbn)
{
	auto& g = gbn.graph;

	// (F1): run over all ONE_B matrices and check their precessors
	for(auto v: vertices(gbn))	
		check_and_apply_F1(gbn, v);
}
