//
// Created by Alexander Rossiter on 30/03/2021.
//

#include "Solver.h"
#include <cmath>
#include <iomanip>
#include <thread>

void Solver::run_nsteps(int nsteps) {
    Solver::set_timestep(300);

    while (nstep < nsteps) {
        // Set the secondary variables for the first iteration.
        run_iteration();
        if (nstep % 5 == 0) {
            std::cout << "TIME STEP NUMBER: " << nstep << std::endl;
            for (Block& b : g.get_blocks()) {
                b.calc_average_residual();
                std::cout << "Block " << b.id << " - DelRoAvg: " << b.del_ro_avg << std::endl;
            }
            std::cout << "\n";
        }
        nstep++;
    }
}

void Solver::run_iteration() {
    // Set the fluxes through the ijk faces in the domain.
    // Multithread
    for (Block& b : g.get_blocks()) {
        Solver::set_secondary_variables(b);
    }
    Solver::apply_boundary_conditions();

    std::vector<std::thread> threads;
    for (Block& b : g.get_blocks()) {
        std::thread t(calculate_block, std::ref(b), std::ref(sp), delta_t);
        threads.push_back(std::move(t));
    }

    for (std::thread &thr : threads) {
        if (thr.joinable()) {
            thr.join();
        }
    }


}

void Solver::calculate_block(Block& b, SolutionParameters& sp, double delta_t) {
    Extent e{};

    e = {b.ist, b.ien, b.jst, b.jen-1, b.kst, b.ken-1};
    Solver::set_mass_fluxes(b, 0, e);

    e = {b.ist, b.ien-1, b.jst, b.jen, b.kst, b.ken-1};
    Solver::set_mass_fluxes(b, 1, e);

    e = {b.ist, b.ien-1, b.jst, b.jen-1, b.kst, b.ken};
    Solver::set_mass_fluxes(b, 2, e);
    Solver::set_wall_bconds(b);


    // I-faces
    e = {b.ist, b.ien, b.jst, b.jen-1, b.kst, b.ken-1};
    Solver::set_convective_fluxes(b, b.c_fluxes["hstag"], "hstag", 0, e);
    Solver::set_convective_fluxes(b, b.c_fluxes["vx"], "vx", 0, e);
    Solver::set_convective_fluxes(b, b.c_fluxes["vy"], "vy", 0, e);
    Solver::set_convective_fluxes(b, b.c_fluxes["vz"], "vz", 0, e);

    // J-faces
    e = {b.ist, b.ien-1, b.jst, b.jen, b.kst, b.ken-1};
    Solver::set_convective_fluxes(b, b.c_fluxes["hstag"], "hstag", 1, e);
    Solver::set_convective_fluxes(b, b.c_fluxes["vx"], "vx", 1, e);
    Solver::set_convective_fluxes(b, b.c_fluxes["vy"], "vy", 1, e);
    Solver::set_convective_fluxes(b, b.c_fluxes["vz"], "vz", 1, e);

    // K-faces
    e = {b.ist, b.ien-1, b.jst, b.jen-1, b.kst, b.ken};
    Solver::set_convective_fluxes(b, b.c_fluxes["hstag"], "hstag", 2, e);
    Solver::set_convective_fluxes(b, b.c_fluxes["vx"], "vx", 2, e);
    Solver::set_convective_fluxes(b, b.c_fluxes["vy"], "vy", 2, e);
    Solver::set_convective_fluxes(b, b.c_fluxes["vz"], "vz", 2, e);


    Solver::sum_convective_fluxes(b, b.primary_vars["ro"],   b.c_fluxes["mass"],  b.residuals["ro"], sp, delta_t);
    Solver::sum_convective_fluxes(b, b.primary_vars["rovx"], b.c_fluxes["vx"],    b.residuals["rovx"], sp, delta_t);
    Solver::sum_convective_fluxes(b, b.primary_vars["rovy"], b.c_fluxes["vy"],    b.residuals["rovy"], sp, delta_t);
    Solver::sum_convective_fluxes(b, b.primary_vars["rovz"], b.c_fluxes["vz"],    b.residuals["rovz"], sp, delta_t);
    Solver::sum_convective_fluxes(b, b.primary_vars["roe"],  b.c_fluxes["hstag"], b.residuals["roe"], sp, delta_t);


    Solver::smooth_defcorr(b, b.primary_vars["ro"], b.corr_primary_vars["ro"], sp);
    Solver::smooth_defcorr(b, b.primary_vars["rovx"], b.corr_primary_vars["rovx"], sp);
    Solver::smooth_defcorr(b, b.primary_vars["rovy"], b.corr_primary_vars["rovy"], sp);
    Solver::smooth_defcorr(b, b.primary_vars["rovz"], b.corr_primary_vars["rovz"], sp);
    Solver::smooth_defcorr(b, b.primary_vars["roe"], b.corr_primary_vars["roe"], sp);
}

