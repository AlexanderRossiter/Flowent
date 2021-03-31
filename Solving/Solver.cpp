//
// Created by Alexander Rossiter on 30/03/2021.
//

#include "Solver.h"

void Solver::apply_boundary_conditions() {
    // Apply the patch boundary conditions.
    for (auto& p : g.get_patches()) {
        p->apply(*this);
    }
}
