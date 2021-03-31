//
// Created by Alexander Rossiter on 23/03/2021.
//

#include <iostream>
#include "Block.h"

Block::Block(vector3d<float> x_, vector3d<float> y_,
             vector3d<float> z_) {

    id = -1;

    // Number of nodes excluding halos.
    ni = x_.size();
    nj = x_[0].size();
    nk = x_[0][0].size();

    // Array initialisation. A +2 cell 'halo' is put around the 3d block, this allows easy patching.
    // Geometry
    x.resize(boost::extents[ni+2][nj+2][nk+2]);
    y.resize(boost::extents[ni+2][nj+2][nk+2]);
    z.resize(boost::extents[ni+2][nj+2][nk+2]);
    isWall.resize(boost::extents[ni + 2][nj + 2][nk + 2]);
    geom.resize(boost::extents[ni+2][nj+2][nk+2]);
    volume.resize(boost::extents[ni+2][nj+2][nk+2]);
    // Primary flow variables
    ro.resize(boost::extents[ni+2][nj+2][nk+2]);
    rovx.resize(boost::extents[ni+2][nj+2][nk+2]);
    rovy.resize(boost::extents[ni+2][nj+2][nk+2]);
    rovz.resize(boost::extents[ni+2][nj+2][nk+2]);
    roe.resize(boost::extents[ni+2][nj+2][nk+2]);
    // Secondary flow variables
    pstat.resize(boost::extents[ni+2][nj+2][nk+2]);
    hstag.resize(boost::extents[ni+2][nj+2][nk+2]);
    vx.resize(boost::extents[ni+2][nj+2][nk+2]);
    vy.resize(boost::extents[ni+2][nj+2][nk+2]);
    vz.resize(boost::extents[ni+2][nj+2][nk+2]);

    // Set the array bounds assuming that we have a halo all around.
    ist = 1;
    ien = ni+1;
    jst = 1;
    jen = nj+1;
    kst = 1;
    ken = nk+1;

    // Add the geometry to arrays leaving halos all around.
    fill_geometry_arrays(x_, y_, z_);
    initialise_walls();

}

void Block::fill_geometry_arrays(vector3d<float>& x_, vector3d<float>& y_,
                                 vector3d<float>& z_) {
    for (int i = ist; i < ien; i++) {
        for (int j = jst; j < jen; j++) {
            for (int k = kst; k < ken; k++) {
                x[i][j][k] = x_[i-ist][j-jst][k-kst];
                y[i][j][k] = y_[i-ist][j-jst][k-kst];
                z[i][j][k] = z_[i-ist][j-jst][k-kst];
            }
        }
    }
}

void Block::set_block_id(int id_) {
    id = id_;
}

void Block::initialise_walls() {
    // We start by assuming all block boundaries are walls, when we read the patches we can then modify where necessary.
    // k faces.
    for (int k : std::vector<int>{kst, ken-1}) {
        for (int i = ist; i < ien; i++) {
            for (int j = jst; j < jen; j++) {
                isWall[i][j][k] = true;
            }
        }
    }

    // j faces.
    for (int j : std::vector<int>{jst, jen-1}) {
        for (int i = ist; i < ien; i++) {
            for (int k = kst; k < ken; k++) {
                isWall[i][j][k] = true;
            }
        }
    }

    // i faces.
    for (int i : std::vector<int>{ist, ien-1}) {
        for (int j = jst; j < jen; j++) {
            for (int k = kst; k < ken; k++) {
                isWall[i][j][k] = true;
            }
        }
    }

}
