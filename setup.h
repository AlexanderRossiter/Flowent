//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_SETUP_H
#define FLOWENT_SETUP_H

#include <string>
#include "Grid.h"

namespace setup {
    struct params{};

    Grid read_grid();
    Grid read_grid_testcase(std::string testcase);
    params read_input_params();
    void calculate_grid_geometries(Grid& g);
    bool check_grid(Grid& g);
    std::vector<float> calculate_face_vector(std::vector<int>& v1, std::vector<int>& v2,
                                             std::vector<int>& v3, std::vector<int>& v4,
                                             Block& b);
    void calculate_block_face_vectors(Block& b);
    void calculate_block_volumes(Block& b);
}

#endif //FLOWENT_SETUP_H
