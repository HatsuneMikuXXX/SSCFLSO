#include <vector>
using namespace std;

struct SSCFLSO {
	int facilities = 0; // Number of facilities. Identifiers zero-based.
	int clients = 0; // Number of clients. Identifiers zero-based.
	vector<float> demands = vector<float>(); // Size |I|.
	vector<float> capacities = vector<float>(); // Size |J|.
	vector<float> open_costs = vector<float>(); // Size |J|. Cost to open a facility.
	vector<vector<float>> dist_costs = vector<vector<float>>(); // Size |J| times |I|. Cost to distribute/of the distance from a facility to a client.
	vector<vector<int>> preferences = vector<vector<int>>(); // Size |I| times |J|. First element (facility) is the most preferred.
};