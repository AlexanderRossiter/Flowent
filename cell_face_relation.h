//
// Created by Alexander Rossiter on 25/03/2021.
//

#ifndef FLOWENT_CELL_FACE_RELATION_H
#define FLOWENT_CELL_FACE_RELATION_H
std::vector<std::vector<int>> iface_verts = {{0,0,0}, {0,1,0}, {0,0,1}, {0,1,1}};
std::vector<std::vector<int>> jface_verts = {{0,0,0}, {1,0,0}, {0,0,1}, {1,0,1}};
std::vector<std::vector<int>> kface_verts = {{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0}};
//std::vector<std::vector<int>> cell_rel_idx = {{0,0,0},{}};
#endif //FLOWENT_CELL_FACE_RELATION_H