void Solver::apply_boundary_conditions() {
    //std::cout << "Applying patch boundary conditions.\n";
    //Apply the patch boundary conditions.
    for (auto& p : g.get_patches()) {
        //std::cout << p->to_string() << std::endl;
        p->apply(*this);
    }
}

void Solver::test_for_nans(Block& b) {
    for (int i = b.ist; i < b.ien; i++) {
        for (int j = b.jst; j < b.jen; j++) {
            for (int k = b.kst; k < b.ken; k++) {
                for (auto &entry : b.primary_vars) {
                    if (std::isnan(entry.second[i][j][k])) {
                        std::cout << "NAN in " << entry.first << " at (i,j,k) = (" << i << ", " << j << ", " << k << ")" << std::endl;
                    }
                }
            }
        }
    }
}

void Solver::set_secondary_variables(Block& b) const {
    //std::cout << "Setting secondary variables\n";
    vector3d<double>& ro = b.primary_vars["ro"];
    vector3d<double>& rovx = b.primary_vars["rovx"];
    vector3d<double>& rovy = b.primary_vars["rovy"];
    vector3d<double>& rovz = b.primary_vars["rovz"];
    vector3d<double>& roe = b.primary_vars["roe"];
    vector3d<double>& pstat = b.secondary_vars["pstat"];
    vector3d<double>& vx = b.secondary_vars["vx"];
    vector3d<double>& vy = b.secondary_vars["vy"];
    vector3d<double>& vz = b.secondary_vars["vz"];
    vector3d<double>& hstag = b.secondary_vars["hstag"];

        for (int i = b.ist; i < b.ien; i++) {
            for (int j = b.jst; j < b.jen; j++) {
                for (int k = b.kst; k < b.ken; k++) {
                    vx[i][j][k] = rovx[i][j][k] / ro[i][j][k];
                    vy[i][j][k] = rovy[i][j][k] / ro[i][j][k];
                    vz[i][j][k] = rovz[i][j][k] / ro[i][j][k];

                    double velSq = vx[i][j][k]*vx[i][j][k] +
                            vy[i][j][k]*vy[i][j][k] +
                            vz[i][j][k]*vz[i][j][k];
                    double EKE = 0.5f*velSq;
                    double tstat = (roe[i][j][k] / ro[i][j][k] - EKE) / gas.cv;
                    pstat[i][j][k] = ro[i][j][k] * gas.R * tstat;//(gas.ga-1) * (roe[i][j][k] - EKE*ro[i][j][k]);
                    hstag[i][j][k] = (roe[i][j][k] + pstat[i][j][k]) / ro[i][j][k];
                }
            }
        }

}

void Solver::set_mass_fluxes(Block& b, int faceId, Extent& extent) {
    // Assign these by reference here so we don't have to do many map lookups.
    vector3d<double>& rovx = b.primary_vars["rovx"];
    vector3d<double>& rovy = b.primary_vars["rovy"];
    vector3d<double>& rovz = b.primary_vars["rovz"];
    vectornd<double,4>& flux = b.c_fluxes["mass"];

    for (int i = extent.ist; i < extent.ien; i++) {
        for (int j = extent.jst; j < extent.jen; j++) {
            for (int k = extent.kst; k < extent.ken; k++) {

                // Make sure the flux begins at 0.
                flux[faceId][i][j][k]=0;

                // Calculate the average value on this face
                double face_avg_rovx = util::calculate_face_average(rovx, faceId, i, j, k);
                double face_avg_rovy = util::calculate_face_average(rovy, faceId, i, j, k);
                double face_avg_rovz = util::calculate_face_average(rovz, faceId, i, j, k);

                // Sum the x,y,z  components of the flux through the i-face for this cell.
                flux[faceId][i][j][k] += face_avg_rovx * b.geom[i][j][k].Ax[faceId];
                flux[faceId][i][j][k] += face_avg_rovy * b.geom[i][j][k].Ay[faceId];
                flux[faceId][i][j][k] += face_avg_rovz * b.geom[i][j][k].Az[faceId];

            }
        }
    }
}

