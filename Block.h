//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_BLOCK_H
#define FLOWENT_BLOCK_H


#include <vector>

class Block {

public:
    std::vector<std::vector<std::vector<float>>> x;
    std::vector<std::vector<std::vector<float>>> y;
    std::vector<std::vector<std::vector<float>>> z;
    int ni;
    int nj;
    int nk;

    Block(std::vector<std::vector<std::vector<float>>> x_,
          std::vector<std::vector<std::vector<float>>> y_,
          std::vector<std::vector<std::vector<float>>> z_);


};


#endif //FLOWENT_BLOCK_H
