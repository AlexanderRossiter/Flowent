//
// Created by Alexander Rossiter on 27/03/2021.
//

#include "Patch.h"

void PeriodicPatch::apply(Grid &g) {

}

std::string PeriodicPatch::to_string() {
    std::string str(std::string("Periodic: bid: ") + std::to_string(bid) + std::string(", nxbid: ") + std::to_string(nxbid) + std::string(", ") + extent.to_string() + std::string(", ") + nextDir.to_string());
    return str;
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
