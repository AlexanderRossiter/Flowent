#include <iostream>
#include "Grid/Grid.h"
#include "util/setup.h"
#include "util/GridWriter.h"
#include "Solving/Solver.h"
#include "Solving/InitialGuess.h"

int main(int argc, char **argv) {

    // Hard coded test cases for testing.
    std::string directory = "../resources/test_cases/";
    std::string tc = "Nash_Rig";
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
    Grid g = setup::read_grid(fullpath);
    std::cout << " Done." << std::endl;
    g.move_patches_to_halo_grid();

    // Calculate grid cell normal vectors and volumes.
    g.calculate_grid_geometries();

    // Find where the walls are, i.e. where there are no patches.
    // Also change the iteration extent of the grid so that we deal
    // with periodic patches appropriately.
    std::cout << "Initialising walls...";
    g.initialise_walls();
    //g.move_block_iteration_extent_for_periodic_patches();
    std::cout << " Done." << std::endl;

    // Print out patch details.
    for (auto& p : g.get_patches()) {
        std::cout << p->to_string() << std::endl;
    }

    // Hard coded guess currently.
    BasicGuess ig(gas, 0.6, 0.9995E5, 300, 0, 0);
    ig.generate_guess(g);

    // Run the program
    Solver solver(g, gas, sp);
    solver.run_nsteps(sp.nstep);

    // Write the solution
    GridWriter gw(solver.g);
    std::string str(directory + "gridout.flwnt");
    gw.write_grid(str);

}


