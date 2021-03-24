//
// Created by Alexander Rossiter on 24/03/2021.
//

#ifndef FLOWENT_CELL_H
#define FLOWENT_CELL_H


#include <vector>

class Cell {
public:
    float Ai;
    float Aj;
    float Ak;
    std::vector<float> Si;
    std::vector<float> Sj;
    std::vector<float> Sk;

    Cell(std::vector<float> Si_, std::vector<float> Sj_, std::vector<float> Sk_);
    Cell(): Ai{0}, Aj{0}, Ak{0}{};
};


#endif //FLOWENT_CELL_H
