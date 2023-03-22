//
// Created by Alexander Rossiter on 24/03/2021.
//

#ifndef FLOWENT_CELL_H
#define FLOWENT_CELL_H

#include <vector>

class Cell
{
  public:
    std::vector<std::vector<double>> S;
    std::vector<double>              A;
    std::vector<double>              Ax;
    std::vector<double>              Ay;
    std::vector<double>              Az;

    Cell(std::vector<double> Si_, std::vector<double> Sj_, std::vector<double> Sk_);
    Cell() :
        A{ std::vector<double>(3) },
        Ax{ std::vector<double>(3) },
        Ay{ std::vector<double>(3) },
        Az{ std::vector<double>(3) } {};
};

#endif // FLOWENT_CELL_H
