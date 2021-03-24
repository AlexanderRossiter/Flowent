//
// Created by Alexander Rossiter on 23/03/2021.
//

#include <fstream>
#include <regex>
#include <iostream>
#include "setup.h"
//Grid setup::read_grid() {
//
//}

Grid setup::read_grid_testcase(std::string testcase) {
    std::ifstream file("/Users/ADR/CLionProjects/Flowent/test_cases/" + testcase);
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
    std::vector<std::vector<std::vector<float>>> x(ni, std::vector<std::vector<float>>(nj, std::vector<float>(nk)));
    std::vector<std::vector<std::vector<float>>> y(ni, std::vector<std::vector<float>>(nj, std::vector<float>(nk)));
    std::vector<std::vector<std::vector<float>>> z(ni, std::vector<std::vector<float>>(nj, std::vector<float>(nk)));


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
    Block b = Block(x, y, z);
    Grid g;
    g.add_block(b);

    return g;
}

void setup::calculate_grid_geometries(Grid &g) {
    // Calculates the grid cell volumes and normal vectors for the
    // finite volume method.

    float delXa;
    float delXb;
    float delYa;
    float delYb;
    float delZa;
    float delZb;

    std::vector<float> s1;
    std::vector<float> s2;
    std::vector<float> s3;

    for (Block& b : g.blocks) {
        for (int i = 0; i < b.ni-1; i++) {
            for (int j = 0; j < b.nj-1; j++) {
                for (int k = 0; k < b.nk-1; k++) {

                    delXa = b.x[i][j+1][k+1] - b.x[i][j][k];
                    delXb = b.x[i][j+1][k]   - b.x[i][j][k+1];

                    delYa = b.y[i][j+1][k+1] - b.y[i][j][k];
                    delYb = b.y[i][j+1][k]   - b.y[i][j][k+1];

                    delZa = b.z[i][j+1][k+1] - b.z[i][j][k];
                    delZb = b.z[i][j+1][k]   - b.z[i][j][k+1];

                    s1 = {0.5f * (delZa*delYb - delYa*delZb),
                          0.5f * (delXa*delZb - delZa*delXb),
                          0.5f * (delYa*delXb - delXa*delYb)};

                    delXa = b.x[i+1][j+1][k] - b.x[i][j][k];
                    delXb = b.x[i][j+1][k]   - b.x[i+1][j][k];

                    delYa = b.y[i+1][j+1][k] - b.y[i][j][k];
                    delYb = b.y[i][j+1][k]   - b.y[i+1][j][k];

                    delZa = b.z[i+1][j+1][k] - b.z[i][j][k];
                    delZb = b.z[i][j+1][k]   - b.z[i+1][j][k];

                    s2 = {0.5f * (delZa*delYb - delYa*delZb),
                          0.5f * (delXa*delZb - delZa*delXb),
                          0.5f * (delYa*delXb - delXa*delYb)};

                    delXa = b.x[i+1][j][k+1] - b.x[i][j][k];
                    delXb = b.x[i][j][k+1]   - b.x[i+1][j][k];

                    delYa = b.y[i+1][j][k+1] - b.y[i][j][k];
                    delYb = b.y[i][j][k+1]   - b.y[i+1][j][k];

                    delZa = b.z[i+1][j][k+1] - b.z[i][j][k];
                    delZb = b.z[i][j][k+1]   - b.z[i+1][j][k];

                    s3 = {0.5f * (delZa*delYb - delYa*delZb),
                          0.5f * (delXa*delZb - delZa*delXb),
                          0.5f * (delYa*delXb - delXa*delYb)};

                    b.geom[i][j][k] = Cell(s1, s2, s3);
                    std::cout << b.geom[i][j][k].Ai << ", " << b.geom[i][j][k].Aj << ", " << b.geom[i][j][k].Ak << std::endl;

                }
            }
        }
    }
}



