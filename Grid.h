//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_GRID_H
#define FLOWENT_GRID_H


#include <vector>
#include "Block.h"

class Grid {

public:
    std::vector<Block> blocks;
    int nb;

    void add_block(Block& b);
};


#endif //FLOWENT_GRID_H
