#include "algorithm.h"


std::vector<facility_vector> add_neighborhood(const facility_vector& x, const facility_vector& preprocessed) {
	std::vector<facility_vector> res = {};
	facility_vector neighbor;
	for (int i = 0; i < x.size(); i++) {
		if (preprocessed[i] == 1 && x[i] == 0) {
			neighbor = x;
			neighbor[i] = 1;
			res.push_back(neighbor);
		}
	}
	return res;
}

std::vector<facility_vector> remove_neighborhood(const facility_vector& x) {
	std::vector<facility_vector> res = {};
	facility_vector neighbor;
	for (int i = 0; i < x.size(); i++) {
		if (x[i] == 1) {
			neighbor = x;
			neighbor[i] = 0;
			res.push_back(neighbor);
		}
	}
	return res;
}

std::vector<facility_vector> swap_neighborhood(const facility_vector& x, const facility_vector& preprocessed) {
	std::vector<facility_vector> res = {};
	facility_vector neighbor;
	for (int i = 0; i < x.size(); i++) {
		if (preprocessed[i] == 1 && x[i] == 0) {
			for (int j = 0; j < x.size(); j++) {
				if (x[j] == 1) {
					neighbor = x;
					neighbor[i] = 1;
					neighbor[j] = 0;
					res.push_back(neighbor);
				}
			}
		}
	}
	return res;
}