void Solver::set_convective_fluxes(Block& b, vectornd<double,4>& flux, std::string varName, int faceId, Extent& extent) {
    // Assign these by reference here so we don't have to do many map lookups.
//    vector3d<double>& vx = b.secondary_vars["vx"];
//    vector3d<double>& vy = b.secondary_vars["vy"];
//    vector3d<double>& vz = b.secondary_vars["vz"];
    vector3d<double>& pstat = b.secondary_vars["pstat"];
    vector3d<double>& s_var = b.secondary_vars[varName];
    vectornd<double, 4>& mdot = b.c_fluxes["mass"];

    for (int i = extent.ist; i < extent.ien; i++) {
        for (int j = extent.jst; j < extent.jen; j++) {
            for (int k = extent.kst; k < extent.ken; k++) {

                // Make sure the flux begins at 0.
                flux[faceId][i][j][k]=0;

                // Calculate the average value on this face
                double face_avg_secondary = util::calculate_face_average(s_var, faceId, i, j, k);

                // Sum the x,y,z  components of the flux through the i-face for this cell.
                flux[faceId][i][j][k] += mdot[faceId][i][j][k] * face_avg_secondary;

                // If this is momentum flux we also include the relevant PdA term.
                if (varName.at(1) == 'x') {
                    flux[faceId][i][j][k] += util::calculate_face_average(pstat, faceId, i, j, k) * b.geom[i][j][k].Ax[faceId];
                } else if (varName.at(1) == 'y') {
                    flux[faceId][i][j][k] += util::calculate_face_average(pstat, faceId, i, j, k) * b.geom[i][j][k].Ay[faceId];
                } else if(varName.at(1) == 'z') {
                    flux[faceId][i][j][k] += util::calculate_face_average(pstat, faceId, i, j, k) * b.geom[i][j][k].Az[faceId];
                }

            }
        }
    }
}

void Solver::set_timestep(double tstag) {
    double a_sound = sqrt(gas.ga * gas.R * tstag);
    double U = a_sound;
    delta_t = sp.cfl * cbrt(g.minV) / (U + a_sound);
}


