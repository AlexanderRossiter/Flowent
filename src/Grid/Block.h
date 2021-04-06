//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_BLOCK_H
#define FLOWENT_BLOCK_H


#include <vector>
#include <map>
#include "Cell.h"
#include "../util/typedefinitions.h"

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

    float minV;

    // Primary flow variables (conserved eqns).
    std::map<std::string, vector3d<float>> primary_vars = {{"ro", vector3d<float>()},
                                                           {"rovx", vector3d<float>()},
                                                           {"rovy", vector3d<float>()},
                                                           {"rovz", vector3d<float>()},
                                                           {"roe", vector3d<float>()}};

    // Secondary flow variables.
    std::map<std::string, vector3d<float>> secondary_vars = {{"pstat", vector3d<float>()},
                                                             {"hstag", vector3d<float>()},
                                                             {"vx", vector3d<float>()},
                                                             {"vy", vector3d<float>()},
                                                             {"vz", vector3d<float>()}};

    // Convective fluxes.
    std::map<std::string, vectornd<float, 4>> c_fluxes = {{"mass", vectornd<float, 4>()},
                                                         {"vx", vectornd<float, 4>()},
                                                         {"vy", vectornd<float, 4>()},
                                                         {"vz", vectornd<float, 4>()},
                                                         {"hstag", vectornd<float, 4>()}};

    // Residuals
    std::map<std::string, vector3d<float>> residuals = {{"ro", vector3d<float>()},
                                                           {"rovx", vector3d<float>()},
                                                           {"rovy", vector3d<float>()},
                                                           {"rovz", vector3d<float>()},
                                                           {"roe", vector3d<float>()}};

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
    Block()=default;
    ~Block()=default;
    void set_block_id(int id_);

};


#endif //FLOWENT_BLOCK_H
