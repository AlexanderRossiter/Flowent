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
    //std::ifstream file("/Users/ADR/CLionProjects/Flowent/test_cases/" + testcase);
    std::ifstream file("..\\test_cases\\" + testcase);
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

    std::vector<float> si;
    std::vector<float> sj;
    std::vector<float> sk;

    std::vector<int> v1;
    std::vector<int> v2;
    std::vector<int> v3;
    std::vector<int> v4;

    for (Block& b : g.blocks) {
        // All cells except external faces.
        for (int i = 0; i < b.ni-1; i++) {
            for (int j = 0; j < b.nj-1; j++) {
                for (int k = 0; k < b.nk-1; k++) {

                    v1 = {i, j, k};
                    v2 = {i, j+1, k+1};
                    v3 = {i, j, k+1};
                    v4 = {i, j+1, k};
                    si = setup::calculate_face_vector(v1, v2, v3, v4, b);

                    v1 = {i, j, k};
                    v2 = {i+1, j+1, k};
                    v3 = {i+1, j, k};
                    v4 = {i, j+1, k};
                    sk = setup::calculate_face_vector(v1, v2, v3, v4, b);

                    v1 = {i, j, k};
                    v2 = {i+1, j, k+1};
                    v3 = {i+1, j, k};
                    v4 = {i, j, k+1};
                    sj = setup::calculate_face_vector(v1, v2, v3, v4, b);

                    b.geom[i][j][k] = Cell(si, sj, sk);
                }
            }
        }

        // i=const end face
        int i = b.ni-1;
        for (int j = 0; j < b.nj-1; j++) {
            for (int k = 0; k < b.nk-1; k++) {
                std::cout << i << " " << j << " " << k << std::endl;

                v1 = {i, j, k};
                v2 = {i, j+1, k+1};
                v3 = {i, j, k+1};
                v4 = {i, j+1, k};
                si = setup::calculate_face_vector(v1, v2, v3, v4, b);
                sj = {0, 0, 0};
                sk = {0, 0, 0};
                b.geom[i][j][k] = Cell(si, sj, sk);
            }
        }

        // j=const end face
        int j = b.nj-1;
        for (int i= 0; i < b.ni-1; i++) {
            for (int k = 0; k < b.nk-1; k++) {
                v1 = {i, j, k};
                v2 = {i+1, j, k+1};
                v3 = {i+1, j, k};
                v4 = {i, j, k+1};
                sj = setup::calculate_face_vector(v1, v2, v3, v4, b);
                si = {0, 0, 0};
                sk = {0, 0, 0};
                b.geom[i][j][k] = Cell(si, sj, sk);
            }
        }
        // k=const end face
        int k = b.nk-1;
        for (int i = 0; i < b.ni-1; i++) {
            for (int j = 0; j < b.nj-1; j++) {
                v1 = {i, j, k};
                v2 = {i+1, j+1, k};
                v3 = {i+1, j, k};
                v4 = {i, j+1, k};
                sk = setup::calculate_face_vector(v1, v2, v3, v4, b);
                si = {0, 0, 0};
                sj = {0, 0, 0};
                b.geom[i][j][k] = Cell(si, sj, sk);
            }
        }
//        k = 0;
//        for (int i = 0; i < b.ni; i++) {
//            for (int j = 0; j < b.nj; j++) {
//                std::cout << b.geom[i][j][k].Ai << ", " << b.geom[i][j][k].Aj << ", " << b.geom[i][j][k].Ak << std::endl;
//            }
//        }
    }
}

std::vector<float> setup::calculate_face_vector(std::vector<int>& v1, std::vector<int>& v2,
                                         std::vector<int>& v3, std::vector<int>& v4,
                                         Block& b) {
    float delXa = b.x[v2[0]][v2[1]][v2[2]] - b.x[v1[0]][v1[1]][v1[2]];
    float delXb = b.x[v4[0]][v4[1]][v4[2]] - b.x[v3[0]][v3[1]][v3[2]];

    float delYa = b.y[v2[0]][v2[1]][v2[2]] - b.y[v1[0]][v1[1]][v1[2]];
    float delYb = b.y[v4[0]][v4[1]][v4[2]] - b.y[v3[0]][v3[1]][v3[2]];

    float delZa = b.z[v2[0]][v2[1]][v2[2]] - b.z[v1[0]][v1[1]][v1[2]];
    float delZb = b.z[v4[0]][v4[1]][v4[2]] - b.z[v3[0]][v3[1]][v3[2]];

    std::vector<float> s1 = {0.5f * (delZa*delYb - delYa*delZb),
                             0.5f * (delXa*delZb - delZa*delXb),
                             0.5f * (delYa*delXb - delXa*delYb)};

    return s1;
}



