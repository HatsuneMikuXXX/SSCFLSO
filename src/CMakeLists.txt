cmake_minimum_required(VERSION 3.22.1)
project(SSCFLSO_project)

# Change these paths accordingly
set(GUROBI_INC /mnt/e/gurobi1103/linux64/include) # Must include gurobi_c++.h and gurobi_c.h
set(GUROBI_LIB /mnt/e/gurobi1103/linux64/lib) # Must include libgurobi_c++.a and libgurobi110_light.so

add_executable(SSCFLSO)

add_subdirectory(algorithms)
add_subdirectory(benchmarking)
add_subdirectory(helpers)
add_subdirectory(SSCFLSO_model)
add_subdirectory(tracking)
add_subdirectory(UI)
add_subdirectory(unit_testing_framework)

# Gurobi Declarations
target_include_directories(SSCFLSO PRIVATE ${GUROBI_INC})  
# Gurobi Libraries
add_library(grblib SHARED IMPORTED GLOBAL)
set_target_properties(grblib PROPERTIES IMPORTED_LOCATION ${GUROBI_LIB}/libgurobi110_light.so)
target_link_libraries(SSCFLSO ${GUROBI_LIB}/libgurobi_c++.a grblib)

target_sources(SSCFLSO PUBLIC main.cpp)