void Solver::sum_convective_fluxes(Block &b, vector3d<double>& phi, vectornd<double, 4>& flux, vector3d<double>& residual, SolutionParameters& sp, double delta_t) {
    //std::cout << "Summing fluxes\n";

    // First, calculate the change in the variable over the timestep dt.
    // Loop over the cells
    vector3d<double> delta_phi(boost::extents[b.ni+2][b.nj+2][b.nk+2]);
    vector3d<double> store(boost::extents[b.ni+2][b.nj+2][b.nk+2]);
    for (int i = b.ist; i < b.ien-1; i++) {
        for (int j = b.jst; j < b.jen-1; j++) {
            for (int k = b.kst; k < b.ken-1; k++) {
                store[i][j][k] = -delta_t / b.volume[i][j][k] * (flux[0][i][j][k]-flux[0][i+1][j][k] +
                        flux[1][i][j][k]-flux[1][i][j+1][k] + flux[2][i][j][k]-flux[2][i][j][k+1]);

                // Inclusion of a second order time derivative.
                delta_phi[i][j][k] = store[i][j][k] + sp.facsec*(store[i][j][k] - residual[i][j][k]);
                residual[i][j][k] = store[i][j][k];
            }
        }
    }

    // Now we distribute the changes in the variable to the 8 corners of the cell.
    // Interior nodes:
    for (int i = b.ist+1; i < b.ien-1; i++) {
        for (int j = b.jst+1; j < b.jen-1; j++) {
            for (int k = b.kst+1; k < b.ken-1; k++) {
                phi[i][j][k] += 0.125f * (delta_phi[i][j][k] + delta_phi[i-1][j][k] + delta_phi[i-1][j-1][k] + delta_phi[i][j-1][k] +
                        delta_phi[i][j][k-1] + delta_phi[i-1][j][k-1] + delta_phi[i-1][j-1][k-1] + delta_phi[i][j-1][k-1]);
            }
        }
    }

    // j=const boundaries
    for (int i = b.ist+1; i < b.ien-1; i++) {
        for (int k = b.kst+1; k < b.ken-1; k++) {
            phi[i][b.jst][k]   += 0.25f * (delta_phi[i][b.jst][k] + delta_phi[i-1][b.jst][k] + delta_phi[i][b.jst][k-1] + delta_phi[i-1][b.jst][k-1]);
            phi[i][b.jen-1][k] += 0.25f * (delta_phi[i][b.jen-2][k] + delta_phi[i-1][b.jen-2][k] + delta_phi[i][b.jen-2][k-1] + delta_phi[i-1][b.jen-2][k-1]);
        }
    }

    // k=const boundaries
    for (int i = b.ist+1; i < b.ien-1; i++) {
        for (int j = b.jst+1; j < b.jen-1; j++) {
            phi[i][j][b.kst]   += 0.25f * (delta_phi[i][j][b.kst] + delta_phi[i-1][j][b.kst] + delta_phi[i][j-1][b.kst] + delta_phi[i-1][j-1][b.kst]);
            phi[i][j][b.ken-1] += 0.25f * (delta_phi[i][j][b.ken-2] + delta_phi[i-1][j][b.ken-2] + delta_phi[i][j-1][b.ken-2] + delta_phi[i-1][j-1][b.ken-2]);
        }
    }

    // i=const boundaries
    for (int j = b.jst+1; j < b.jen-1; j++) {
        for (int k = b.kst+1; k < b.ken-1; k++) {
            phi[b.ist][j][k]   += 0.25f * (delta_phi[b.ist][j][k] + delta_phi[b.ist][j-1][k] + delta_phi[b.ist][j][k-1] + delta_phi[b.ist][j-1][k-1]);
            phi[b.ien-1][j][k] += 0.25f * (delta_phi[b.ien-2][j][k] + delta_phi[b.ien-2][j-1][k] + delta_phi[b.ien-2][j][k-1] + delta_phi[b.ien-2][j-1][k-1]);
        }
    }

    // j=const i lines
    for (int i = b.ist+1; i < b.ien-1; i++) {
        phi[i][b.jst][b.kst]     += 0.5f * (delta_phi[i][b.jst][b.kst] + delta_phi[i-1][b.jst][b.kst]);
        phi[i][b.jen-1][b.kst]   += 0.5f * (delta_phi[i][b.jen-2][b.kst] + delta_phi[i-1][b.jen-2][b.kst]);
        phi[i][b.jst][b.ken-1]   += 0.5f * (delta_phi[i][b.jst][b.ken-2] + delta_phi[i-1][b.jst][b.ken-2]);
        phi[i][b.jen-1][b.ken-1] += 0.5f * (delta_phi[i][b.jen-2][b.ken-2] + delta_phi[i-1][b.jen-2][b.ken-2]);
    }


    // i=const k lines
    for (int k = b.kst+1; k < b.ken-1; k++) {
        phi[b.ist][b.jst][k]     += 0.5f * (delta_phi[b.ist][b.jst][k] + delta_phi[b.ist][b.jst][k-1]);
        phi[b.ist][b.jen-1][k]   += 0.5f * (delta_phi[b.ist][b.jen-2][k] + delta_phi[b.ist][b.jen-2][k-1]);
        phi[b.ien-1][b.jst][k]   += 0.5f * (delta_phi[b.ien-2][b.jst][k] + delta_phi[b.ien-2][b.jst][k-1]);
        phi[b.ien-1][b.jen-1][k] += 0.5f * (delta_phi[b.ien-2][b.jen-2][k] + delta_phi[b.ien-2][b.jen-2][k-1]);
    }

    // k=const j lines
    for (int j = b.jst+1; j < b.jen-1; j++) {
        phi[b.ist][j][b.kst]     += 0.5f * (delta_phi[b.ist][j][b.kst] + delta_phi[b.ist][j-1][b.kst]);
        phi[b.ist][j][b.ken-1]   += 0.5f * (delta_phi[b.ist][j][b.ken-2] + delta_phi[b.ist][j-1][b.ken-2]);
        phi[b.ien-1][j][b.kst]   += 0.5f * (delta_phi[b.ien-2][j][b.kst] + delta_phi[b.ien-2][j-1][b.kst]);
        phi[b.ien-1][j][b.ken-1] += 0.5f * (delta_phi[b.ien-2][j][b.ken-2] + delta_phi[b.ien-2][j-1][b.ken-2]);
    }

    // Eight Corners
    phi[b.ist][b.jst][b.kst]       += delta_phi[b.ist][b.jst][b.kst];

    phi[b.ist][b.jen-1][b.kst]     += delta_phi[b.ist][b.jen-2][b.kst];

    phi[b.ist][b.jst][b.ken-1]     += delta_phi[b.ist][b.jst][b.ken-2];

    phi[b.ien-1][b.jst][b.kst]     += delta_phi[b.ien-2][b.jst][b.kst];

    phi[b.ien-1][b.jen-1][b.kst]   += delta_phi[b.ien-2][b.jen-2][b.kst];

    phi[b.ien-1][b.jst][b.ken-1]   += delta_phi[b.ien-2][b.jst][b.ken-2];

    phi[b.ist][b.jen-1][b.ken-1]   += delta_phi[b.ist][b.jen-2][b.ken-2];

    phi[b.ien-1][b.jen-1][b.ken-1] += delta_phi[b.ien-2][b.jen-2][b.ken-2];


//    for (int i = b.ist; i < b.ien-1; i++) {
//        for (int j = b.jst; j < b.jen - 1; j++) {
//            for (int k = b.kst; k < b.ken - 1; k++) {
//                residual[i][j][k] = delta_phi[i][j][k];
//            }
//        }
//    }

}

