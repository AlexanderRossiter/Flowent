//
// Created by Alexander Rossiter on 27/03/2021.
//
#include "Grid.h"


void Grid::calculate_grid_geometries() {
    // Calculates the grid cell volumes and normal vectors for the
    // finite volume method.

    for (Block& b : get_blocks()) {
        // All cells except external faces.
        std::cout << "Calculating surface vectors... ";
        calculate_block_face_vectors(b);
        std::cout << "Done." << std::endl;
        std::cout << "Calculating volumes... ";
        calculate_block_volumes(b);
        std::cout << "Done." << std::endl;
    }
}

void Grid::calculate_block_face_vectors(Block& b) {
    std::vector<std::vector<float>> sijk(3, std::vector<float>(3));

    std::vector<std::vector<int>> vertex_ijk;
    for (int i = b.ist; i < b.ien-1; i++) {
        for (int j = b.jst; j < b.jen-1; j++) {
            for (int k = b.kst; k < b.ken-1; k++) {
                for (int faceId = 0; faceId < 3; faceId++) {
                    // Loop through the 3 faces that are stored in this cell.
                    // face_verts[faceId][vertex][index]
                    // faceId - 0:iface, 1:jface, 2:kface
                    vertex_ijk = get_vertex_ijk_vectors(faceId, i, j, k);
                    sijk[faceId] = calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                                                vertex_ijk[2], vertex_ijk[3], b);
                }
                b.geom[i][j][k] = Cell(sijk[0], sijk[1], sijk[2]);
                //std::cout << b.geom[i][j][k].A[0] << ", " << b.geom[i][j][k].A[1] << ", " << b.geom[i][j][k].A[2] << std::endl;
//                std::cout << "(" << b.geom[i][j][k].S[0][0] << ", " << b.geom[i][j][k].S[0][1] << ", " << b.geom[i][j][k].S[0][2] << "), ";
//                std::cout << "(" << b.geom[i][j][k].S[1][0] << ", " << b.geom[i][j][k].S[1][1] << ", " << b.geom[i][j][k].S[1][2] << "), ";
//                std::cout << "(" << b.geom[i][j][k].S[2][0] << ", " << b.geom[i][j][k].S[2][1] << ", " << b.geom[i][j][k].S[2][2] << ")" << std::endl;
            }
        }
    }

    // i=const end face
    int i = b.ien-1;
    int faceId = 0;
    for (int j = b.jst; j < b.jen-1; j++) {
        for (int k = b.kst; k < b.ken-1; k++) {
            vertex_ijk = get_vertex_ijk_vectors(faceId, i, j, k);
            sijk[faceId] = calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                                        vertex_ijk[2], vertex_ijk[3], b);
            sijk[1] = {0, 0, 0};
            sijk[2] = {0, 0, 0};
            b.geom[i][j][k] = Cell(sijk[0], sijk[1], sijk[2]);
        }
    }

    // j=const end face
    int j = b.jen-1;
    faceId = 1;
    for (int i= b.ist; i < b.ien-1; i++) {
        for (int k = b.kst; k < b.ken-1; k++) {

            vertex_ijk = get_vertex_ijk_vectors(faceId, i, j, k);
            sijk[faceId] = calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                                        vertex_ijk[2], vertex_ijk[3], b);
            sijk[0] = {0, 0, 0};
            sijk[2] = {0, 0, 0};
            b.geom[i][j][k] = Cell(sijk[0], sijk[1], sijk[2]);
        }
    }
    // k=const end face
    int k = b.ken-1;
    faceId = 2;
    for (int i = b.ist; i < b.ien-1; i++) {
        for (int j = b.jst; j < b.jen-1; j++) {
            vertex_ijk = get_vertex_ijk_vectors(faceId, i, j, k);
            sijk[faceId] = calculate_face_vector(vertex_ijk[0], vertex_ijk[1],
                                                        vertex_ijk[2], vertex_ijk[3], b);
            sijk[0] = {0, 0, 0};
            sijk[1] = {0, 0, 0};
            b.geom[i][j][k] = Cell(sijk[0], sijk[1], sijk[2]);
        }
    }
}

