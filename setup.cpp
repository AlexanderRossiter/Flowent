//
// Created by Alexander Rossiter on 23/03/2021.
//

#include <fstream>
#include <regex>
#include <iostream>
#include "setup.h"
#include "util.h"


Grid setup::read_grid_testcase(std::string& testcase) {
    std::ifstream file("../test_cases/" + testcase + ".grid");
    if (file.fail()) {
        std::cout << "Failed to open file." << std::endl;
        throw std::exception();
    }

    std::string str;
    const std::regex r("([+-]?[0-9]*([.][0-9]+)?), ([+-]?[0-9]*([.][0-9]+)?), ([+-]?[0-9]*([.][0-9]+)?)");
    std::smatch m;

    int ni = -1;
    int nj = -1;
    int nk = -1;
    // Get first line which contains grid size.
    std::getline(file, str);
    if (std::regex_match(str, m, r)) {
        ni = std::stoi(m[1]);
        nj = std::stoi(m[3]);
        nk = std::stoi(m[5]);
    }
    //std::cout << ni << ", " << nj << ", " << nk << std::endl;

    // Initialise x, y, z
    vector3d<float> x(boost::extents[ni][nj][nk]);
    vector3d<float> y(boost::extents[ni][nj][nk]);
    vector3d<float> z(boost::extents[ni][nj][nk]);

    for (int i = 0; i < ni; i++) {
        for (int j = 0; j < nj; j++) {
            for (int k = 0; k < nk; k++) {
                std::getline(file, str);
                if (std::regex_match(str, m, r)) {
                    x[i][j][k] = std::stof(m[1]);
                    y[i][j][k] = std::stof(m[3]);
                    z[i][j][k] = std::stof(m[5]);
                    //std::cout << x[i][j][k] << ", " << y[i][j][k] << ", " << z[i][j][k] << std::endl;
                }
            }
        }
    }
    Grid g;
    g.add_block(Block(x, y, z));

    return g;
}






