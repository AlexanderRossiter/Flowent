//
// Created by Alexander Rossiter on 30/03/2021.
//

#include "Solver.h"

void Solver::apply_boundary_conditions() {
    // Apply the patch boundary conditions.
    for (auto& p : g.get_patches()) {
        p->apply(*this);
    }
}

void Solver::set_secondary_variables() {
    for (Block& b : g.get_blocks()) {
        for (int i = b.ist; i < b.ien; i++) {
            for (int j = b.jst; j < b.jen; j++) {
                for (int k = b.kst; k < b.ken; k++) {
                    b.secondary_vars["vx"][i][j][k] = b.primary_vars["rovx"][i][j][k] / b.primary_vars["ro"][i][j][k];
                    b.secondary_vars["vy"][i][j][k] = b.primary_vars["rovy"][i][j][k] / b.primary_vars["ro"][i][j][k];
                    b.secondary_vars["vz"][i][j][k] = b.primary_vars["rovz"][i][j][k] / b.primary_vars["ro"][i][j][k];

                    float velSq = b.secondary_vars["vx"][i][j][k]*b.secondary_vars["vx"][i][j][k] +
                            b.secondary_vars["vy"][i][j][k]*b.secondary_vars["vy"][i][j][k] +
                            b.secondary_vars["vz"][i][j][k]*b.secondary_vars["vz"][i][j][k];
                    b.secondary_vars["pstat"][i][j][k] = (gas.ga-1) * (b.primary_vars["roe"][i][j][k] - 0.5f*b.primary_vars["ro"][i][j][k]*velSq);
                    b.secondary_vars["hstag"][i][j][k] = (b.primary_vars["roe"][i][j][k] + b.secondary_vars["pstat"][i][j][k]) / b.primary_vars["ro"][i][j][k];
                }
            }
        }
    }

}
