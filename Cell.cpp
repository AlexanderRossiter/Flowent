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
//    Si = Si_;
//    Sj = Sj_;
//    Sk = Sk_;

//    A.push_back(std::sqrt(S[0][0]*S[0][0] + S[0][1]*S[0][1] + S[0][2]*S[0][2]));
//    A.push_back(std::sqrt(S[1][0]*S[1][0] + S[1][1]*S[1][1] + S[1][2]*S[1][2]));
//    A.push_back(std::sqrt(S[2][0]*S[2][0] + S[2][1]*S[2][1] + S[2][2]*S[2][2]));

    // Normalise the vectors.
    for (int n = 0; n < 3; n++) {
        A.push_back(std::sqrt(S[n][0]*S[n][0] + S[n][1]*S[n][1] + S[n][2]*S[n][2]));
        for (float& f : S[n]) {f /= A[n];}
    }

}
