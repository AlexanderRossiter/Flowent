//
// Created by Alexander Rossiter on 25/03/2021.
//

#ifndef FLOWENT_CELL_FACE_RELATION_H
#define FLOWENT_CELL_FACE_RELATION_H

// Define the vertices for each face of the cell.
// Vertices stored stored
// 3 o----o 2
//   |----|
//   |----|
// 4 o----o 1
std::vector<std::vector<std::vector<int>>> face_verts = {{{0,0,0}, {0,1,0}, {0,1,1}, {0,0,1}},
                                                         {{1,0,1}, {1,0,0}, {0,0,0}, {0,0,1}},
                                                         {{1,0,0}, {1,1,0}, {0,1,0}, {0,0,0}}};

//std::vector<std::vector<int>> cell_rel_idx = {{0,0,0},{}};
#endif //FLOWENT_CELL_FACE_RELATION_H
