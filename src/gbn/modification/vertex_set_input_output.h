#pragma once

#include "../general/gbn.h"

struct VertexSetInputOutputs {
	using Port = std::pair<Vertex, int>;

	std::vector<Port> input_ports;
	std::vector<Port> output_ports;

	std::vector<Port> input_external_map; 	
	std::vector<std::vector<Port>> output_external_map; 	
};

VertexSetInputOutputs build_inputs_outputs_for_vertices(const GBN& gbn, std::vector<Vertex> vertices);
