//
// Created by adr39 on 31/03/2021.
//

#include "InitialGuess.h"
#include "compressible_flow_functions.h"

void
BasicGuess::generate_guess(Grid& g)
{
    double p_po  = cff::fp(Mach, gas.ga);
    double tstat = pow(p_po, (gas.ga - 1) / gas.ga) * to;
    double vel   = sqrt(2 * gas.cp * (to - tstat));
    double ro    = p_po * po / gas.R / tstat;

    for (Block& b : g.get_blocks()) {
        for (int i = b.ist; i < b.ien; i++) {
            for (int j = b.jst; j < b.jen; j++) {
                for (int k = b.kst; k < b.ken; k++) {
                    b.secondary_vars["pstat"][i][j][k] = p_po * po;
                    double tmp_yaw;
                    if (i < b.ien - 1 && k > b.kst && k < b.ken - 1) {
                        tmp_yaw = atan((b.y[i + 1][j][k] - b.y[i][j][k]) / (b.x[i + 1][j][k] - b.x[i][j][k])) * 180 / M_PI;
                    } else {
                        tmp_yaw = 0;
                    }
                    b.secondary_vars["vx"][i][j][k] = vel * cos(tmp_yaw * M_PI / 180) * cos(pitch * M_PI / 180);
                    b.secondary_vars["vy"][i][j][k] = vel * sin(tmp_yaw * M_PI / 180);
                    b.secondary_vars["vz"][i][j][k] = vel * cos(tmp_yaw * M_PI / 180) * sin(pitch * M_PI / 180);
                    b.primary_vars["ro"][i][j][k]   = ro;
                    b.primary_vars["rovx"][i][j][k] = b.secondary_vars["vx"][i][j][k] * ro;
                    b.primary_vars["rovy"][i][j][k] = b.secondary_vars["vy"][i][j][k] * ro;
                    b.primary_vars["rovz"][i][j][k] = b.secondary_vars["vz"][i][j][k] * ro;
                    b.primary_vars["roe"][i][j][k]  = ro * (gas.cv * tstat + 0.5 * vel * vel);
                }
            }
        }
    }
}

void
UpperLowerWalls::generate_guess(Grid& g)
{
}
