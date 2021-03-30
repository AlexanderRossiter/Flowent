//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_BLOCK_H
#define FLOWENT_BLOCK_H


#include <vector>
#include "Cell.h"
#include "../typedefinitions.h"

class Block {

private:
    void fill_geometry_arrays(vector3d<float>& x_, vector3d<float>& y_,
            vector3d<float>& z_);
    void initialise_walls();

public:
    // Grid properties.
    vector3d<float> x;
    vector3d<float> y;
    vector3d<float> z;
    vector3d<float> volume;
    vector3d<Cell>  geom;
    vector3d<bool>  isWall;

    // Flow variables.
    vector3d<float> ro;
    vector3d<float> rovx;
    vector3d<float> rovy;
    vector3d<float> rovz;
    vector3d<float> roe;

    int id;

    int ni;
    int nj;
    int nk;

    int ist;
    int ien;
    int jst;
    int jen;
    int kst;
    int ken;

    Block(vector3d<float> x_, vector3d<float> y_, vector3d<float> z_);
    Block(){};
    ~Block(){};
    void set_block_id(int id_);

};


#endif //FLOWENT_BLOCK_H
