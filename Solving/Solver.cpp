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
                    b.vx[i][j][k] = b.rovx[i][j][k] / b.ro[i][j][k];
                    b.vy[i][j][k] = b.rovy[i][j][k] / b.ro[i][j][k];
                    b.vz[i][j][k] = b.rovz[i][j][k] / b.ro[i][j][k];

                    float velSq = b.vx[i][j][k]*b.vx[i][j][k] + b.vy[i][j][k]*b.vy[i][j][k] + b.vz[i][j][k]*b.vz[i][j][k];
                    b.pstat[i][j][k] = (gas.ga-1) * (b.roe[i][j][k] - 0.5f*b.ro[i][j][k]*velSq);
                    b.hstag[i][j][k] = (b.roe[i][j][k] + b.pstat[i][j][k]) / b.ro[i][j][k];
                }
            }
        }
    }

}
