#include "unit_test.h"

void start_unit_testing(int argc) {
	if (argc > 1 + 0) { return; }

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
}