//
// Created by Alexander Rossiter on 24/03/2021.
//

#ifndef FLOWENT_CELL_H
#define FLOWENT_CELL_H


#include <vector>

class Cell {
public:
    std::vector<std::vector<float>> S;
    std::vector<float> A;
    std::vector<float> Ax;
    std::vector<float> Ay;
    std::vector<float> Az;


    Cell(std::vector<float> Si_, std::vector<float> Sj_, std::vector<float> Sk_);
    Cell(): A{std::vector<float>(3)}, Ax{std::vector<float>(3)}, Ay{std::vector<float>(3)}, Az{std::vector<float>(3)}{};
};


#endif //FLOWENT_CELL_H
