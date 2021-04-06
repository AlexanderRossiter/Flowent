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
    float delta_t;
    void apply_boundary_conditions();
    void set_secondary_variables();
    void set_convective_fluxes(Block& b, int faceId, Extent& extent);
    void sum_convective_fluxes(Block &b, vector3d<float>& phi, vectornd<float, 4>& flux, vector3d<float>& residual);
    void run_iteration();
    void set_timestep(float tstag);
    void set_wall_bconds(Block& b);
    void run_nsteps(int nsteps);
    void smooth(Block& b, vector3d<float>& phi);

};


#endif //FLOWENT_SOLVER_H
