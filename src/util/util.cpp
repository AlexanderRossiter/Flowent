//
// Created by Alexander Rossiter on 25/03/2021.
//

#include "util.h"
#include "../Grid/cell_face_relation.h"
std::vector<std::string> util::str_split(const std::string& inputString, const std::string& delimiter) {
    // The vector that will contain the split string.
    std::vector<std::string> splitString;
    int startIndex = 0;
    int endIndex = 0;

    while ( (endIndex = inputString.find(delimiter, startIndex)) <
            inputString.size() ) {
        std::string subString = inputString.substr(startIndex, endIndex -
                                                               startIndex);
        splitString.push_back(subString);
        startIndex = endIndex + delimiter.size();
    }

    if (startIndex < inputString.size()) {
        std::string subString = inputString.substr(startIndex);
        splitString.push_back(subString);
    }

    return splitString;
}

float util::calculate_face_average(vector3d<float>& arr, int& faceId, int& i, int& j, int& k) {
    const std::vector<std::vector<int>> & idx = facegeom::face_verts[faceId]; // Switching this to a reference made it much faster :o
    return 0.25f * (arr[idx[0][0]+i][idx[0][1]+j][idx[0][2]+k]
                    + arr[idx[1][0]+i][idx[1][1]+j][idx[1][2]+k]
                    + arr[idx[2][0]+i][idx[2][1]+j][idx[2][2]+k]
                    + arr[idx[3][0]+i][idx[3][1]+j][idx[3][2]+k]);
}

bool util::at_least_two(bool a, bool b, bool c, bool d) {
    return (a && b) || (a && c) || (a && d) || (b && c) || (b && d) || (c && d);
}

void util::copy_grid_flow_variables(Block& b1, Block& b2, int i, int j, int k, int i2, int j2, int k2) {
    for (auto& entry : b1.primary_vars) {
        entry.second[i][j][k] = b2.primary_vars[entry.first][i2][j2][k2];
    }
    for (auto& entry : b1.secondary_vars) {
        entry.second[i][j][k] = b2.secondary_vars[entry.first][i2][j2][k2];
    }
    for (auto& entry : b1.c_fluxes) {
        entry.second[i][j][k] = b2.c_fluxes[entry.first][i2][j2][k2];
    }
}
