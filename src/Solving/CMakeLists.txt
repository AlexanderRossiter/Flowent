add_library(Solving STATIC
    Gas.cpp
    InitialGuess.cpp
    Solver.cpp
)

target_include_directories(Solving PUBLIC .)
target_link_libraries(Solving PUBLIC
    util
    Grid
)