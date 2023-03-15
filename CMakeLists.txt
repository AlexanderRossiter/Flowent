cmake_minimum_required(VERSION 3.20.0)
project(Flowent CXX)

set(CMAKE_CXX_STANDARD 23)

include_directories("/usr/local/boost_1_81_0/")

add_compile_options(-O3)

include(CMakePrintHelpers)
cmake_print_variables(CMAKE_BINARY_DIR)

add_executable(Flowent src/main.cpp)

add_subdirectory(./src/Grid)
add_subdirectory(./src/Solving)
add_subdirectory(./src/util)

target_link_libraries(Flowent PUBLIC
    ${CMAKE_BINARY_DIR}/src/Grid/libGrid.a
    ${CMAKE_BINARY_DIR}/src/Solving/libSolving.a
    ${CMAKE_BINARY_DIR}/src/util/libutil.a
)

