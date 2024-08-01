#ifndef HELPER_H
#define HELPER_H
#include "../common.h"

const double EPSILON = 10e-7;
typedef std::vector<int> range_vector;

// Abbreviated Standard Algorithms - only commonly used ones
// How are they abbreviated? Instead of passing container.begin() and container.end() we pass only a reference to the container.
namespace asa {
	template<class ContainerRef, class T>
	T sum(const ContainerRef& con, T init) {
		return std::accumulate(std::begin(con), std::end(con), init);
	}

	template<class ContainerRef, class Compare>
	void sort(ContainerRef& con, const Compare& comp) {
		std::sort(std::begin(con), std::end(con), comp);
	}

	template<class ContainerRef, class UnaryFunc>
	void for_each(const ContainerRef& con, const UnaryFunc& func) {
		std::for_each(std::begin(con), std::end(con), func);
	}

	template<class ContainerRef, class UnaryOp>
	void transform(ContainerRef& con, const UnaryOp& op) {
		std::transform(std::begin(con), std::end(con), std::begin(con), op);
	}

	template<class ContainerRef, class OutputContainerRef, class UnaryOp>
	void transform(const ContainerRef& con, OutputContainerRef& outcon, const UnaryOp& op) {
		std::transform(std::begin(con), std::end(con), std::begin(outcon), op);
	}

	template<class ContainerRef, class ArgumentRef, class OutputContainerRef, class BinaryOp>
	void transform(const ContainerRef& con, const ArgumentRef& arg, OutputContainerRef& outcon, const BinaryOp& op) {
		std::transform(std::begin(con), std::end(con), std::begin(arg), std::begin(outcon), op);
	}

	template<class ContainerRef, class Generator>
	void generate(ContainerRef& con, const Generator& gen) {
		std::generate(std::begin(con), std::end(con), gen);
	}

	template<class ContainerRef, class UnaryPred>
	bool all_of(const ContainerRef& con, const UnaryPred& p) {
		return std::all_of(std::begin(con), std::end(con), p);
	}

	template<class ContainerRef, class UnaryPred>
	bool any_of(const ContainerRef& con, const UnaryPred& p) {
		return std::any_of(std::begin(con), std::end(con), p);
	}
}

range_vector range(size_t n);
range_vector range(int n);
void filter(std::vector<bool>& container, const std::function<bool(int)>& predicate);

void inverse(std::vector<int>& output, const std::vector<int>& input);

bool no_facility_is_open(const std::vector<bool>& container);

bool are_same(double a, double b);
bool are_same(const std::vector<double>& a, const std::vector<double>& b);
bool are_same(const std::vector<int>& a, const std::vector<int>& b);
bool are_same(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b);

// Returns ||v||_2
double magnitude(const std::vector<double>& v);

std::string primitive_list_to_string(const std::vector<bool>& container);
std::string primitive_list_to_string(const std::vector<int>& container);
std::string primitive_list_to_string(const std::vector<double>& container);
#endif