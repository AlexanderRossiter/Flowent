//
// Created by Alexander Rossiter on 23/03/2021.
//

#include "Grid.h"

void Grid::add_block(Block &b) {
    blocks.push_back(b);
    nb = blocks.size();
}
