//
// Created by Alexander Rossiter on 27/03/2021.
//
#include "cell_face_relation.h"
// Define the vertices for each face of the cell.
// Vertices stored stored
// 3 o----o 2
//   |----|
//   |----|
// 4 o----o 1
const std::vector<std::vector<std::vector<int>>> face_verts = {{{0,0,0}, {0,1,0}, {0,1,1}, {0,0,1}},
                                                         {{1,0,1}, {1,0,0}, {0,0,0}, {0,0,1}},
                                                         {{1,0,0}, {1,1,0}, {0,1,0}, {0,0,0}}};