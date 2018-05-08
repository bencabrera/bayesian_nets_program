#pragma once

#include "../general/gbn.h"

struct PortComparison {
	bool operator()(const std::pair<Vertex, std::size_t>& p1, const std::pair<Vertex, std::size_t>& p2) {
		if(p1.first != p2.first)
			return p1.first < p2.first;
		return p1.second < p2.second;
	}
};

struct VertexSetInputOutputs {
	using Port = std::pair<Vertex, int>;

	std::vector<Port> input_ports;
	std::vector<Port> output_ports;

	std::vector<Port> input_external_map; 	
	std::vector<std::vector<Port>> output_external_map; 	
};

VertexSetInputOutputs build_inputs_outputs_for_vertices(const GBN& gbn, std::vector<Vertex> vertices);
