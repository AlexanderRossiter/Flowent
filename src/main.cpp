#include <iostream>
#include "Grid/Grid.h"
#include "util/setup.h"
#include "util/GridWriter.h"
#include "Solving/Solver.h"
#include "Solving/InitialGuess.h"
#include "util/Menu.h"

int main(int argc, char **argv) {

    Menu menu;
    menu.starting_menu();
    std::string fullpath = menu.directory + menu.case_name;

    // Read in gas properties.
    std::cout << "Reading gas properties...";
    Gas gas = setup::read_gas(menu.gas_string);
    std::cout << " Done." << std::endl;

    // Read in solution parameters.
    std::cout << "Reading solution parameters...";
    SolutionParameters sp = setup::read_solution_params(menu.directory);
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
    // g.move_block_iteration_extent_for_periodic_patches();
    std::cout << " Done." << std::endl;

    // Print out patch details.
    for (auto& p : g.get_patches()) {
        std::cout << p->to_string() << std::endl;
    }

    // Initial guess.
    BasicGuess ig(gas, menu.ig_M, menu.ig_Po, menu.ig_To, menu.ig_yaw, menu.ig_pitch);
    ig.generate_guess(g);

    // Run the program
    Solver solver(g, gas, sp);
    solver.run_nsteps(sp.nstep);

    // Write the solution
    GridWriter gw(solver.g);
    std::string str(menu.directory + "gridout.flwnt");
    gw.write_grid(str);

}


