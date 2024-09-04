add_subdirectory(composite)
add_subdirectory(grasp)
add_subdirectory(gurobi)
add_subdirectory(lagrangian_relaxation)
add_subdirectory(local_search)
add_subdirectory(preprocess)
add_subdirectory(rounding)
add_subdirectory(semi_lagrangian_relaxation)

target_sources(SSCFLSO PUBLIC
		algorithmClass.cpp
		unit_test_utility.cpp
		algorithm_unit_test.cpp
	algorithmClass.h
	unit_test_utility.h
	algorithm_unit_test.h
)