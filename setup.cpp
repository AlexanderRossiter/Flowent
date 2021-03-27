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
        std::cout << "Failed to open grid file." << std::endl;
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
    str = "test_cases/" + testcase;
    read_patches(g, str);

    return g;
}

void setup::read_patches(Grid& g, std::string& fname) {
    // Open the patch file.
    std::ifstream file("../" + fname + ".patch");
    if (file.fail()) {
        std::cout << "Failed to open patch file." << std::endl;
        throw std::exception();
    }

    // Initialise storage variables.
    std::string str;
    std::vector<std::string> patch_str_vec;
    char kind;
    std::vector<int> extent;
    std::vector<std::string> next_dir;
    int bid;

    // Loop though lines in patch file.
    while(std::getline(file, str)) {
        patch_str_vec = util::str_split(str, " ");
        // Get patch type.
        assert(patch_str_vec[0].size() == 1);
        kind = patch_str_vec[0][0];
        // Get block patch belongs to.
        bid = std::stoi(patch_str_vec[1]);
        // Get the patch extents.
        for (int i = 2; i <= 7; i++)
            extent.push_back(std::stoi(patch_str_vec[i]));

        switch (kind) {
            case 'P': {
                next_dir = std::vector<std::string>(patch_str_vec.begin() + 9, patch_str_vec.end());
                PeriodicPatch p = PeriodicPatch(bid, extent, std::stoi(patch_str_vec[8]), next_dir);
                g.get_block_by_id(bid).patches.push_back(p);
                break;
            }
            case 'I':
                break;
            case 'E':
                break;
        }

//        for (Block& b: g.get_blocks()) {
//            for (auto p : b.patches) {
//                std::cout << p.bid << " " << p.ist << " " << p.ien << p.nexti << std::endl;
//            }
//        }
        std::cout << util::vector_to_string(patch_str_vec) << std::endl;
    }

    for (Block& b : g.get_blocks()) {
        for (Patch& p)
    }


}






