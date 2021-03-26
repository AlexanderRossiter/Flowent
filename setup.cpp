//
// Created by Alexander Rossiter on 23/03/2021.
//

#include <fstream>
#include <regex>
#include <iostream>
#include "setup.h"
#include "util.h"
#include "cell_face_relation.h"
//Grid setup::read_grid() {
//
//}

Grid setup::read_grid_testcase(std::string testcase) {
    //std::ifstream file("/Users/ADR/CLionProjects/Flowent/test_cases/" + testcase);
    std::ifstream file("../test_cases/" + testcase);
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
        std::cout << "Calculating surface vectors..." << std::endl;
        setup::calculate_block_face_vectors(b);
        setup::calculate_block_volumes(b);
    }
}

void setup::calculate_block_face_vectors(Block& b) {
    std::vector<float> si;
    std::vector<float> sj;
    std::vector<float> sk;
    std::vector<std::vector<float>> sijk;

    std::vector<int> v1;
    std::vector<int> v2;
    std::vector<int> v3;
    std::vector<int> v4;

    std::vector<std::vector<int>> vertex_ijk;
    for (int i = 0; i < b.ni-1; i++) {
        for (int j = 0; j < b.nj-1; j++) {
            for (int k = 0; k < b.nk-1; k++) {
                for (int faceId = 0; faceId < 3; faceId++) {
                    // Loop through the 3 faces that are stored in this cell.
                    // face_verts[faceId][vertex][index]
                    // faceId - 0:iface, 1:jface, 2:kface
                    vertex_ijk = setup::get_vertex_ijk_vectors(faceId, i, j, k);
                    sijk[0] = setup::calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                                           vertex_ijk[2], vertex_ijk[3], b);
                }
                b.geom[i][j][k] = Cell(sijk[0], sijk[1], sijk[2]);
            }
        }
    }

    // i=const end face
    int i = b.ni-1;
    int faceId = 0;
    for (int j = 0; j < b.nj-1; j++) {
        for (int k = 0; k < b.nk-1; k++) {
            vertex_ijk = setup::get_vertex_ijk_vectors(faceId, i, j, k);
            si = setup::calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                              vertex_ijk[2], vertex_ijk[3], b);
            sj = {0, 0, 0};
            sk = {0, 0, 0};
            b.geom[i][j][k] = Cell(si, sj, sk);
        }
    }

    // j=const end face
    int j = b.nj-1;
    faceId = 1;
    for (int i= 0; i < b.ni-1; i++) {
        for (int k = 0; k < b.nk-1; k++) {
            vertex_ijk = setup::get_vertex_ijk_vectors(faceId, i, j, k);
            sj = setup::calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                              vertex_ijk[2], vertex_ijk[3], b);
            si = {0, 0, 0};
            sk = {0, 0, 0};
            b.geom[i][j][k] = Cell(si, sj, sk);
        }
    }
    // k=const end face
    int k = b.nk-1;
    faceId = 2;
    for (int i = 0; i < b.ni-1; i++) {
        for (int j = 0; j < b.nj-1; j++) {
            vertex_ijk = setup::get_vertex_ijk_vectors(faceId, i, j, k);
            sk = setup::calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                              vertex_ijk[2], vertex_ijk[3], b);
            si = {0, 0, 0};
            sj = {0, 0, 0};
            b.geom[i][j][k] = Cell(si, sj, sk);
        }
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

std::vector<std::vector<int>> setup::get_vertex_ijk_vectors(int faceId, int i, int j, int k) {
    std::vector<int> v1;
    std::vector<int> v2;
    std::vector<int> v3;
    std::vector<int> v4;

    v1 = {face_verts[faceId][0][0] + i, face_verts[faceId][0][1] + j, face_verts[faceId][0][2] + k};
    v2 = {face_verts[faceId][2][0] + i, face_verts[faceId][2][1] + j, face_verts[faceId][2][2] + k};
    v3 = {face_verts[faceId][3][0] + i, face_verts[faceId][3][1] + j, face_verts[faceId][3][2] + k};
    v4 = {face_verts[faceId][1][0] + i, face_verts[faceId][1][1] + j, face_verts[faceId][1][2] + k};

    return std::vector<std::vector<int>> {v1,v2,v3,v4};
}

