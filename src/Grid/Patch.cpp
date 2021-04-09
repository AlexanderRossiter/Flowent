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
                // Loop over variables
                for (auto& [key, val]: b1.primary_vars) {
                    b1.primary_vars[key][i+extent.ist][j+extent.jst][k+extent.kst] = b2.primary_vars[key][i+nxp->extent.ist][j+nxp->extent.jst][k+nxp->extent.kst];
                }
                for (auto& [key, val]: b1.secondary_vars) {
                    b1.secondary_vars[key][i+extent.ist][j+extent.jst][k+extent.kst] = b2.secondary_vars[key][i+nxp->extent.ist][j+nxp->extent.jst][k+nxp->extent.kst];
                }
                for (auto& [key, val]: b1.c_fluxes) {
                    for (int faceId=0; faceId < 3; faceId++)
                        b1.c_fluxes[key][faceId][i+extent.ist][j+extent.jst][k+extent.kst] = b2.c_fluxes[key][faceId][i+nxp->extent.ist][j+nxp->extent.jst][k+nxp->extent.kst];
                }

            }
        }
    }
}

std::string PeriodicPatch::to_string() {
    std::string str(std::string("Periodic: bid: ") + std::to_string(bid) + std::string(", nxbid: ") + std::to_string(nxbid) + std::string(", ") + extent.to_string() + std::string(", ") + nextDir.to_string());
    return str;
}

void PeriodicPatch::alter_block_iteration_extent(Grid& g) {
    // This only works for standard periodic patches atm, i.e. nexti=+I, nextj=+J, nextk=+K
    Block& b1 = g.get_block_by_id(bid);
    Block& b2 = g.get_block_by_id(nxbid);
    std::unique_ptr<Patch>& p2 = g.get_patch_by_id(nxpid);
    switch (face) {
        case 0:
            b1.ist = 0;
            break;
        case 1:
            b1.jst = 0;
            break;
        case 2:
            b1.kst = 0;
            for (int i = b1.ist; i < b1.ien; i++) {
                for (int j = b1.jst; j < b1.jen; j++) {
                    b1.volume[i][j][0] = b2.volume[i][j][p2->extent.kst-1];
                    b1.geom[i][j][0] = b2.geom[i][j][p2->extent.kst-1];
                }
            }
            break;
        case 3:
            b1.ien += 1;
            break;
        case 4:
            b1.jen += 1;
            break;
        case 5:
            b1.ken += 1;
            for (int i = b1.ist; i < b1.ien; i++) {
                for (int j = b1.jst; j < b1.jen; j++) {
                    b1.volume[i][j][b1.ken-2] = b2.volume[i][j][p2->extent.kst];
                    b1.geom[i][j][b1.ken-2]   = b2.geom[i][j][p2->extent.kst];
                }
            }
            break;
    }
}

void ExitPatch::apply(Solver& solver) {
    // Set the static pressure on the exit face to p_down.
    Block& b = solver.g.get_block_by_id(bid);
    for (int i = extent.ist; i < extent.ien; i++) {
        for (int j = extent.jst; j < extent.jen; j++) {
            for (int k = extent.kst; k < extent.ken; k++) {
                b.secondary_vars["pstat"][i][j][k] = p_exit;
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
                // Stagnation density.
                float ro_stag = conditions.Po / solver.gas.R / conditions.To;
                float ro_new = b.primary_vars["ro"][i][j][k];

                // If this is the first iteration we don't have a ro_(n-1) so we just use the
                // current ro.
                if (solver.nstep > 0) {
                    ro_new = rfin * b.primary_vars["ro"][i][j][k] + (1-rfin) * ro_nm1[i][j][k];
                }
                ro_nm1[i][j][k] = b.primary_vars["ro"][i][j][k];

                // If our static density is greater than stagnation density
                // set it to be just less. This avoids needless NaNs during
                // transients.
                if (ro_new > ro_stag) {
                    ro_new = 0.9999f * ro_stag;
                }

                // Calculate the secondary variables.
                float tstat = conditions.To * pow(ro_new / ro_stag, solver.gas.ga-1);
                float vel = sqrt(2*solver.gas.cp*(conditions.To-tstat));
                float E = solver.gas.cv*tstat + 0.5f*vel*vel;
                b.secondary_vars["vx"][i][j][k] = vel * cos(conditions.yaw*M_PI/180) * cos(conditions.pitch*M_PI/180);
                b.secondary_vars["vy"][i][j][k] = vel * sin(conditions.yaw*M_PI/180);
                b.secondary_vars["vz"][i][j][k] = vel * cos(conditions.yaw*M_PI/180) * sin(conditions.pitch*M_PI/180);
                // Update the primary variables.
                b.primary_vars["ro"][i][j][k] = ro_new;
                b.primary_vars["rovx"][i][j][k] = b.primary_vars["ro"][i][j][k]*b.secondary_vars["vx"][i][j][k];
                b.primary_vars["rovy"][i][j][k] = b.primary_vars["ro"][i][j][k]*b.secondary_vars["vy"][i][j][k];
                b.primary_vars["rovz"][i][j][k] = b.primary_vars["ro"][i][j][k]*b.secondary_vars["vz"][i][j][k];
                b.primary_vars["roe"][i][j][k]  = b.primary_vars["ro"][i][j][k]*E;

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
