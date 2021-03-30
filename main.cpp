#include <iostream>
#include "Grid/Grid.h"
#include "setup.h"
#include "GridWriter.h"
#include "Solver.h"

int main(int argc, char **argv) {

    // Hard coded test cases for testing.
    std::string directory = "../test_cases/";
    std::string tc = "gaussian_bump";
    std::string gasStr = "air";

    std::string fullpath = directory + tc;

    // Read in gas properties.
    std::cout << "Reading gas properties...";
    Gas gas = setup::read_gas(gasStr);
    std::cout << " Done." << std::endl;

    // Read in solution parameters.
    std::cout << "Reading solution parameters...";
    SolutionParameters sp = setup::read_solution_params(directory);
    std::cout << " Done" << std::endl;

    // Read grid data
    std::cout << "Reading grid...";
    Grid g = setup::read_grid_testcase(fullpath);
    std::cout << " Done." << std::endl;
    g.move_patches_to_halo_grid();

    // Calculate grid cell normal vectors and volumes.
    g.calculate_grid_geometries();

    // Find where the walls are, i.e. where there are no patches.
    std::cout << "Initialising walls...";
    g.initialise_walls();
    std::cout << " Done." << std::endl;

    Solver flowent(g, gas, sp);


//    GridWriter gw(g);
//    std::string str("/Users/ADR/Documents/Programming/Python/flowent_test_case/gridout.flwnt");
//    gw.write_grid(str);

}


