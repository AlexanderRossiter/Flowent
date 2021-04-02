//
// Created by adr39 on 31/03/2021.
//

#ifndef FLOWENT_INITIALGUESS_H
#define FLOWENT_INITIALGUESS_H


#include "../Grid/Grid.h"
#include "Gas.h"

class InitialGuess {
public:
    InitialGuess()=default;
    ~InitialGuess()=default;

    InitialGuess(Gas& gas): gas{gas} {};
    virtual void generate_guess(Grid& g)=0;

protected:
    Gas gas;
};

// Sets a uniform Mach number through whole domain.
class BasicGuess: InitialGuess {
public:
    BasicGuess()=default;
    ~BasicGuess()=default;

    BasicGuess(Gas& gas, float Mach_, float po_, float to_, float yaw_, float pitch_): InitialGuess(gas), Mach{Mach_}, po{po_}, to{to_}, yaw{yaw_}, pitch{pitch_} {};

    void generate_guess(Grid& g) override;

private:
    float Mach;
    float po; // stag p
    float to; // stag T
    float yaw;
    float pitch;
};


#endif //FLOWENT_INITIALGUESS_H
