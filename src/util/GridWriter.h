//
// Created by Alexander Rossiter on 30/03/2021.
//

#ifndef FLOWENT_GRIDWRITER_H
#define FLOWENT_GRIDWRITER_H

#include <string>
#include <iostream>
#include <fstream>
#include "../Grid/Grid.h"


class GridWriter {
private:
    Grid g;

public:
    GridWriter(Grid& g_): g{std::move(g_)}{};
    ~GridWriter()=default;

    void write_grid(std::string& fname);
    void write_solution(std::string& fname);
};


#endif //FLOWENT_GRIDWRITER_H
