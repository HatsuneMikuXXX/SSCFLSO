import os

print("Running the SSCFLSO application requires the gurobi headers and libraries which can be downloaded from the gurobi website.\n")
incl_path = input("Please input the absolute path or relative path with respect to the source folder 'src' to the directory which contains gurobi headers 'gurobi_c.h' and 'gurobi_c++.h': ")
if not os.path.isdir(incl_path):
	print("Given path is not a directory. Canceling process.")
	exit()
lib_path = input("Please input the absolute path or relative path with respect to the source folder 'src' to the directory which contains the static gurobi library 'libgurobi_c++.a' and the shared gurobi library 'libgurobi110_light.so':")
if not os.path.isdir(lib_path):
	print("Given path is not a directory. Canceling process.")
	exit()
print("Starting process to create top level CMake file.")
try:
	os.remove("src/CMakeLists.txt")
except OSError:
	pass
cmake = open("src/CMakeLists.txt", "w")
cmake.write("# CMake created by Python script. Please, only adjust if you know what to do\n")
cmake.write("cmake_minimum_required(VERSION 3.22.1)\n")
cmake.write("project(SSCFLSO_project)\n")
cmake.write("add_executable(SSCFLSO)\n")
cmake.write("add_subdirectory(algorithms)\n")
cmake.write("add_subdirectory(benchmarking)\n")
cmake.write("add_subdirectory(helpers)\n")
cmake.write("add_subdirectory(SSCFLSO_model)\n")
cmake.write("add_subdirectory(tracking)\n")
cmake.write("add_subdirectory(UI)\n")
cmake.write("add_subdirectory(unit_testing_framework)\n")
cmake.write("set(GUROBI_INC " + incl_path + ")\n")
cmake.write("set(GUROBI_LIB " + lib_path + ")\n")
cmake.write("target_include_directories(SSCFLSO PRIVATE ${GUROBI_INC})  \n")
cmake.write("add_library(grblib SHARED IMPORTED GLOBAL)\n")
cmake.write("set_target_properties(grblib PROPERTIES IMPORTED_LOCATION ${GUROBI_LIB}/libgurobi110_light.so)\n")
cmake.write("target_link_libraries(SSCFLSO ${GUROBI_LIB}/libgurobi_c++.a grblib)\n")
cmake.write("target_sources(SSCFLSO PUBLIC main.cpp)")
cmake.close()
print("Finished process.")