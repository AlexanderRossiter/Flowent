add_library(Grid STATIC
    Block.cpp
    cell_face_relation.cpp
    Cell.cpp
    Grid_setup.cpp
    Grid.cpp
    Patch.cpp
)

target_include_directories(Grid PUBLIC .)
target_link_libraries(Grid PRIVATE 
    util 
    Solving
)