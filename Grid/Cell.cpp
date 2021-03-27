//
// Created by Alexander Rossiter on 24/03/2021.
//

#include "Cell.h"
#include <cmath>
#include <utility>

Cell::Cell(std::vector<float> Si_, std::vector<float> Sj_, std::vector<float> Sk_) {
    S.push_back(Si_);
    S.push_back(Sj_);
    S.push_back(Sk_);
    for (int n = 0; n < 3; n++) {
        A.push_back(std::sqrt(S[n][0]*S[n][0] + S[n][1]*S[n][1] + S[n][2]*S[n][2]));
        for (float& f : S[n]) {f /= A[n];}
    }

}
