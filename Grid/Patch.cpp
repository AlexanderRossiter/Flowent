//
// Created by Alexander Rossiter on 27/03/2021.
//

#include <stdexcept>
#include "Patch.h"
#include "../Solving/Solver.h"
void PeriodicPatch::apply(Solver& solver) {
    Block& b1 = solver.g.get_block_by_id(bid);
    std::unique_ptr<Patch>& nxp = solver.g.get_patch_by_id(nxpid);
    Block& b2 = solver.g.get_block_by_id(nxbid);

    for (int i = 0; i < extent.ien-extent.ist; i++) {
        for (int j = 0; j < extent.jen-extent.jst; j++) {
            for (int k = 0; k < extent.ken-extent.kst; k++) {
                // next i = +i, next j = +j, next k = +k
                b1.ro[i+extent.ist][j+extent.jst][k+extent.kst] = b2.ro[i+nxp->extent.ist][j+nxp->extent.jst][k+nxp->extent.kst];
            }
        }
    }
}

std::string PeriodicPatch::to_string() {
    std::string str(std::string("Periodic: bid: ") + std::to_string(bid) + std::string(", nxbid: ") + std::to_string(nxbid) + std::string(", ") + extent.to_string() + std::string(", ") + nextDir.to_string());
    return str;
}

void PeriodicPatch::alter_block_iteration_extent(Block &b) {
    switch (face) {
        case 0:
            b.ist = 0;
            break;
        case 1:
            b.jst = 0;
            break;
        case 2:
            b.kst = 0;
            break;
        case 3:
            b.ien += 1;
            break;
        case 4:
            b.jen += 1;
            break;
        case 5:
            b.ken += 1;
            break;
    }
}

void ExitPatch::apply(Solver& solver) {
    // Set the static pressure on the exit face to p_down.
    Block& b = solver.g.get_block_by_id(bid);
    for (int i = extent.ist; i < extent.ien; i++) {
        for (int j = extent.jst; j < extent.jen; j++) {
            for (int k = extent.kst; k < extent.ken; k++) {
                b.pstat[i][j][k] = p_exit;
            }
        }
    }
}

std::string ExitPatch::to_string() {
    std::string str(std::string("Exit: bid: ") + std::to_string(bid) + std::string(" ") + extent.to_string() + std::string(" p_exit: ") + std::to_string(p_exit));
    return str;
}

void InletPatch::apply(Solver& solver) {
    Block& b = solver.g.get_block_by_id(bid);
    for (int i = extent.ist; i < extent.ien; i++) {
        for (int j = extent.jst; j < extent.jen; j++) {
            for (int k = extent.kst; k < extent.ken; k++) {
                //std::cout << i << " " << j << " " << k << std::endl;
                // Stagnation density.
                float ro_stag = conditions.Po / solver.gas.R / conditions.To;
                float ro_new = b.ro[i][j][k];

                // If this is the first iteration we don't have a ro_(n-1) so we just use the
                // current ro.
                if (solver.nstep > 0) {
                    ro_new = rfin * b.ro[i][j][k] + (1-rfin) * ro_nm1;
                }
                ro_nm1 = b.ro[i][j][k];

                // If our static density is greater than stagnation density
                // set it to be just less. This avoids needless NaNs during
                // transients.
                if (ro_new > ro_stag) {
                    ro_new = 0.9999f * ro_stag;
                }
                // Calculate the secondary variables.
                float tstat = conditions.To * pow(ro_new / ro_stag, solver.gas.ga-1);
                float vel = sqrt(2*solver.gas.cv*(conditions.To-tstat));
                float E = solver.gas.cv*tstat + 0.5f*vel*vel;
                b.vx[i][j][k] = vel * cos(conditions.yaw*M_PI/180) * cos(conditions.pitch*M_PI/180);
                b.vy[i][j][k] = vel * sin(conditions.yaw*M_PI/180);
                b.vz[i][j][k] = vel * cos(conditions.yaw*M_PI/180) * sin(conditions.pitch*M_PI/180);

                // Update the primary variables.
                b.ro[i][j][k] = ro_new;
                b.rovx[i][j][k] = b.ro[i][j][k]*b.vx[i][j][k];
                b.rovy[i][j][k] = b.ro[i][j][k]*b.vy[i][j][k];
                b.rovz[i][j][k] = b.ro[i][j][k]*b.vz[i][j][k];
                b.roe[i][j][k]  = b.ro[i][j][k]*E;

            }
        }
    }
}

std::string InletPatch::to_string() {
    std::string str(std::string("Inlet: bid: ") + std::to_string(bid) + std::string(" ") + extent.to_string() + std::string(" ") + conditions.to_string());
    return str;
}

void Patch::shift_patch_extent(int ishift, int jshift, int kshift) {
    extent.ist += ishift;
    extent.jst += jshift;
    extent.kst += kshift;
    extent.ien += ishift;
    extent.jen += jshift;
    extent.ken += kshift;
}

void Patch::find_face() {
    if ((extent.ist == extent.ien-1) && (extent.jst != extent.jen-1) && (extent.kst != extent.ken-1)){
        if (extent.ist == 0) {
            face = 0;
        } else {
            face = 3;
        }
    } else if ((extent.ist != extent.ien-1) && (extent.jst == extent.jen-1) && (extent.kst != extent.ken-1)) {
        if (extent.ist == 0) {
            face = 1;
        } else {
            face = 4;
        }
    } else if ((extent.ist != extent.ien-1) && (extent.jst != extent.jen-1) && (extent.kst == extent.ken-1)) {
        if (extent.kst == 0) {
            face = 2;
        } else {
            face = 5;
        }
    } else {
        throw std::invalid_argument("Invalid patch extent");
    }
}
