//
// Created by Alexander Rossiter on 30/03/2021.
//

#ifndef FLOWENT_SOLVER_H
#define FLOWENT_SOLVER_H


#include "Grid/Grid.h"
#include "Gas.h"
#include "SolutionParameters.h"

class Solver {
private:
    Grid g;
    Gas gas;
    SolutionParameters sp;
public:
    Solver(Grid& g_, Gas& gas_, SolutionParameters& sp_): g{std::move(g_)}, gas{gas_}, sp{sp_} {};
    ~Solver(){};
};


#endif //FLOWENT_SOLVER_H
