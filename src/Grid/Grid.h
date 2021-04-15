//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_GRID_H
#define FLOWENT_GRID_H


#include <vector>
#include "Block.h"
#include "../util/util.h"
#include <iostream>
#include <memory>
#include "cell_face_relation.h"
#include "Patch.h"

class Grid {
private:
    std::vector<Block> blocks;
    std::vector<std::unique_ptr<Patch>> patches;


    // Abstraction for grid volume and surface vector calculations.
    static std::vector<double> calculate_face_vector(std::vector<int>& v1, std::vector<int>& v2,
                                                    std::vector<int>& v3, std::vector<int>& v4,
                                                    Block& b);
    void calculate_block_face_vectors(Block& b);
    void calculate_block_volumes(Block& b);
    static std::vector<std::vector<int>> get_vertex_ijk_vectors(int faceId, int i, int j, int k);
    std::vector<double> get_face_midpoint_vector(int faceId, int i, int j, int k, Block& b);
    void find_grid_min_volume();

public:
    float minV;
    int nb;
    void add_block(Block b);
    Block& get_block_by_id(int bid);
    std::unique_ptr<Patch>& get_patch_by_id(int pid);
    std::vector<Block>&  get_blocks();
    std::vector<std::unique_ptr<Patch>>& get_patches();
    void calculate_grid_geometries();
    void initialise_walls();
    void move_patches_to_halo_grid();
    void move_block_iteration_extent_for_periodic_patches();

};


#endif //FLOWENT_GRID_H
