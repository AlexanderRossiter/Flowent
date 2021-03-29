#include <iostream>
#include "Grid/Grid.h"
#include "setup.h"

int main(int argc, char **argv) {

    // Read grid data
    std::string tc = "gaussian_bump";
    Grid g = setup::read_grid_testcase(tc);
    g.move_patches_to_halo_grid();

    // Calculate grid cell normal vectors and volumes.
    g.calculate_grid_geometries();

    for (std::unique_ptr<Patch>& p : g.get_patches()) {
        std::cout << p->to_string() << std::endl;
    }

    for (Block& b : g.get_blocks()) {
        for (int i = b.ist; i < b.ien; i++) {
            for (int j = b.jst; j < b.jen; j++) {
                std::cout << b.isWall[i][j][b.ken-1] << " ";
            }
            std::cout << "\n";
        }
    }
    std::cout << std::endl;

    g.initialise_walls();

    for (Block& b : g.get_blocks()) {
        for (int i = b.ist; i < b.ien; i++) {
            for (int j = b.jst; j < b.jen; j++) {
                std::cout << b.isWall[i][j][b.ken-1] << " ";
            }
            std::cout << "\n";
        }
    }
    std::cout << std::endl;

    // Find walls.


    // Run main loop

}


