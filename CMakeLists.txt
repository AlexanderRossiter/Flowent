cmake_minimum_required(VERSION 3.17)
project(Flowent)

set(CMAKE_CXX_STANDARD 17)

include_directories("/usr/local/boost_1_75_0/")

add_executable(Flowent main.cpp Block.cpp Block.h Gas.cpp Gas.h algo.cpp algo.h setup.cpp setup.h Grid.cpp Grid.h Cell.cpp Cell.h typedefinitions.h util.cpp util.h)