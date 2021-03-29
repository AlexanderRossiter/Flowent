//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_BLOCK_H
#define FLOWENT_BLOCK_H


#include <vector>
#include "Cell.h"
#include "../typedefinitions.h"

class Block {

public:
    vector3d<float> x;
    vector3d<float> y;
    vector3d<float> z;
    vector3d<float> volume;
    vector3d<Cell> geom;
    //std::vector<Patch*> patches;
    int id;
    int ni;
    int nj;
    int nk;

    Block(vector3d<float> x_, vector3d<float> y_, vector3d<float> z_);
    void set_block_id(int id_);

};


#endif //FLOWENT_BLOCK_H
