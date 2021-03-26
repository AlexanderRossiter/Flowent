//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_SETUP_H
#define FLOWENT_SETUP_H

#include <string>
#include "Grid.h"
#include "util.h"

namespace setup {
    struct params{};

    Grid read_grid();
    Grid read_grid_testcase(std::string& testcase);
    params read_input_params();
    void calculate_grid_geometries(Grid& g);
    bool check_grid(Grid& g);
    std::vector<float> calculate_face_vector(std::vector<int>& v1, std::vector<int>& v2,
                                             std::vector<int>& v3, std::vector<int>& v4,
                                             Block& b);
    void calculate_block_face_vectors(Block& b);
    void calculate_block_volumes(Block& b);
    //util::point setup::get_face_mid_vector(int i, int j, int k, Block& b);
    std::vector<std::vector<int>> get_vertex_ijk_vectors(int faceId, int i, int j, int k);
    std::vector<float> get_face_midpoint_vector(int faceId, int i, int j, int k, Block& b);
}

#endif //FLOWENT_SETUP_H
