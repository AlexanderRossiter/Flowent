#include <iostream>
#include "Grid/Grid.h"
#include "setup.h"

int main() {

    // Read grid data
    std::string tc = "gaussian_bump";
    Grid g = setup::read_grid_testcase(tc);

    // Calculate grid cell normal vectors and volumes.
    g.calculate_grid_geometries();

    // Find walls.


    // Run main loop

}


