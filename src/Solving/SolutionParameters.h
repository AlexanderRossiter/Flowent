//
// Created by Alexander Rossiter on 30/03/2021.
//

#ifndef FLOWENT_SOLUTIONPARAMETERS_H
#define FLOWENT_SOLUTIONPARAMETERS_H

struct SolutionParameters
{
    double cfl;
    double sfin;
    double fcorr;
    double facsec;
    int    nstep;
};

#endif // FLOWENT_SOLUTIONPARAMETERS_H
