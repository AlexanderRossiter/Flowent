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
    std::vector<std::vector<float>> sijk(3, std::vector<float>(3));

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
                    sijk[faceId] = setup::calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                                           vertex_ijk[2], vertex_ijk[3], b);
                }
                b.geom[i][j][k] = Cell(sijk[0], sijk[1], sijk[2]);
                //std::cout << b.geom[i][j][k].A[0] << ", " << b.geom[i][j][k].A[1] << ", " << b.geom[i][j][k].A[2] << std::endl;
                std::cout << "(" << b.geom[i][j][k].S[0][0] << ", " << b.geom[i][j][k].S[0][1] << ", " << b.geom[i][j][k].S[0][2] << "), ";
                std::cout << "(" << b.geom[i][j][k].S[1][0] << ", " << b.geom[i][j][k].S[1][1] << ", " << b.geom[i][j][k].S[1][2] << "), ";
                std::cout << "(" << b.geom[i][j][k].S[2][0] << ", " << b.geom[i][j][k].S[2][1] << ", " << b.geom[i][j][k].S[2][2] << ")" << std::endl;
            }
        }
    }

    // i=const end face
    int i = b.ni-1;
    int faceId = 0;
    for (int j = 0; j < b.nj-1; j++) {
        for (int k = 0; k < b.nk-1; k++) {
            vertex_ijk = setup::get_vertex_ijk_vectors(faceId, i, j, k);
            sijk[faceId] = setup::calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                              vertex_ijk[2], vertex_ijk[3], b);
            sijk[1] = {0, 0, 0};
            sijk[2] = {0, 0, 0};
            b.geom[i][j][k] = Cell(sijk[0], sijk[1], sijk[2]);
        }
    }

    // j=const end face
    int j = b.nj-1;
    faceId = 1;
    for (int i= 0; i < b.ni-1; i++) {
        for (int k = 0; k < b.nk-1; k++) {

            vertex_ijk = setup::get_vertex_ijk_vectors(faceId, i, j, k);
            sijk[faceId] = setup::calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                              vertex_ijk[2], vertex_ijk[3], b);
            sijk[0] = {0, 0, 0};
            sijk[2] = {0, 0, 0};
            b.geom[i][j][k] = Cell(sijk[0], sijk[1], sijk[2]);
        }
    }
    // k=const end face
    int k = b.nk-1;
    faceId = 2;
    for (int i = 0; i < b.ni-1; i++) {
        for (int j = 0; j < b.nj-1; j++) {
            vertex_ijk = setup::get_vertex_ijk_vectors(faceId, i, j, k);
            sijk[faceId] = setup::calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                              vertex_ijk[2], vertex_ijk[3], b);
            sijk[0] = {0, 0, 0};
            sijk[1] = {0, 0, 0};
            b.geom[i][j][k] = Cell(sijk[0], sijk[1], sijk[2]);
        }
    }
}

std::vector<float> setup::calculate_face_vector(std::vector<int>& v1, std::vector<int>& v2,
                                         std::vector<int>& v3, std::vector<int>& v4,
                                         Block& b) {
    // Calculates the cross product of the two diagonal vectors of the control volume.
    // Magnitude of the resulting vector is the Area of the face, the vector points
    // normal to the face.
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
    // Returns an array of indices of the four vertices of a face. Returned in order to get correct
    // cross product.
    // 2 o----o 4
    //   |----|
    //   |----|
    // 3 o----o 1
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
    std::vector<float> r;
    std::vector<float> ro(3);
    std::vector<float> r_star(3);

    for (int i = 0; i < b.ni-1; i++) {
        for (int j = 0; j < b.nj-1; j++) {
            for (int k = 0; k < b.nk-1; k++) {
                v = 0.f;
                ro[0] = b.x[i][j][k];
                ro[1] = b.y[i][j][k];
                ro[2] = b.z[i][j][k];

                // Loop over the three faces stored with this cell.
                for (int faceId = 0; faceId < 3; faceId++) {
                    r = setup::get_face_midpoint_vector(faceId, i, j, k, b);
                    r_star = util::vector_subtr(r, ro);
                    v += util::vector_dot(r_star, b.geom[i][j][k].S[faceId]) * b.geom[i][j][k].A[faceId];
                }
                // Faces stored in adjacent cells. These vectors point in the wrong direction
                // for this cell, so must subtract.
                // i-face: stored in i+1th cell.
                int faceId = 0;
                r = setup::get_face_midpoint_vector(faceId, i+1, j, k, b);
                r_star = util::vector_subtr(r, ro);
                v -= util::vector_dot(r_star, b.geom[i+1][j][k].S[faceId]) * b.geom[i+1][j][k].A[faceId];
                // j-face: stored in j+1th cell.
                faceId = 1;
                r = setup::get_face_midpoint_vector(faceId, i, j+1, k, b);
                r_star = util::vector_subtr(r, ro);
                v -= util::vector_dot(r_star, b.geom[i][j+1][k].S[faceId]) * b.geom[i][j+1][k].A[faceId];
                // k-face: stored in k+1th cell.
                faceId = 2;
                r = setup::get_face_midpoint_vector(faceId, i, j, k+1, b);
                r_star = util::vector_subtr(r, ro);
                v -= util::vector_dot(r_star, b.geom[i][j][k+1].S[faceId]) * b.geom[i][j][k+1].A[faceId];

                b.volume[i][j][k] = 0.333333333f * v;
                std::cout << b.volume[i][j][k] / ((b.x[1][0][0]-b.x[0][0][0]) * (b.y[0][1][0]-b.y[0][0][0]) * (b.z[0][0][1]-b.z[0][0][0])) << std::endl;
            }
        }
    }
}

std::vector<float> setup::get_face_midpoint_vector(int faceId, int i, int j, int k, Block& b) {
    // Returns a vector for the midpoint of the current face.

    std::vector<float> r(3); // Initialises to 0.
    std::vector<std::vector<int>> face_verts_ijk = setup::get_vertex_ijk_vectors(faceId, i, j, k);

    for (std::vector<int> vert : face_verts_ijk) {
        r[0] += b.x[vert[0]][vert[1]][vert[2]];
        r[1] += b.y[vert[0]][vert[1]][vert[2]];
        r[2] += b.z[vert[0]][vert[1]][vert[2]];
    }
    for (float& f : r) {f *= 0.25f;}
    return r;
}




