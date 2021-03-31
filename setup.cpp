//
// Created by Alexander Rossiter on 23/03/2021.
//

#include <fstream>
#include <regex>
#include <iostream>
#include "setup.h"
#include "util.h"


Grid setup::read_grid_testcase(std::string& testcase) {
    std::ifstream file(testcase + ".grid");
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
    int nxbid;

    // Loop though lines in patch file.
    while(std::getline(file, str)) {
        std::vector<std::string> patch_str_vec = util::str_split(str, " ");

        // Get patch type.
        assert(patch_str_vec[0].size() == 1);
        char kind = patch_str_vec[0][0];

        // Get block patch belongs to.
        int bid = std::stoi(patch_str_vec[1]);

        // Get the id of this patch.
        int pid = std::stoi(patch_str_vec[2]);

        // Get the patch extents.
        Extent extent = {std::stoi(patch_str_vec[3]),
                  std::stoi(patch_str_vec[4]),
                  std::stoi(patch_str_vec[5]),
                  std::stoi(patch_str_vec[6]),
                  std::stoi(patch_str_vec[7]),
                  std::stoi(patch_str_vec[8])};

        switch (kind) {
            case 'P': {
                NextDir nextDir = {patch_str_vec[10], patch_str_vec[11], patch_str_vec[12]};
                int nxbid = std::stoi(patch_str_vec[8]);
                int nxpid = std::stoi(patch_str_vec[9]);
                g.get_patches().push_back(std::make_unique<PeriodicPatch>(bid, pid, extent, nxbid, nxpid, nextDir));
                break;
            }
            case 'I': {
                InletConds inletConds = {std::stof(patch_str_vec[8]), std::stof(patch_str_vec[9]), std::stof(patch_str_vec[10]),
                              std::stof(patch_str_vec[11])};
                g.get_patches().push_back(std::make_unique<InletPatch>(bid, pid, extent, inletConds));
                break;
            }
            case 'E': {
                g.get_patches().push_back(std::make_unique<ExitPatch>(bid, pid, extent, std::stof(patch_str_vec[8])));
                break;
            }
            default:
                throw std::invalid_argument("Invalid patch type.");
        }

        //std::cout << util::vector_to_string(patch_str_vec) << std::endl;
    }

}

Gas setup::read_gas(std::string& gasName) {
    std::ifstream file("../resources/gases/" + gasName + ".flwntgas");
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::vector<std::string> gasPropStr = util::str_split(buffer.str(), "\n");
    Gas gas;

    for (std::string& str : gasPropStr) {
        if (str.find("cp") != std::string::npos) {
            gas.cp = std::stof(util::str_split(str, " ")[1]);
        } else if (str.find("R") != std::string::npos) {
            gas.R = std::stof(util::str_split(str, " ")[1]);
        } else if (str.find("ga") != std::string::npos) {
            gas.ga = std::stof(util::str_split(str, " ")[1]);
        } else {
            throw std::invalid_argument("Invalid gas property.");
        }
    }

    return gas;

}

SolutionParameters setup::read_solution_params(std::string& directory) {
    std::ifstream file( directory + "solnparams.flwntparam");
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::vector<std::string> solnPropStr = util::str_split(buffer.str(), " ");
    SolutionParameters sf {std::stof(solnPropStr[0]),
                           std::stof(solnPropStr[1]),
                           std::stoi(solnPropStr[2])};

    return sf;
}






