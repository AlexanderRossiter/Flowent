//
// Created by Alexander Rossiter on 23/03/2021.
//

#include <iostream>
#include "Block.h"

Block::Block(vector3d<float> x_, vector3d<float> y_,
             vector3d<float> z_) {

    id = -1;

    ni = x_.size();
    nj = x_[0].size();
    nk = x_[0][0].size();
    x.resize(boost::extents[ni+2][nj+2][nk+2]);
    y.resize(boost::extents[ni+2][nj+2][nk+2]);
    z.resize(boost::extents[ni+2][nj+2][nk+2]);


    // Set the array bounds assuming that we have a halo all around.
    ist = 1;
    ien = ni+1;
    jst = 1;
    jen = nj+1;
    kst = 1;
    ken = nk+1;

    // Add the geometry to arrays leaving halos all around.
    fill_geometry_arrays(x_, y_, z_);



    geom.resize(boost::extents[ni+2][nj+2][nk+2]);
    volume.resize(boost::extents[ni+2][nj+2][nk+2]);
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
