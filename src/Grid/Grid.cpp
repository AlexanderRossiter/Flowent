//
// Created by Alexander Rossiter on 23/03/2021.
//

#include "Grid.h"

void Grid::add_block(Block b) {
    blocks.push_back(b);
    nb = blocks.size();
    blocks.back().set_block_id(nb-1);
}

Block& Grid::get_block_by_id(int desired_bid) {
    for (Block& b : blocks) {
        if (b.id == desired_bid) {
            return b;
        }
    }
    throw std::exception();
}

std::unique_ptr<Patch>& Grid::get_patch_by_id(int pid) {
    for (auto& p : patches) {
        if (p->pid == pid) {
            return p;
        }
    }
    throw std::exception();
}

std::vector<Block>& Grid::get_blocks() {
    return blocks;
}

std::vector<std::unique_ptr<Patch>>& Grid::get_patches() {
    return patches;
}

void Grid::find_grid_min_volume() {
    minV = 1E5;
    for (Block& b : get_blocks())
        minV = b.minV < minV ? b.minV : minV;
}