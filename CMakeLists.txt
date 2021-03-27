cmake_minimum_required(VERSION 3.17)
project(Flowent)

set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/boost_1_75_0/")

add_executable(Flowent main.cpp Grid/Block.cpp Grid/Block.h Gas.cpp Gas.h algo.cpp algo.h setup.cpp setup.h Grid/Grid.cpp Grid/Grid.h Grid/Cell.cpp Grid/Cell.h typedefinitions.h util.cpp util.h Grid/cell_face_relation.h Grid/Grid_setup.cpp Grid/cell_face_relation.cpp Grid/Patch.cpp Grid/Patch.h)