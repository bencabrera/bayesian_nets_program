#include "gbn.h"

class SubGBN {
	public:
		GBN gbn;

		const std::vector<Port> input_ports;
		const std::vector<std::vector<Port>> output_ports;

		static SubGBN make_from_vertices(const GBN& gbn, std::vector<Vertex> vertices);

	private:
		SubGBN(GBN gbn, std::vector<Port> input_ports, std::vector<std::vector<Port>> output_ports); // todo add move constructor &&
};