std::vector<float> Grid::calculate_face_vector(std::vector<int>& v1, std::vector<int>& v2,
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

std::vector<std::vector<int>> Grid::get_vertex_ijk_vectors(int faceId, int i, int j, int k) {
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

void Grid::calculate_block_volumes(Block& b) {
    float v;
    std::vector<float> r;
    std::vector<float> ro(3);
    std::vector<float> r_star(3);

    const std::vector<std::vector<int>> index_offsets = {{1,0,0},{0,1,0}, {0,0,1}};

    for (int i = b.ist; i < b.ien-1; i++) {
        for (int j = b.jst; j < b.jen-1; j++) {
            for (int k = b.kst; k < b.ken-1; k++) {
                v = 0.f;
                ro[0] = b.x[i][j][k];
                ro[1] = b.y[i][j][k];
                ro[2] = b.z[i][j][k];

                // Loop over the three faces stored with this cell.
                for (int faceId = 0; faceId < 3; faceId++) {
                    r = get_face_midpoint_vector(faceId, i, j, k, b);
                    r_star = util::vector_subtr(r, ro);
                    v += util::vector_dot(r_star, b.geom[i][j][k].S[faceId]) * b.geom[i][j][k].A[faceId];

                    r = get_face_midpoint_vector(faceId, i+index_offsets[faceId][0], j+index_offsets[faceId][1], k+index_offsets[faceId][2], b);
                    r_star = util::vector_subtr(r, ro);
                    v -= util::vector_dot(r_star, b.geom[i+index_offsets[faceId][0]][j+index_offsets[faceId][1]][k+index_offsets[faceId][2]].S[faceId]) * b.geom[i+index_offsets[faceId][0]][j+index_offsets[faceId][1]][k+index_offsets[faceId][2]].A[faceId];
                }
//                // Faces stored in adjacent cells. These vectors point in the wrong direction
//                // for this cell, so must subtract.
//                // i-face: stored in i+1th cell.
//                int faceId = 0;
//                r = get_face_midpoint_vector(faceId, i+1, j, k, b);
//                r_star = util::vector_subtr(r, ro);
//                v -= util::vector_dot(r_star, b.geom[i+1][j][k].S[faceId]) * b.geom[i+1][j][k].A[faceId];
//                // j-face: stored in j+1th cell.
//                faceId = 1;
//                r = get_face_midpoint_vector(faceId, i, j+1, k, b);
//                r_star = util::vector_subtr(r, ro);
//                v -= util::vector_dot(r_star, b.geom[i][j+1][k].S[faceId]) * b.geom[i][j+1][k].A[faceId];
//                // k-face: stored in k+1th cell.
//                faceId = 2;
//                r = get_face_midpoint_vector(faceId, i, j, k+1, b);
//                r_star = util::vector_subtr(r, ro);
//                v -= util::vector_dot(r_star, b.geom[i][j][k+1].S[faceId]) * b.geom[i][j][k+1].A[faceId];

                b.volume[i][j][k] = 0.333333333f * v;
                if (v < 0) {
                    std::cout << "NEGATIVE VOLUME FOUND AT (b, i, j, k) = " << b.id << " " << i << " " << j << " " << k << std::endl;
                }
                //std::cout << b.volume[i][j][k] / ((b.x[1][0][0]-b.x[0][0][0]) * (b.y[0][1][0]-b.y[0][0][0]) * (b.z[0][0][1]-b.z[0][0][0])) << std::endl;
            }
        }
    }
}

std::vector<float> Grid::get_face_midpoint_vector(int faceId, int i, int j, int k, Block& b) {
    // Returns a vector for the midpoint of the current face.

    std::vector<float> r(3); // Initialises to 0.
    std::vector<std::vector<int>> face_verts_ijk = get_vertex_ijk_vectors(faceId, i, j, k);

    for (std::vector<int> vert : face_verts_ijk) {
        r[0] += b.x[vert[0]][vert[1]][vert[2]];
        r[1] += b.y[vert[0]][vert[1]][vert[2]];
        r[2] += b.z[vert[0]][vert[1]][vert[2]];
    }
    for (float& f : r) {f *= 0.25f;}
    return r;
}


void Grid::move_patches_to_halo_grid() {
    for (auto& p : get_patches()) { // using auto to reduce typing of unique_ptr
        Block b = get_block_by_id(p->bid);
        p->shift_patch_extent(b.ist, b.jst, b.kst);
    }
}

void Grid::initialise_walls() {
    // Go through all patches, anywhere patch sits is not a wall.
    for (auto& p : patches) {
        Block& b = get_block_by_id(p->bid); // reference to b.
        for (int i = p->extent.ist; i < p->extent.ien; i++) {
            for (int j = p->extent.jst; j < p->extent.jen; j++) {
                for (int k = p->extent.kst; k < p->extent.ken; k++) {
                    b.isWall[i][j][k] = false;
                }
            }
        }
    }
}

void Grid::move_block_iteration_extent_for_periodic_patches() {
    for (auto& p : patches) {
        p->alter_block_iteration_extent(get_block_by_id(p->bid));
    }
}





