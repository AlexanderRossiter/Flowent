//
// Created by Alexander Rossiter on 24/03/2021.
//

#include "Cell.h"
#include "../util/util.h"
#include <cmath>
#include <utility>

Cell::Cell(std::vector<double> Si_, std::vector<double> Sj_, std::vector<double> Sk_)
{
    S.push_back(Si_);
    S.push_back(Sj_);
    S.push_back(Sk_);
    for (int n = 0; n < 3; n++) {
        A.push_back(std::sqrt(S[n][0] * S[n][0] + S[n][1] * S[n][1] + S[n][2] * S[n][2]));
        Ax.push_back((S[n][0]));
        Ay.push_back((S[n][1]));
        Az.push_back((S[n][2]));
        for (double& f : S[n]) {
            f /= A[n];
        }
    }
}
