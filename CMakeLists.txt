cmake_minimum_required(VERSION 3.17)
project(Flowent)

set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/boost_1_81_0/")

add_compile_options(-O3)

add_executable(Flowent src/main.cpp src/Grid/Block.cpp src/Grid/Block.h src/Solving/Gas.cpp src/Solving/Gas.h src/util/setup.cpp src/util/setup.h src/Grid/Grid.cpp src/Grid/Grid.h src/Grid/Cell.cpp src/Grid/Cell.h src/util/typedefinitions.h src/util/util.cpp src/util/util.h src/Grid/cell_face_relation.h src/Grid/Grid_setup.cpp src/Grid/cell_face_relation.cpp src/Grid/Patch.cpp src/Grid/Patch.h src/util/GridWriter.cpp src/util/GridWriter.h src/Solving/Solver.cpp src/Solving/Solver.h src/Solving/SolutionParameters.h src/Solving/InitialGuess.h src/Solving/InitialGuess.cpp src/util/compressible_flow_functions.cpp src/util/compressible_flow_functions.h src/util/Menu.cpp src/util/Menu.h)
