#include "unit_test.h"

void start_unit_testing(int argc) {
	if (argc > 1 + 0) { return; }
	/*
	// Functionality
	SSCFLSOUnitTest::run_tests();
	AlgorithmUnitTest::run_tests();
	// Correctness
	PreprocessUnitTest::run_tests();
	LocalSearchUnitTest::run_tests();
	GreedyUnitTest::run_tests();
	CompositeUnitTest::run_tests();
	TabuUnitTest::run_tests();
	SemiLagrangianRelaxationUnitTest::run_tests();
	// Check time enforcement
	RoundingUnitTest::run_tests();
	GRASPUnitTest::run_tests();
	SimulatedAnnealingUnitTest::run_tests();
	GurobiUnitTest::run_tests();
	LagrangianRelaxationUnitTest::run_tests();

	*/
	// Check if algorithms can solve a medium instance 
	SSCFLSO instance = Generator::load_instance("instances/unit_tests/unit_test_medium.plc", true);
	Timer timer(3 * ONE_MINUTE);
	timer.start_timer();
	ReportResult dummy_report = ReportResult::dummy_report();
	solution_and_value SV = {facility_vector(50, 0), -1};
	Validator FLV(instance);

	LocalSearch algo = LocalSearch(LocalSearch::RANDOM_RESTART, LocalSearch::FIRST);
	algo.solve(instance, SV, timer, dummy_report, false);
	FLV.set_solution(SV.sol);
	std::cout << "Feasible " << bool(FLV.feasible()) << std::endl;
	std::cout << "Value " << FLV.value() << std::endl;
}