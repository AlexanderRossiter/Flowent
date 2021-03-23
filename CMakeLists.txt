cmake_minimum_required(VERSION 3.17)
project(Flowent)

set(CMAKE_CXX_STANDARD 17)

add_executable(Flowent main.cpp Block.cpp Block.h Gas.cpp Gas.h algo.cpp algo.h setup.cpp setup.h Grid.cpp Grid.h)