//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_GAS_H
#define FLOWENT_GAS_H


class Gas {
public:
    float cp;
    float R;
    float ga;

    Gas(float cp_, float R_, float ga_): cp{cp_}, R{R_}, ga{ga_} {};
    Gas(){};
    ~Gas(){};
};


#endif //FLOWENT_GAS_H
