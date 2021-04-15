//
// Created by Alexander Rossiter on 30/03/2021.
//

#ifndef FLOWENT_SOLVER_H
#define FLOWENT_SOLVER_H


#include "../Grid/Grid.h"
#include "Gas.h"
#include "SolutionParameters.h"

class Solver {
private:
    SolutionParameters sp;

public:
    Solver(Grid& g_, Gas& gas_, SolutionParameters& sp_): g{std::move(g_)}, gas{gas_}, sp{sp_} {};
    ~Solver(){};
    Grid g;
    Gas gas;
    int nstep=0;
    double delta_t;
    void apply_boundary_conditions();
    void set_secondary_variables(Block& b);
    void set_convective_fluxes(Block& b, int faceId, Extent& extent);
    void set_convective_fluxes_2(Block& b, vectornd<double,4>& flux, std::string varName, int faceId, Extent& extent);
    void set_mass_fluxes(Block& b, int faceId, Extent& extent);
    void sum_convective_fluxes(Block &b, vector3d<double>& phi, vectornd<double, 4>& flux, vector3d<double>& residual);
    void run_iteration();
    void set_timestep(double tstag);
    void set_wall_bconds(Block& b);
    void run_nsteps(int nsteps);
    void smooth(Block& b, vector3d<double>& phi);
    void test_for_nans(Block& b);
    void print_flux(vectornd<double, 4> var, int faceId, int k=1);
    void print_var(vector3d<double> var, int k=1);
    void print_Ax(vector3d<Cell> var, int k);

};


#endif //FLOWENT_SOLVER_H
