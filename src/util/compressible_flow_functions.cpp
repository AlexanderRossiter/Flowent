//
// Created by adr39 on 31/03/2021.
//

#include "compressible_flow_functions.h"

float
cff::fMp(double p_po, double ga)
{
    //// Mach number from p/po.
    return sqrt((pow(p_po, -(ga - 1) / ga) - 1) * (2 / (ga - 1)));
}

float
cff::fp(double M, double ga)
{
    //// Return p/po for given M;
    return pow(1 + (ga - 1) / 2 * M * M, -ga / (ga - 1));
}