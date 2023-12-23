#ifndef SSCFLSO_H
#define SSCFLSO_H
#include <vector>
struct SSCFLSO {
	int facilities = 0; // Number of facilities. Identifiers zero-based.
	int clients = 0; // Number of clients. Identifiers zero-based.
	std::vector<float> demands = std::vector<float>(); // Size |I|.
	std::vector<float> capacities = std::vector<float>(); // Size |J|.
	std::vector<float> open_costs = std::vector<float>(); // Size |J|. Cost to open a facility.
	std::vector<std::vector<float>> dist_costs = std::vector<std::vector<float>>(); // Size |J| times |I|. Cost to distribute/of the distance from a facility to a client.
	std::vector<std::vector<int>> preferences = std::vector<std::vector<int>>(); // Size |I| times |J|. First element (facility) is the most preferred.
};
#endif