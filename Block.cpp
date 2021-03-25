//
// Created by Alexander Rossiter on 23/03/2021.
//

#include "Block.h"

Block::Block(std::vector<std::vector<std::vector<float>>> x_, std::vector<std::vector<std::vector<float>>> y_,
             std::vector<std::vector<std::vector<float>>> z_) {
    x = x_;
    y = y_;
    z = z_;
    ni = x.size();
    nj = x[0].size();
    nk = x[0][0].size();

    geom = std::vector<std::vector<std::vector<Cell>>> (ni, std::vector<std::vector<Cell>>(nj, std::vector<Cell>(nk)));
    volume = std::vector<std::vector<std::vector<float>>>(ni, std::vector<std::vector<float>>(nj, std::vector<float>(nk)));
}
