//
// Created by Alexander Rossiter on 23/03/2021.
//

#include "Block.h"

Block::Block(vector3d<float> x_, vector3d<float> y_,
             vector3d<float> z_) {

    ni = x_.size();
    nj = x_[0].size();
    nk = x_[0][0].size();
    x.resize(boost::extents[ni][nj][nk]);
    y.resize(boost::extents[ni][nj][nk]);
    z.resize(boost::extents[ni][nj][nk]);
    x = x_;
    y = y_;
    z = z_;
    id = -1;

    geom.resize(boost::extents[ni][nj][nk]);
    volume.resize(boost::extents[ni][nj][nk]);
}

void Block::set_block_id(int id_) {
    id = id_;
}
