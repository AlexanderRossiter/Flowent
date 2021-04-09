//
// Created by Alexander Rossiter on 30/03/2021.
//

#include "Solver.h"
#include <cmath>

void Solver::run_nsteps(int nsteps) {
    Solver::set_timestep(300);

    while (nstep < nsteps) {
        run_iteration();
        if (nstep % 5 == 0) {
            std::cout << "TIME STEP NUMBER: " << nstep << std::endl;
            std::cout << "DelRo " << g.get_block_by_id(0).residuals["ro"][10][1][1] << std::endl;
        }
        nstep++;
    }
}

void Solver::run_iteration() {


    // Set the fluxes through the ijk faces in the domain.
    Extent e{};
    for (Block& b : g.get_blocks()) {
        Solver::set_secondary_variables(b);
        Solver::apply_boundary_conditions();

        // I-faces
        //std::cout << "I-faces" << std::endl;
        e = {b.ist, b.ien, b.jst, b.jen-1, b.kst, b.ken-1};
        Solver::set_convective_fluxes(b, 0, e);

        // J-faces
        //std::cout << "J-faces" << std::endl;
        e = {b.ist, b.ien-1, b.jst, b.jen, b.kst, b.ken-1};
        Solver::set_convective_fluxes(b, 1, e);

        // K-faces
        //std::cout << "K-faces" << std::endl;
        e = {b.ist, b.ien-1, b.jst, b.jen-1, b.kst, b.ken};
        Solver::set_convective_fluxes(b, 2, e);

        Solver::set_wall_bconds(b);

        std::cout << b.c_fluxes["mass"][0][2][14][1] << std::endl;
        //std::cout << b.secondary_vars["pstat"][49][1][2]<< std::endl;

//        std::cout << b.primary_vars["ro"][1][20][2] << std::endl;

        Solver::sum_convective_fluxes(b, b.primary_vars["ro"],   b.c_fluxes["mass"],  b.residuals["ro"]);
        Solver::sum_convective_fluxes(b, b.primary_vars["rovx"], b.c_fluxes["vx"],    b.residuals["rovx"]);
        Solver::sum_convective_fluxes(b, b.primary_vars["rovy"], b.c_fluxes["vy"],    b.residuals["rovy"]);
        Solver::sum_convective_fluxes(b, b.primary_vars["rovz"], b.c_fluxes["vz"],    b.residuals["rovz"]);
        Solver::sum_convective_fluxes(b, b.primary_vars["roe"],  b.c_fluxes["hstag"], b.residuals["roe"]);

        smooth(b, b.primary_vars["ro"]);
        smooth(b, b.primary_vars["rovx"]);
        smooth(b, b.primary_vars["rovy"]);
        smooth(b, b.primary_vars["rovz"]);
        smooth(b, b.primary_vars["roe"]);
        test_for_nans(b);



    }
}

