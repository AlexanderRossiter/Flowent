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
    void fill_geometry_arrays(vector3d<double>& x_, vector3d<double>& y_,
            vector3d<double>& z_);
    void initialise_walls();

public:
    // Grid properties.
    vector3d<double> x;
    vector3d<double> y;
    vector3d<double> z;
    vector3d<double> volume;
    vector3d<Cell>  geom;
    vector3d<double>  isWall;

    double minV;

    // Primary flow variables (conserved eqns).
    std::map<std::string, vector3d<double>> primary_vars = {{"ro", vector3d<double>()},
                                                           {"rovx", vector3d<double>()},
                                                           {"rovy", vector3d<double>()},
                                                           {"rovz", vector3d<double>()},
                                                           {"roe", vector3d<double>()}};

    // Secondary flow variables.
    std::map<std::string, vector3d<double>> secondary_vars = {{"pstat", vector3d<double>()},
                                                             {"hstag", vector3d<double>()},
                                                             {"vx", vector3d<double>()},
                                                             {"vy", vector3d<double>()},
                                                             {"vz", vector3d<double>()}};

    // Convective fluxes.
    std::map<std::string, vectornd<double, 4>> c_fluxes = {{"mass", vectornd<double, 4>()},
                                                         {"vx", vectornd<double, 4>()},
                                                         {"vy", vectornd<double, 4>()},
                                                         {"vz", vectornd<double, 4>()},
                                                         {"hstag", vectornd<double, 4>()}};

    // Residuals
    std::map<std::string, vector3d<double>> residuals = {{"ro", vector3d<double>()},
                                                           {"rovx", vector3d<double>()},
                                                           {"rovy", vector3d<double>()},
                                                           {"rovz", vector3d<double>()},
                                                           {"roe", vector3d<double>()}};

    // Deferred correction variables
    std::map<std::string, vector3d<double>> corr_primary_vars = {{"ro", vector3d<double>()},
                                                            {"rovx", vector3d<double>()},
                                                            {"rovy", vector3d<double>()},
                                                            {"rovz", vector3d<double>()},
                                                            {"roe", vector3d<double>()}};

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

    Block(vector3d<double> x_, vector3d<double> y_, vector3d<double> z_);
    Block()=default;
    ~Block()=default;
    void set_block_id(int id_);

};


#endif //FLOWENT_BLOCK_H
