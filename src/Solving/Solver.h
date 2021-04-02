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
    void apply_boundary_conditions();
    void set_secondary_variables();

};


#endif //FLOWENT_SOLVER_H
