//
// Created by Alexander Rossiter on 27/03/2021.
//

#include <stdexcept>
#include "Patch.h"
#include "../Solving/Solver.h"
void PeriodicPatch::apply(Solver& solver) {
    Block& b1 = solver.g.get_block_by_id(bid);
    std::unique_ptr<Patch>& nxp = solver.g.get_patch_by_id(nxpid, nxbid);
    Block& b2 = solver.g.get_block_by_id(nxbid);
    for (int i = 0; i < extent.ien-extent.ist; i++) {
        for (int j = 0; j < extent.jen-extent.jst; j++) {
            for (int k = 0; k < extent.ken-extent.kst; k++) {
                // Sort out periodic patches.

                int pi = i+extent.ist;
                int pj = j+extent.jst;
                int pk = k+extent.kst;

                int nxpi = nxp->extent.ist   + i;
                int nxpj = nxp->extent.jst   + j;
                int nxpk = nxp->extent.kst   + k;

                if      (nextDir.nexti=="+I") nxpi = nxp->extent.ist   + i;
                else if (nextDir.nexti=="-I") nxpi = nxp->extent.ien-1 - i;
                else if (nextDir.nexti=="+J") nxpj = nxp->extent.jst   + i;
                else if (nextDir.nexti=="-J") nxpj = nxp->extent.jen-1 - i;
                else if (nextDir.nexti=="+K") nxpk = nxp->extent.kst   + i;
                else if (nextDir.nexti=="-K") nxpk = nxp->extent.ken-1 - i;

                if      (nextDir.nextj=="+I") nxpi = nxp->extent.ist   + j;
                else if (nextDir.nextj=="-I") nxpi = nxp->extent.ien-1 - j;
                else if (nextDir.nextj=="+J") nxpj = nxp->extent.jst   + j;
                else if (nextDir.nextj=="-J") nxpj = nxp->extent.jen-1 - j;
                else if (nextDir.nextj=="+K") nxpk = nxp->extent.kst   + j;
                else if (nextDir.nextj=="-K") nxpk = nxp->extent.ken-1 - j;

                if      (nextDir.nextk=="+I") nxpi = nxp->extent.ist   + k;
                else if (nextDir.nextk=="-I") nxpi = nxp->extent.ien-1 - k;
                else if (nextDir.nextk=="+J") nxpj = nxp->extent.jst   + k;
                else if (nextDir.nextk=="-J") nxpj = nxp->extent.jen-1 - k;
                else if (nextDir.nextk=="+K") nxpk = nxp->extent.kst   + k;
                else if (nextDir.nextk=="-K") nxpk = nxp->extent.ken-1 - k;

                for (auto& [key, val]: b1.primary_vars) {
                    b1.primary_vars[key][pi][pj][pk] = 0.5*(b1.primary_vars[key][pi][pj][pk]+b2.primary_vars[key][nxpi][nxpj][nxpk]);
                    b2.primary_vars[key][nxpi][nxpj][nxpk] = b1.primary_vars[key][pi][pj][pk];
                }

                for (auto& [key, val]: b1.secondary_vars) {
                    b1.secondary_vars[key][pi][pj][pk] = 0.5*(b1.secondary_vars[key][pi][pj][pk]+b2.secondary_vars[key][nxpi][nxpj][nxpk]);
                    b2.secondary_vars[key][nxpi][nxpj][nxpk] = b1.secondary_vars[key][pi][pj][pk];

                }

// This sets the fluxes to be periodic, I don't think we need it though.
//                for (auto& [key, val]: b1.c_fluxes) {
//                    for (int faceId=0; faceId < 3; faceId++){
//                        b1.c_fluxes[key][faceId][i+extent.ist][j+extent.jst][k+extent.kst] = b2.c_fluxes[key][faceId][i+nxp->extent.ist][j+nxp->extent.jst][k+nxp->extent.kst];
//                        if (face == 2) {
//                            b1.c_fluxes[key][faceId][i+extent.ist][j+extent.jst][k+extent.kst-1] = b2.c_fluxes[key][faceId][i+nxp->extent.ist][j+nxp->extent.jst][k+nxp->extent.kst-1];
//                        } else if (face == 5) {
//                            b1.c_fluxes[key][faceId][i+extent.ist][j+extent.jst][k+extent.kst+1] = b2.c_fluxes[key][faceId][i+nxp->extent.ist][j+nxp->extent.jst][k+nxp->extent.kst+1];
//                        }
//                    }
//                }

            }
        }
    }
}

std::string PeriodicPatch::to_string() {
    std::string str(std::string("Periodic: bid: ") + std::to_string(bid) + std::string(", pid: ") + std::to_string(pid) + std::string(", nxbid: ") + std::to_string(nxbid)+ std::string(", nxpid: ") + std::to_string(nxpid) + std::string(", ") + extent.to_string() + std::string(", ") + nextDir.to_string());
    return str;
}

