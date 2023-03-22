//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_SETUP_H
#define FLOWENT_SETUP_H

#include "Gas.h"
#include "Grid.h"
#include "Patch.h"
#include "SolutionParameters.h"
#include "util.h"
#include <string>

namespace setup {
struct params
{};

Grid
read_grid(std::string& fname);
Grid
read_grid_testcase(std::string& testcase);
params
read_input_params();
void
read_patches(Grid& g, std::string& fname);
Gas
read_gas(std::string& gasName);
SolutionParameters
read_solution_params(std::string& directory);
}

#endif // FLOWENT_SETUP_H
