//
// Created by Alexander Rossiter on 24/03/2021.
//

#include "Cell.h"
#include <cmath>

Cell::Cell(std::vector<float> Si_, std::vector<float> Sj_, std::vector<float> Sk_) {
    Si = Si_;
    Sj = Sj_;
    Sk = Sk_;

    Ai = sqrt(Si[0]*Si[0] + Si[1]*Si[1] + Si[2]*Si[2]);
    Aj = sqrt(Sj[0]*Sj[0] + Sj[1]*Sj[1] + Sj[2]*Sj[2]);
    Ak = sqrt(Sk[0]*Sk[0] + Sk[1]*Sk[1] + Sk[2]*Sk[2]);

    // Normalise the vectors.
    for (float& f : Si) {f /= Ai;}
    for (float& f : Sj) {f /= Aj;}
    for (float& f : Sk) {f /= Ak;}
}
