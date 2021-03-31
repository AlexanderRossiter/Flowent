//
// Created by Alexander Rossiter on 27/03/2021.
//

#include <stdexcept>
#include "Patch.h"

void PeriodicPatch::apply(Grid &g) {

}

std::string PeriodicPatch::to_string() {
    std::string str(std::string("Periodic: bid: ") + std::to_string(bid) + std::string(", nxbid: ") + std::to_string(nxbid) + std::string(", ") + extent.to_string() + std::string(", ") + nextDir.to_string());
    return str;
}

void PeriodicPatch::alter_block_iteration_extent(Block &b) {
    switch (face) {
        case 0:
            b.ist = 0;
            break;
        case 1:
            b.jst = 0;
            break;
        case 2:
            b.kst = 0;
            break;
        case 3:
            b.ien += 1;
            break;
        case 4:
            b.jen += 1;
            break;
        case 5:
            b.ken += 1;
            break;
    }
}

void ExitPatch::apply(Grid &g) {

}

std::string ExitPatch::to_string() {
    std::string str(std::string("Exit: bid: ") + std::to_string(bid) + std::string(" ") + extent.to_string() + std::string(" p_exit: ") + std::to_string(p_exit));
    return str;
}

void InletPatch::apply(Grid &g) {

}

std::string InletPatch::to_string() {
    std::string str(std::string("Inlet: bid: ") + std::to_string(bid) + std::string(" ") + extent.to_string() + std::string(" ") + conditions.to_string());
    return str;
}

void Patch::shift_patch_extent(int ishift, int jshift, int kshift) {
    extent.ist += ishift;
    extent.jst += jshift;
    extent.kst += kshift;
    extent.ien += ishift;
    extent.jen += jshift;
    extent.ken += kshift;
}

void Patch::find_face() {
    if ((extent.ist == extent.ien-1) && (extent.jst != extent.jen-1) && (extent.kst != extent.ken-1)){
        if (extent.ist == 0) {
            face = 0;
        } else {
            face = 3;
        }
    } else if ((extent.ist != extent.ien-1) && (extent.jst == extent.jen-1) && (extent.kst != extent.ken-1)) {
        if (extent.ist == 0) {
            face = 1;
        } else {
            face = 4;
        }
    } else if ((extent.ist != extent.ien-1) && (extent.jst != extent.jen-1) && (extent.kst == extent.ken-1)) {
        if (extent.kst == 0) {
            face = 2;
        } else {
            face = 5;
        }
    } else {
        throw std::invalid_argument("Invalid patch extent");
    }
}
