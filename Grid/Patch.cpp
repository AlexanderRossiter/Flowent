//
// Created by Alexander Rossiter on 27/03/2021.
//

#include "Patch.h"

void PeriodicPatch::apply(Grid &g) {

}

void ExitPatch::apply(Grid &g) {

}

void InletPatch::apply(Grid &g) {

}

void Patch::shift_patch_extent(int ishift, int jshift, int kshift) {
    extent.ist += ishift;
    extent.jst += jshift;
    extent.kst += kshift;
    extent.ien += ishift;
    extent.jen += jshift;
    extent.ken += kshift;
}