void Solver::set_wall_bconds(Block& b) {
    //// Loop over the edges of the block and see if the face is a wall.

    // i=const
    for (int j=b.jst; j < b.jen-1; j++) {
        for (int k = b.kst; k < b.ken - 1; k++) {
            if (b.isWall[b.ist][j][k] || b.isWall[b.ist][j + 1][k] ||
                b.isWall[b.ist][j][k + 1] || b.isWall[b.ist][j + 1][k + 1]) {
                b.c_fluxes["mass"][0][b.ist][j][k] = 0;
            }


            if (b.isWall[b.ien - 1][j][k] || b.isWall[b.ien - 1][j + 1][k] ||
                b.isWall[b.ien - 1][j][k + 1] || b.isWall[b.ien - 1][j + 1][k + 1]) {
                b.c_fluxes["mass"][0][b.ien - 1][j][k] = 0;

            }
        }
    }

    // j=const
    for (int i = b.ist; i < b.ien - 1; i++) {
        for (int k = b.kst; k < b.ken - 1; k++) {
            if (b.isWall[i][b.jst][k] || b.isWall[i + 1][b.jst][k] ||
                b.isWall[i][b.jst][k + 1] || b.isWall[i + 1][b.jst][k + 1]) {
                b.c_fluxes["mass"][1][i][b.jst][k] = 0;
            }

            if (b.isWall[i][b.jen - 1][k] || b.isWall[i + 1][b.jen - 1][k] ||
                b.isWall[i][b.jen - 1][k + 1] || b.isWall[i + 1][b.jen - 1][k + 1]) {
                b.c_fluxes["mass"][1][i][b.jen - 1][k] = 0;

            }
        }
    }

    // k=const
    for (int i = b.ist; i < b.ien - 1; i++) {
        for (int j = b.jst; j < b.jen - 1; j++) {
            if (b.isWall[i][j][b.kst] || b.isWall[i][j + 1][b.kst] ||
                b.isWall[i + 1][j][b.kst] || b.isWall[i + 1][j + 1][b.kst]) {
                b.c_fluxes["mass"][2][i][j][b.kst] = 0;
            }

            if (b.isWall[i][j][b.ken - 1] || b.isWall[i][j + 1][b.ken - 1] ||
                b.isWall[i + 1][j][b.ken - 1] || b.isWall[i + 1][j + 1][b.ken - 1]) {
                b.c_fluxes["mass"][2][i][j][b.ken - 1] = 0;
            }
        }
    }
}

