//
// Created by adr39 on 31/03/2021.
//

#ifndef FLOWENT_COMPRESSIBLE_FLOW_FUNCTIONS_H
#define FLOWENT_COMPRESSIBLE_FLOW_FUNCTIONS_H
#include "math.h"
namespace cff {
    float fMp(float p_po, float ga=1.4);
    float fp(float M, float ga=1.4);
}
#endif //FLOWENT_COMPRESSIBLE_FLOW_FUNCTIONS_H
