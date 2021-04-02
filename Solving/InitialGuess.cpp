//
// Created by adr39 on 31/03/2021.
//

#include "InitialGuess.h"
#include "../compressible_flow_functions.h"

void BasicGuess::generate_guess(Grid &g) {
    float p_po = cff::fp(Mach, gas.ga);
    float tstat = pow(p_po, (gas.ga-1)/gas.ga)*to;
    float vel = sqrt(2*gas.cp*(to-tstat));
    float ro = p_po * po / gas.R / tstat;

    for (Block& b : g.get_blocks()) {
        for (int i = b.ist; i < b.ien; i++) {
            for (int j = b.jst; j < b.jen; j++) {
                for (int k = b.kst; k < b.ken; k++) {
                    b.secondary_vars["pstat"][i][j][k] = p_po * po;
                    b.secondary_vars["vx"][i][j][k] = vel * cos(yaw*M_PI/180) * cos(pitch*M_PI/180);
                    b.secondary_vars["vy"][i][j][k] = vel * sin(yaw*M_PI/180);
                    b.secondary_vars["vz"][i][j][k] = vel * cos(yaw*M_PI/180) * sin(pitch*M_PI/180);
                    b.primary_vars["ro"][i][j][k] = ro;
                    b.primary_vars["rovx"][i][j][k] = b.secondary_vars["vx"][i][j][k] * ro;
                    b.primary_vars["rovy"][i][j][k] = b.secondary_vars["vy"][i][j][k] * ro;
                    b.primary_vars["rovz"][i][j][k] = b.secondary_vars["vz"][i][j][k] * ro;
                    b.primary_vars["roe"][i][j][k] = ro * (gas.cv * tstat + 0.5*vel*vel);
                }
            }
        }
    }
}