void Solver::smooth(Block& b, vector3d<double>& phi) {

    // We make all the chages to variable store, this way we don't smooth with
    // smoothed values.
    vector3d<double> store(boost::extents[b.ni+2][b.nj+2][b.nk+2]);
    double sfm1 = 1-sp.sfin;

    // Start with internal nodes
    for (int i = b.ist+1; i < b.ien-1; i++) {
        for (int j = b.jst+1; j < b.jen-1; j++) {
            for (int k = b.kst+1; k < b.ken-1; k++) {
                double avg = 0.1666667f * (phi[i+1][j][k]+phi[i-1][j][k]
                        +phi[i][j+1][k]+phi[i][j-1][k]
                        +phi[i][j][k+1]+phi[i][j][k-1]);

                store[i][j][k] = sfm1*phi[i][j][k] + sp.sfin*avg;
            }
        }
    }

    // i=const
    for (int j = b.jst; j < b.jen; j++) {
        for (int k = b.kst; k < b.ken; k++) {
            int jp1 = j==b.jen-1 ? j : j+1;
            int kp1 = k==b.ken-1 ? k : k+1;
            int jm1 = j==b.jst ? j : j-1;
            int km1 = k==b.kst ? k : k-1;

            double avg = (0.2f*phi[b.ist][jp1][k] + 0.2f*phi[b.ist][jm1][k] +
                    0.2f*phi[b.ist][j][kp1] + 0.2f*phi[b.ist][j][km1] +
                    0.4f*phi[b.ist+1][j][k] - 0.2f*phi[b.ist+2][j][k]);
            store[b.ist][j][k] = sfm1*phi[b.ist][j][k] + sp.sfin*avg;

            avg = (0.2f*phi[b.ien-1][jp1][k] + 0.2f*phi[b.ien-1][jm1][k] +
                   0.2f*phi[b.ien-1][j][kp1] + 0.2f*phi[b.ien-1][j][km1] +
                   0.4f*phi[b.ien-2][j][k] - 0.2f*phi[b.ien-3][j][k]);
            store[b.ien-1][j][k] = sfm1*phi[b.ien-1][j][k] + sp.sfin*avg;

        }
    }

    // j=const
    for (int i = b.ist; i < b.ien; i++) {
        for (int k = b.kst; k < b.ken; k++) {
            int ip1 = i==b.ien-1 ? i : i+1;
            int kp1 = k==b.ken-1 ? k : k+1;
            int im1 = i==b.ist ? i : i-1;
            int km1 = k==b.kst ? k : k-1;

            double avg = (0.2f*phi[ip1][b.jst][k] + 0.2f*phi[im1][b.jst][k] +
                         0.2f*phi[i][b.jst][kp1] + 0.2f*phi[i][b.jst][km1] +
                         0.4f*phi[i][b.jst+1][k] - 0.2f*phi[i][b.jst+2][k]);
            store[i][b.jst][k] = sfm1*phi[i][b.jst][k] + sp.sfin*avg;

            avg = (0.2f*phi[ip1][b.jen-1][k] + 0.2f*phi[im1][b.jen-1][k] +
                   0.2f*phi[i][b.jen-1][kp1] + 0.2f*phi[i][b.jen-1][km1] +
                   0.4f*phi[i][b.jen-2][k] - 0.2f*phi[i][b.jen-3][k]);
            store[i][b.jen-1][k] = sfm1*phi[i][b.jen-1][k] + sp.sfin*avg;

        }
    }

    // k=const
    for (int i = b.ist; i < b.ien; i++) {
        for (int j = b.jst; j < b.jen; j++) {
            int ip1 = i==b.ien-1 ? i : i+1;
            int jp1 = j==b.jen-1 ? j : j+1;
            int im1 = i==b.ist ? i : i-1;
            int jm1 = j==b.jst ? j : j-1;

            double avg = (0.2f*phi[ip1][j][b.kst] + 0.2f*phi[im1][j][b.kst] +
                         0.2f*phi[i][jp1][b.kst] + 0.2f*phi[i][jm1][b.kst] +
                         0.4f*phi[i][j][b.kst+1] - 0.2f*phi[i][j][b.kst+2]);
            store[i][j][b.kst] = sfm1*phi[i][j][b.kst] + sp.sfin*avg;

            avg = (0.2f*phi[ip1][j][b.ken-1] + 0.2f*phi[im1][j][b.ken-1] +
                   0.2f*phi[i][jp1][b.ken-1] + 0.2f*phi[i][jm1][b.ken-1] +
                   0.4f*phi[i][j][b.ken-2] - 0.2f*phi[i][j][b.ken-3]);
            store[i][j][b.ken-1] = sfm1*phi[i][j][b.ken-1] + sp.sfin*avg;

        }
    }

    for (int i = b.ist; i < b.ien; i++) {
        for (int j = b.jst; j < b.jen; j++) {
            for (int k = b.kst; k < b.ken; k++) {
                phi[i][j][k] = store[i][j][k];
            }
        }
    }

}