void setup::calculate_block_volumes(Block& b) {
    float v;
    util::point r{};
    util::point ro{};

    for (int i = 0; i < b.ni-1; i++) {
        for (int j = 0; j < b.nj-1; j++) {
            for (int k = 0; k < b.nk-1; k++) {
                v = 0.f;
                ro.x = b.x[i][j][k];
                ro.y = b.y[i][j][k];
                ro.z = b.z[i][j][k];

                // Three faces that are stored in this cell.
                // i-face
                r.x = 0.25f * (b.x[i][j][k] + b.x[i][j+1][k] + b.x[i][j][k+1] + b.x[i][j+1][k+1]);
                r.y = 0.25f * (b.y[i][j][k] + b.y[i][j+1][k] + b.y[i][j][k+1] + b.y[i][j+1][k+1]);
                r.z = 0.25f * (b.z[i][j][k] + b.z[i][j+1][k] + b.z[i][j][k+1] + b.z[i][j+1][k+1]);
                v += (r.x-ro.x)*b.geom[i][j][k].Si[0] + (r.y-ro.y)*b.geom[i][j][k].Si[1] + (r.z-ro.z)*b.geom[i][j][k].Si[2];
                // j-face
                r.x = 0.25f * (b.x[i][j][k] + b.x[i+1][j][k] + b.x[i][j][k+1] + b.x[i+1][j][k+1]);
                r.y = 0.25f * (b.y[i][j][k] + b.x[i+1][j][k] + b.y[i][j][k+1] + b.y[i+1][j][k+1]);
                r.z = 0.25f * (b.z[i][j][k] + b.z[i+1][j][k] + b.z[i][j][k+1] + b.z[i+1][j][k+1]);
                v += (r.x-ro.x)*b.geom[i][j][k].Sj[0] + (r.y-ro.y)*b.geom[i][j][k].Sj[1] + (r.z-ro.z)*b.geom[i][j][k].Sj[2];
                // k-face
                r.x = 0.25f * (b.x[i][j][k] + b.x[i+1][j][k] + b.x[i][j+1][k] + b.x[i+1][j+1][k]);
                r.y = 0.25f * (b.y[i][j][k] + b.y[i+1][j][k] + b.y[i][j+1][k] + b.y[i+1][j+1][k]);
                r.z = 0.25f * (b.z[i][j][k] + b.z[i+1][j][k] + b.z[i][j+1][k] + b.z[i+1][j+1][k]);
                v += (r.x-ro.x)*b.geom[i][j][k].Sk[0] + (r.y-ro.y)*b.geom[i][j][k].Sk[1] + (r.z-ro.z)*b.geom[i][j][k].Sk[2];

                // Faces that are stored in adjacent cells.
                // i-face stored in next cell
                r.x = 0.25f * (b.x[i+1][j][k] + b.x[i+1][j+1][k] + b.x[i+1][j][k+1] + b.x[i+1][j+1][k+1]);
                r.y = 0.25f * (b.y[i+1][j][k] + b.y[i+1][j+1][k] + b.y[i+1][j][k+1] + b.y[i+1][j+1][k+1]);
                r.z = 0.25f * (b.z[i+1][j][k] + b.z[i+1][j+1][k] + b.z[i+1][j][k+1] + b.z[i+1][j+1][k+1]);
                v += -(r.x-ro.x)*b.geom[i+1][j][k].Si[0] - (r.y-ro.y)*b.geom[i+1][j][k].Si[1] - (r.y-ro.y)*b.geom[i+1][j][k].Si[2];
                // j-face stored in next cell
                r.x = 0.25f * (b.x[i][j+1][k] + b.x[i+1][j+1][k] + b.x[i][j+1][k+1] + b.x[i+1][j+1][k+1]);
                r.y = 0.25f * (b.y[i][j+1][k] + b.x[i+1][j+1][k] + b.y[i][j+1][k+1] + b.y[i+1][j+1][k+1]);
                r.z = 0.25f * (b.z[i][j+1][k] + b.z[i+1][j+1][k] + b.z[i][j+1][k+1] + b.z[i+1][j+1][k+1]);
                v += -(r.x-ro.x)*b.geom[i][j+1][k].Sj[0] - (r.y-ro.y)*b.geom[i][j+1][k].Sj[1] - (r.z-ro.z)*b.geom[i][j+1][k].Sj[2];
                // k-face stored in next cell
                r.x = 0.25f * (b.x[i][j][k+1] + b.x[i+1][j][k+1] + b.x[i][j+1][k+1] + b.x[i+1][j+1][k+1]);
                r.y = 0.25f * (b.y[i][j][k+1] + b.y[i+1][j][k+1] + b.y[i][j+1][k+1] + b.y[i+1][j+1][k+1]);
                r.z = 0.25f * (b.z[i][j][k+1] + b.z[i+1][j][k+1] + b.z[i][j+1][k+1] + b.z[i+1][j+1][k+1]);
                v += -(r.x-ro.x)*b.geom[i][j][k+1].Sk[0] - (r.y-ro.y)*b.geom[i][j][k+1].Sk[1] - (r.z-ro.z)*b.geom[i][j][k+1].Sk[2];


                b.volume[i][j][k] = 0.333333333f * v;
                std::cout << b.volume[i][j][k] << std::endl;
            }
        }
    }
}




