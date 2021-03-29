//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_GRID_H
#define FLOWENT_GRID_H


#include <vector>
#include "Block.h"
#include "../util.h"
#include <iostream>
#include "cell_face_relation.h"

// Forward declaration
class Patch;

class Grid {
private:
    std::vector<Block> blocks;
    std::vector<Patch*> patches;
    static std::vector<float> calculate_face_vector(std::vector<int>& v1, std::vector<int>& v2,
                                                    std::vector<int>& v3, std::vector<int>& v4,
                                                    Block& b);
    void calculate_block_face_vectors(Block& b);
    void calculate_block_volumes(Block& b);
    std::vector<std::vector<int>> get_vertex_ijk_vectors(int faceId, int i, int j, int k);
    std::vector<float> get_face_midpoint_vector(int faceId, int i, int j, int k, Block& b);

public:
    int nb;
    void add_block(Block b);
    Block& get_block_by_id(int bid);
    std::vector<Block>&  get_blocks();
    std::vector<Patch*>& get_patches();
    void calculate_grid_geometries();

};


#endif //FLOWENT_GRID_H
