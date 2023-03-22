
add_library(util STATIC
    compressible_flow_functions.cpp
    GridWriter.cpp
    Menu.cpp
    setup.cpp
    util.cpp
)

target_include_directories(util PUBLIC .)
target_link_libraries(util PUBLIC
    Solving
    Grid
)