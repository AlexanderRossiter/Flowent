//
// Created by adr39 on 31/03/2021.
//

#include "compressible_flow_functions.h"

float cff::fMp(float p_po, float ga) {
    //// Mach number from p/po.
    return sqrt((pow(p_po, -(ga-1) / ga) - 1) * (2 / (ga-1)));
}

float cff::fp(float M, float ga) {
    //// Return p/po for given M;
    return pow(1 + (ga-1)/2 * M*M, -ga / (ga-1));
}