void Solver::smooth_defcorr(Block& b, vector3d<double>& phi, vector3d<double>& corr_phi, SolutionParameters& sp) {

    // We make all the chages to variable store, this way we don't smooth with
    // smoothed values.
    vector3d<double> store(boost::extents[b.ni+2][b.nj+2][b.nk+2]);
    double sfm1 = 1-sp.sfin;
    double corr_new;
    double cf = 0.99;
    double cfm1 = 1-cf;

    // Start with internal nodes
    for (int i = b.ist+1; i < b.ien-1; i++) {
        for (int j = b.jst+1; j < b.jen-1; j++) {
            for (int k = b.kst+1; k < b.ken-1; k++) {
                double avg = 0.1666667f * (phi[i+1][j][k]+phi[i-1][j][k]
                                           +phi[i][j+1][k]+phi[i][j-1][k]
                                           +phi[i][j][k+1]+phi[i][j][k-1]);

                corr_new = sp.fcorr * (phi[i][j][k] - avg);
                corr_phi[i][j][k] = cf*corr_phi[i][j][k] + cfm1*corr_new;
                store[i][j][k]    = sfm1*phi[i][j][k] + sp.sfin*(avg+corr_phi[i][j][k]);
            }
        }
    }

    // i=const
    for (int j = b.jst; j < b.jen; j++) {
        for (int k = b.kst; k < b.ken; k++) {
            int jp1 = j==b.jen-1 ? j : j+1;
            int kp1 = k==b.ken-1 ? k : k+1;
            int jm1 = j==b.jst ? j : j-1;
            int km1 = k==b.kst ? k : k-1;

            double avg = (0.2f*phi[b.ist][jp1][k] + 0.2f*phi[b.ist][jm1][k] +
                          0.2f*phi[b.ist][j][kp1] + 0.2f*phi[b.ist][j][km1] +
                          0.4f*phi[b.ist+1][j][k] - 0.2f*phi[b.ist+2][j][k]);

            corr_new = sp.fcorr * (phi[b.ist][j][k] - avg);
            corr_phi[b.ist][j][k] = cf * corr_phi[b.ist][j][k] + cfm1 * corr_new;

            store[b.ist][j][k] = sfm1*phi[b.ist][j][k] + sp.sfin*(avg+corr_phi[b.ist][j][k]);

            avg = (0.2f*phi[b.ien-1][jp1][k] + 0.2f*phi[b.ien-1][jm1][k] +
                   0.2f*phi[b.ien-1][j][kp1] + 0.2f*phi[b.ien-1][j][km1] +
                   0.4f*phi[b.ien-2][j][k] - 0.2f*phi[b.ien-3][j][k]);

            corr_new = sp.fcorr * (phi[b.ien-1][j][k] - avg);
            corr_phi[b.ien-1][j][k] = cf * corr_phi[b.ien - 1][j][k] + cfm1 * corr_new;
            store[b.ien-1][j][k] = sfm1*phi[b.ien-1][j][k] + sp.sfin*(avg+corr_phi[b.ien-1][j][k]);

        }
    }

    // j=const
    for (int i = b.ist; i < b.ien; i++) {
        for (int k = b.kst; k < b.ken; k++) {
            int ip1 = i==b.ien-1 ? i : i+1;
            int kp1 = k==b.ken-1 ? k : k+1;
            int im1 = i==b.ist   ? i : i-1;
            int km1 = k==b.kst   ? k : k-1;

            double avg = (0.2f*phi[ip1][b.jst][k] + 0.2f*phi[im1][b.jst][k] +
                          0.2f*phi[i][b.jst][kp1] + 0.2f*phi[i][b.jst][km1] +
                          0.4f*phi[i][b.jst+1][k] - 0.2f*phi[i][b.jst+2][k]);
            corr_new = sp.fcorr * (phi[i][b.jst][k] - avg);
            corr_phi[i][b.jst][k] = cf * corr_phi[i][b.jst][k] + cfm1 * corr_new;

            store[i][b.jst][k] = sfm1*phi[i][b.jst][k] + sp.sfin*(avg+corr_phi[i][b.jst][k]);

            avg = (0.2f*phi[ip1][b.jen-1][k] + 0.2f*phi[im1][b.jen-1][k] +
                   0.2f*phi[i][b.jen-1][kp1] + 0.2f*phi[i][b.jen-1][km1] +
                   0.4f*phi[i][b.jen-2][k] - 0.2f*phi[i][b.jen-3][k]);

            corr_new = sp.fcorr * (phi[i][b.jen-1][k] - avg);
            corr_phi[i][b.jen-1][k] = cf * corr_phi[i][b.jen - 1][k] + cfm1 * corr_new;

            store[i][b.jen-1][k] = sfm1*phi[i][b.jen-1][k] + sp.sfin*(avg+corr_phi[i][b.jen-1][k]);

        }
    }

    // k=const
    for (int i = b.ist; i < b.ien; i++) {
        for (int j = b.jst; j < b.jen; j++) {
            int ip1 = i==b.ien-1 ? i : i+1;
            int jp1 = j==b.jen-1 ? j : j+1;
            int im1 = i==b.ist   ? i : i-1;
            int jm1 = j==b.jst   ? j : j-1;

            double avg = (0.2f*phi[ip1][j][b.kst] + 0.2f*phi[im1][j][b.kst] +
                          0.2f*phi[i][jp1][b.kst] + 0.2f*phi[i][jm1][b.kst] +
                          0.4f*phi[i][j][b.kst+1] - 0.2f*phi[i][j][b.kst+2]);

            corr_new = sp.fcorr * (phi[i][j][b.kst] - avg);
            corr_phi[i][j][b.kst] = cf * corr_phi[i][j][b.kst] + cfm1 * corr_new;
            store[i][j][b.kst] = sfm1*phi[i][j][b.kst] + sp.sfin*(avg+corr_phi[i][j][b.kst]);

            avg = (0.2f*phi[ip1][j][b.ken-1] + 0.2f*phi[im1][j][b.ken-1] +
                   0.2f*phi[i][jp1][b.ken-1] + 0.2f*phi[i][jm1][b.ken-1] +
                   0.4f*phi[i][j][b.ken-2] - 0.2f*phi[i][j][b.ken-3]);

            corr_new = sp.fcorr * (phi[i][j][b.ken-1] - avg);
            corr_phi[i][j][b.ken-1] = cf*corr_phi[i][j][b.ken-1] + cfm1*corr_new;
            store[i][j][b.ken-1] = sfm1*phi[i][j][b.ken-1] + sp.sfin*(avg+corr_phi[i][j][b.ken-1]);

        }
    }

    for (int i = b.ist; i < b.ien; i++) {
        for (int j = b.jst; j < b.jen; j++) {
            for (int k = b.kst; k < b.ken; k++) {
                phi[i][j][k] = store[i][j][k];
            }
        }
    }

}

void Solver::print_flux(vectornd<double, 4> var, int faceId, int k) {
    for (int j = var.shape()[2]-1; j >= 0; j--) {
        for (int i = 0; i < var.shape()[1]; i++) {
            std::cout << var[faceId][i][j][k] << std::setw(15);
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Solver::print_var(vector3d<double> var, int k) {
    for (int j = var.shape()[1]-1; j >= 0; j--) {
        for (int i = 0; i < var.shape()[0]; i++) {
            std::cout << var[i][j][k] << std::setw(15);
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void Solver::print_Ax(vector3d<Cell> var, int k) {
    for (int j = var.shape()[2]-1; j >= 0; j--) {
        for (int i = 0; i < var.shape()[1]; i++) {
            std::cout << var[i][j][k].Ax[0] << std::setw(15);
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

