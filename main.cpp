#include <iostream>
#include "Grid/Grid.h"
#include "setup.h"

int main(int argc, char **argv) {

    // Read grid data
    std::string tc = "gaussian_bump";
    Grid g = setup::read_grid_testcase(tc);

    // Calculate grid cell normal vectors and volumes.
    g.calculate_grid_geometries();

    for (std::unique_ptr<Patch>& p : g.get_patches()) {
        std::cout << p->to_string() << std::endl;
    }

    for (Block& b : g.get_blocks()) {
        for (int i = b.ist; i < b.ien; i++) {
            std::cout << b.volume[i][b.jst][b.kst] << std::endl;
        }
    }

    // Find walls.


    // Run main loop

}


