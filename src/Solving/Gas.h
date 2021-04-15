//
// Created by Alexander Rossiter on 23/03/2021.
//

#ifndef FLOWENT_GAS_H
#define FLOWENT_GAS_H


class Gas {
public:
    double cp;
    double R;
    double ga;
    double cv;

    Gas(double cp_, double R_, double ga_): cp{cp_}, R{R_}, ga{ga_} {cv = cp / ga;};
    Gas()=default;
    ~Gas()=default;
};


#endif //FLOWENT_GAS_H
