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
	algorithmClass.h
)