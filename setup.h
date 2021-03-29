//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_SETUP_H
#define FLOWENT_SETUP_H

#include <string>
#include "Grid/Grid.h"
#include "util.h"
#include "Grid/Patch.h"


namespace setup {
    struct params{};

    Grid read_grid();
    Grid read_grid_testcase(std::string& testcase);
    params read_input_params();
    void read_patches(Grid& g, std::string& fname);
}

#endif //FLOWENT_SETUP_H