void PeriodicPatch::alter_block_iteration_extent(Grid& g) {
    // Should be able to remove this now.
    Block &b1 = g.get_block_by_id(bid);
    Block &b2 = g.get_block_by_id(nxbid);
    std::unique_ptr<Patch> &p2 = g.get_patch_by_id(nxpid, nxbid);
    switch (face) {
        case 0:
            b1.ist = 0;
            for (int j = b1.jst; j < b1.jen; j++) {
                for (int k = b1.kst; k < b1.ken; k++) {
                    b1.volume[0][j][k] = b2.volume[p2->extent.ist-1][j][k];
                    b1.geom[0][j][k] = b2.geom[p2->extent.ist-1][j][k];
                }
            }
            break;
        case 1:
            b1.jst = 0;
            for (int i = b1.ist; i < b1.ien; i++) {
                for (int k = b1.kst; k < b1.ken; k++) {
                    b1.volume[i][0][k] = b2.volume[i][p2->extent.jst-1][k];
                    b1.geom[i][0][k] = b2.geom[i][p2->extent.jst-1][k];
                }
            }
            break;
        case 2:
            b1.kst = 0;
            for (int i = b1.ist; i < b1.ien; i++) {
                for (int j = b1.jst; j < b1.jen; j++) {
                    b1.volume[i][j][0] = b2.volume[i][j][p2->extent.kst - 1];
                    b1.geom[i][j][0] = b2.geom[i][j][p2->extent.kst - 1];
                }
            }
            break;
        case 3:
            b1.ien += 1;
            for (int j = b1.jst; j < b1.jen; j++) {
                for (int k = b1.kst; k < b1.ken; k++) {
                    b1.volume[b1.ien-2][j][k] = b2.volume[p2->extent.ist][j][k];
                    b1.geom[b1.ien-2][j][k] = b2.geom[p2->extent.ist][j][k];
                }
            }
            break;
        case 4:
            b1.jen += 1;
            for (int i = b1.ist; i < b1.ien; i++) {
                for (int k = b1.kst; k < b1.ken; k++) {
                    b1.volume[i][b1.jen-2][k] = b2.volume[i][p2->extent.jst][k];
                    b1.geom[i][b1.jen-2][k] = b2.geom[i][p2->extent.jst][k];
                }
            }
            break;
        case 5:
            b1.ken += 1;
            for (int i = b1.ist; i < b1.ien; i++) {
                for (int j = b1.jst; j < b1.jen; j++) {
                    b1.volume[i][j][b1.ken - 2] = b2.volume[i][j][p2->extent.kst];
                    b1.geom[i][j][b1.ken - 2] = b2.geom[i][j][p2->extent.kst];
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
    std::string str(std::string("Exit: bid: ") + std::to_string(bid) + std::string(", pid: ") + std::to_string(pid) + std::string(" ") + extent.to_string() + std::string(" p_exit: ") + std::to_string(p_exit));
    return str;
}

void InletPatch::apply(Solver& solver) {

    Block& b = solver.g.get_block_by_id(bid);
    for (int i = extent.ist; i < extent.ien; i++) {
        for (int j = extent.jst; j < extent.jen; j++) {
            for (int k = extent.kst; k < extent.ken; k++) {
                // Stagnation density.
                double ro_stag = conditions.Po / solver.gas.R / conditions.To;
                double ro_new = b.primary_vars["ro"][i][j][k];

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
                double tstat = conditions.To * pow(ro_new / ro_stag, solver.gas.ga-1);
                double vel = sqrt(2*solver.gas.cp*(conditions.To-tstat));
                double E = solver.gas.cv*tstat + 0.5f*vel*vel;
                b.secondary_vars["vx"][i][j][k] = vel * cos(conditions.yaw*M_PI/180) * cos(conditions.pitch*M_PI/180);
                b.secondary_vars["vy"][i][j][k] = vel * sin(conditions.yaw*M_PI/180);
                b.secondary_vars["vz"][i][j][k] = vel * cos(conditions.yaw*M_PI/180) * sin(conditions.pitch*M_PI/180);
                // Update the primary variables.
                b.primary_vars["ro"][i][j][k]   = ro_new;
                b.primary_vars["rovx"][i][j][k] = b.primary_vars["ro"][i][j][k]*b.secondary_vars["vx"][i][j][k];
                b.primary_vars["rovy"][i][j][k] = b.primary_vars["ro"][i][j][k]*b.secondary_vars["vy"][i][j][k];
                b.primary_vars["rovz"][i][j][k] = b.primary_vars["ro"][i][j][k]*b.secondary_vars["vz"][i][j][k];
                b.primary_vars["roe"][i][j][k]  = b.primary_vars["ro"][i][j][k]*E;

            }
        }
    }
}

std::string InletPatch::to_string() {
    std::string str(std::string("Inlet: bid: ") + std::to_string(bid) + std::string(", pid: ") + std::to_string(pid) + std::string(" ") + extent.to_string() + std::string(" ") + conditions.to_string());
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