void Solver::apply_boundary_conditions() {
    //std::cout << "Applying patch boundary conditions.\n";
    //Apply the patch boundary conditions.
    for (auto& p : g.get_patches()) {
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

void Solver::set_secondary_variables(Block& b) {
    //std::cout << "Setting secondary variables\n";
    vector3d<float>& ro = b.primary_vars["ro"];
    vector3d<float>& rovx = b.primary_vars["rovx"];
    vector3d<float>& rovy = b.primary_vars["rovy"];
    vector3d<float>& rovz = b.primary_vars["rovz"];
    vector3d<float>& roe = b.primary_vars["roe"];
    vector3d<float>& pstat = b.secondary_vars["pstat"];
    vector3d<float>& vx = b.secondary_vars["vx"];
    vector3d<float>& vy = b.secondary_vars["vy"];
    vector3d<float>& vz = b.secondary_vars["vz"];
    vector3d<float>& hstag = b.secondary_vars["hstag"];

        for (int i = b.ist; i < b.ien; i++) {
            for (int j = b.jst; j < b.jen; j++) {
                for (int k = b.kst; k < b.ken; k++) {
                    vx[i][j][k] = rovx[i][j][k] / ro[i][j][k];
                    vy[i][j][k] = rovy[i][j][k] / ro[i][j][k];
                    vz[i][j][k] = rovz[i][j][k] / ro[i][j][k];

                    float velSq = vx[i][j][k]*vx[i][j][k] +
                            vy[i][j][k]*vy[i][j][k] +
                            vz[i][j][k]*vz[i][j][k];
                    float EKE = 0.5f*velSq;
                    float tstat = (roe[i][j][k] / ro[i][j][k] - EKE) / gas.cv;
                    pstat[i][j][k] = ro[i][j][k] * gas.R * tstat;//(gas.ga-1) * (roe[i][j][k] - EKE*ro[i][j][k]);
                    hstag[i][j][k] = (roe[i][j][k] + pstat[i][j][k]) / ro[i][j][k];
                }
            }
        }

}

void Solver::set_convective_fluxes(Block& b, int faceId, Extent& extent) {
    // Assign these by reference here so we don't have to do many map lookups.
    vector3d<float>& rovx = b.primary_vars["rovx"];
    vector3d<float>& rovy = b.primary_vars["rovy"];
    vector3d<float>& rovz = b.primary_vars["rovz"];
    vector3d<float>& pstat = b.secondary_vars["pstat"];
    vector3d<float> s_var(boost::extents[b.ni+2][b.nj+2][b.nk+2]);


    for (auto& entry : b.c_fluxes) {
        bool notMass = false;
        if (entry.first != "mass") {
            s_var = b.secondary_vars[entry.first];
            notMass = true;
        }

        for (int i = extent.ist; i < extent.ien; i++) {
            for (int j = extent.jst; j < extent.jen; j++) {
                for (int k = extent.kst; k < extent.ken; k++) {

                    // Make sure the flux begins at 0.
                    entry.second[faceId][i][j][k]=0;

                    // Calculate the average value on this face
                    float face_avg_secondary = 1;
                    if (notMass) {
                        face_avg_secondary = util::calculate_face_average(s_var, faceId, i, j, k);
                    }
                    float face_avg_rovx = util::calculate_face_average(rovx, faceId, i, j, k);
                    float face_avg_rovy = util::calculate_face_average(rovy, faceId, i, j, k);
                    float face_avg_rovz = util::calculate_face_average(rovz, faceId, i, j, k);

                    // Sum the x,y,z  components of the flux through the i-face for this cell.
                    entry.second[faceId][i][j][k] += face_avg_secondary * face_avg_rovx * b.geom[i][j][k].Ax[faceId];
                    entry.second[faceId][i][j][k] += face_avg_secondary * face_avg_rovy * b.geom[i][j][k].Ay[faceId];
                    entry.second[faceId][i][j][k] += face_avg_secondary * face_avg_rovz * b.geom[i][j][k].Az[faceId];

                    //std::cout << entry.first << " " << entry.second[faceId][i][j][k] << " " << b.geom[i][j][k].A[0] << " " << i << " " << j << " " << k << std::endl;
                    // If this is momentum flux we also include the relevant PdA term.
                    if (entry.first == "vx") {
                        entry.second[faceId][i][j][k] += util::calculate_face_average(pstat, faceId, i, j, k) * b.geom[i][j][k].Ax[faceId];
                    } else if (entry.first == "vy") {
                        entry.second[faceId][i][j][k] += util::calculate_face_average(pstat, faceId, i, j, k) * b.geom[i][j][k].Ay[faceId];
                    } else if( entry.first == "vz") {
                        entry.second[faceId][i][j][k] += util::calculate_face_average(pstat, faceId, i, j, k) * b.geom[i][j][k].Az[faceId];
                    }

                }
            }
        }
    }
}

void Solver::set_timestep(float tstag) {
    float a_sound = sqrt(gas.ga * gas.R * tstag);
    float U = a_sound;
    delta_t = sp.cfl * cbrt(g.minV) / (U + a_sound);
}


void Solver::sum_convective_fluxes(Block &b, vector3d<float>& phi, vectornd<float, 4>& flux, vector3d<float>& residual) {
    //std::cout << "Summing fluxes\n";

    // First, calculate the change in the variable over the timestep dt.
    // Loop over the cells
    vector3d<float> delta_phi(boost::extents[b.ni+2][b.nj+2][b.nk+2]);
    for (int i = b.ist; i < b.ien-1; i++) {
        for (int j = b.jst; j < b.jen-1; j++) {
            for (int k = b.kst; k < b.ken-1; k++) {
                delta_phi[i][j][k] = delta_t / b.volume[i][j][k] * (flux[0][i][j][k]-flux[0][i+1][j][k] +
                        flux[1][i][j][k]-flux[1][i][j+1][k] + flux[2][i][j][k]-flux[2][i][j][k+1]);

                delta_phi[i][j][k] = 2.f*delta_phi[i][j][k] - residual[i][j][k]; // SCREE
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
            phi[i][b.jst][k] += 0.25f * (delta_phi[i][b.jst][k] + delta_phi[i-1][b.jst][k] + delta_phi[i][b.jst][k-1] + delta_phi[i-1][b.jst][k-1]);
            phi[i][b.jen-1][k] += 0.25f * (delta_phi[i][b.jen-2][k] + delta_phi[i-1][b.jen-2][k] + delta_phi[i][b.jen-2][k-1] + delta_phi[i-1][b.jen-2][k-1]);
        }
    }

    // k=const boundaries
    for (int i = b.ist+1; i < b.ien-1; i++) {
        for (int j = b.jst+1; j < b.jen-1; j++) {
            phi[i][j][b.kst] += 0.25f * (delta_phi[i][j][b.kst] + delta_phi[i-1][j][b.kst] + delta_phi[i][j-1][b.kst] + delta_phi[i-1][j-1][b.kst]);
            phi[i][j][b.ken-1] += 0.25f * (delta_phi[i][j][b.ken-2] + delta_phi[i-1][j][b.ken-2] + delta_phi[i][j][b.ken-2] + delta_phi[i-1][j-1][b.ken-2]);
        }
    }

    // i=const boundaries
    for (int j = b.jst+1; j < b.jen-1; j++) {
        for (int k = b.kst+1; k < b.ken-1; k++) {
            phi[b.ist][j][k] += 0.25f * (delta_phi[b.ist][j][k] + delta_phi[b.ist][j-1][k] + delta_phi[b.ist][j][k-1] + delta_phi[b.ist][j-1][k-1]);
            phi[b.ien-1][j][k] += 0.25f * (delta_phi[b.ien-2][j][k] + delta_phi[b.ien-2][j-1][k] + delta_phi[b.ien-2][j][k-1] + delta_phi[b.ien-2][j-1][k-1]);
        }
    }

    // j=const i lines
    for (int i = b.ist+1; i < b.ien-1; i++) {
        phi[i][b.jst][b.kst]   += 0.5f * (delta_phi[i][b.jst][b.kst] + delta_phi[i-1][b.jst][b.kst]);
        phi[i][b.jen-1][b.kst] += 0.5f * (delta_phi[i][b.jen-2][b.kst] + delta_phi[i-1][b.jen-2][b.kst]);
        phi[i][b.jst][b.ken-1]   += 0.5f * (delta_phi[i][b.jst][b.ken-2] + delta_phi[i-1][b.jst][b.ken-2]);
        phi[i][b.jen-1][b.ken-1] += 0.5f * (delta_phi[i][b.jen-2][b.ken-2] + delta_phi[i-1][b.jen-2][b.ken-2]);
    }

    // i=const k lines
    for (int k = b.kst+1; k < b.ken-1; k++) {
        phi[b.ist][b.jst][k]   += 0.5f * (delta_phi[b.ist][b.jst][k] + delta_phi[b.ist][b.jst][k-1]);
        phi[b.ist][b.jen-1][k] += 0.5f * (delta_phi[b.ist][b.jen-2][k] + delta_phi[b.ist][b.jen-2][k-1]);
        phi[b.ien-1][b.jst][k]   += 0.5f * (delta_phi[b.ien-2][b.jst][k] + delta_phi[b.ien-2][b.jst][k-1]);
        phi[b.ien-1][b.jen-1][k] += 0.5f * (delta_phi[b.ien-2][b.jen-2][k] + delta_phi[b.ien-2][b.jen-2][k-1]);
    }

    // k=const j lines
    for (int j = b.jst+1; j < b.jen-1; j++) {
        phi[b.ist][j][b.kst]   += 0.5f * (delta_phi[b.ist][j][b.kst] + delta_phi[b.ist][j-1][b.kst]);
        phi[b.ist][j][b.ken-1] += 0.5f * (delta_phi[b.ist][j][b.ken-2] + delta_phi[b.ist][j-1][b.ken-2]);
        phi[b.ien-1][j][b.kst]   += 0.5f * (delta_phi[b.ien-2][j][b.kst] + delta_phi[b.ien-2][j-1][b.kst]);
        phi[b.ien-1][j][b.ken-1] += 0.5f * (delta_phi[b.ien-2][j][b.ken-2] + delta_phi[b.ien-2][j-1][b.ken-2]);
    }



    // Eight Corners
    phi[b.ist][b.jst][b.kst] += delta_phi[b.ist][b.jst][b.kst];

    phi[b.ist][b.jen-1][b.kst] += delta_phi[b.ist][b.jen-2][b.kst];

    phi[b.ist][b.jst][b.ken-1] += delta_phi[b.ist][b.jst][b.ken-2];

    phi[b.ien-1][b.jst][b.kst] += delta_phi[b.ien-2][b.jst][b.kst];

    phi[b.ien-1][b.jen-1][b.kst] += delta_phi[b.ien-2][b.jen-2][b.kst];

    phi[b.ien-1][b.jst][b.ken-1] += delta_phi[b.ien-2][b.jst][b.ken-2];

    phi[b.ien-1][b.jen-1][b.ken-1] += delta_phi[b.ien-2][b.jen-2][b.ken-2];


    // This is needless, we could make delta_phi a pointer to residual at the beginning.
    for (int i = b.ist; i < b.ien-1; i++) {
        for (int j = b.jst; j < b.jen - 1; j++) {
            for (int k = b.kst; k < b.ken - 1; k++) {
                residual[i][j][k] = delta_phi[i][j][k];
            }
        }
    }

}

void Solver::set_wall_bconds(Block& b) {
    //// Loop over the edges of the block and see if the face is a wall.

    // i=const
    for (int j=b.jst; j < b.jen-1; j++) {
        for (int k=b.kst; k < b.ken-1; k++) {
            if (util::at_least_two(b.isWall[b.ist][j][k], b.isWall[b.ist][j+1][k],
                                    b.isWall[b.ist][j][k+1], b.isWall[b.ist][j+1][k+1])) {
                for (auto& entry : b.c_fluxes)
                    entry.second[0][b.ist][j][k] = 0;
            }

            if (util::at_least_two(b.isWall[b.ien-1][j][k], b.isWall[b.ien-1][j+1][k],
                                   b.isWall[b.ien-1][j][k+1], b.isWall[b.ien-1][j+1][k+1])) {
                for (auto& entry : b.c_fluxes)
                    entry.second[0][b.ien-1][j][k] = 0;
            }
        }
    }

    // j=const
    for (int i=b.ist; i < b.ien-1; i++) {
        for (int k=b.kst; k < b.ken-1; k++) {
            if (util::at_least_two(b.isWall[i][b.jst][k], b.isWall[i+1][b.jst][k],
                                   b.isWall[i][b.jst][k+1], b.isWall[i+1][b.jst][k+1])) {
                for (auto& entry : b.c_fluxes)
                    entry.second[1][i][b.jst][k] = 0;
            }

            if (util::at_least_two(b.isWall[i][b.jen-1][k], b.isWall[i+1][b.jen-1][k],
                                   b.isWall[i][b.jen-1][k+1], b.isWall[i+1][b.jen-1][k+1])) {
                for (auto& entry : b.c_fluxes) {
                    entry.second[1][i][b.jen - 1][k] = 0;
                }
            }
        }
    }

    // k=const
    for (int i=b.ist; i < b.ien-1; i++) {
        for (int j=b.jst; j < b.jen-1; j++) {
            if (util::at_least_two(b.isWall[i][j][b.kst], b.isWall[i][j+1][b.kst],
                                   b.isWall[i+1][j][b.kst], b.isWall[i+1][j+1][b.kst])) {
                for (auto& entry : b.c_fluxes)
                    entry.second[2][i][j][b.kst] = 0;
            }

            if (util::at_least_two(b.isWall[i][j][b.ken-1], b.isWall[i][j+1][b.ken-1],
                                   b.isWall[i+1][j][b.ken-1], b.isWall[i+1][j+1][b.ken-1])) {
                for (auto& entry : b.c_fluxes)
                    entry.second[2][i][j][b.ken-1] = 0;
            }
        }
    }
}

void Solver::smooth(Block& b, vector3d<float>& phi) {

    // We make all the chages to variable store, this way we don't smooth with
    // smoothed values.
    vector3d<float> store(boost::extents[b.ni+2][b.nj+2][b.nk+2]);
    float sfm1 = 1-sp.sfin;

    // Start with internal nodes
    for (int i = b.ist+1; i < b.ien-1; i++) {
        for (int j = b.jst+1; j < b.jen-1; j++) {
            for (int k = b.kst+1; k < b.ken-1; k++) {
                float avg = 0.1666667f * (phi[i+1][j][k]+phi[i-1][j][k]
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

            float avg = (0.2f*phi[b.ist][jp1][k] + 0.2f*phi[b.ist][jm1][k] +
                    0.2f*phi[b.ist][j][kp1] + 0.2f*phi[b.ist][j][km1] +
                    0.4f*phi[b.ist+1][j][k] - 0.2f*phi[b.ist+1][j][k]);
            store[b.ist][j][k] = sfm1*phi[b.ist][j][k] + sp.sfin*avg;

            avg = (0.2f*phi[b.ien-1][jp1][k] + 0.2f*phi[b.ien-1][jm1][k] +
                   0.2f*phi[b.ien-1][j][kp1] + 0.2f*phi[b.ien-1][j][km1] +
                   0.4f*phi[b.ien-2][j][k] - 0.2f*phi[b.ien-3][j][k]);
            store[b.ist][j][k] = sfm1*phi[b.ien-1][j][k] + sp.sfin*avg;

        }
    }

    // j=const
    for (int i = b.ist; i < b.ien; i++) {
        for (int k = b.kst; k < b.ken; k++) {
            int ip1 = i==b.jen-1 ? i : i+1;
            int kp1 = k==b.ken-1 ? k : k+1;
            int im1 = i==b.jst ? i : i-1;
            int km1 = k==b.kst ? k : k-1;

            float avg = (0.2f*phi[ip1][b.jst][k] + 0.2f*phi[im1][b.jst][k] +
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
            int ip1 = i==b.jen-1 ? i : i+1;
            int jp1 = j==b.ken-1 ? j : j+1;
            int im1 = i==b.jst ? i : i-1;
            int jm1 = j==b.kst ? j : j-1;

            float avg = (0.2f*phi[ip1][j][b.kst] + 0.2f*phi[im1][j][b.kst] +
                         0.2f*phi[i][jp1][b.kst] + 0.2f*phi[i][jm1][b.kst] +
                         0.4f*phi[i][j][b.kst+1] - 0.2f*phi[i][j][b.kst+2]);
            store[i][j][b.kst] = sfm1*phi[i][j][b.kst] + sp.sfin*avg;

            avg = (0.2f*phi[ip1][j][b.ken-1] + 0.2f*phi[im1][j][b.ken-1] +
                   0.2f*phi[i][jp1][b.ken-1] + 0.2f*phi[i][jm1][b.ken-1] +
                   0.4f*phi[i][j][b.ken-2] - 0.2f*phi[i][j][b.ken-3]);
            store[i][j][b.kst] = sfm1*phi[i][j][b.kst] + sp.sfin*avg;

        }
    }

    for (int i = b.ist+1; i < b.ien-1; i++) {
        for (int j = b.jst+1; j < b.jen-1; j++) {
            for (int k = b.kst+1; k < b.ken-1; k++) {
                phi[i][j][k] = store[i][j][k];
            }
        }
    }

}


