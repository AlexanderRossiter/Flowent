//
// Created by adr39 on 31/03/2021.
//

#ifndef FLOWENT_INITIALGUESS_H
#define FLOWENT_INITIALGUESS_H

#include "Gas.h"
#include "Grid.h"

class InitialGuess
{
  public:
    InitialGuess()  = default;
    ~InitialGuess() = default;

    InitialGuess(Gas& gas) :
        gas{ gas } {};
    virtual void generate_guess(Grid& g) = 0;

  protected:
    Gas gas;
};

// Sets a uniform Mach number through whole domain.
class BasicGuess : InitialGuess
{
  public:
    BasicGuess()  = default;
    ~BasicGuess() = default;

    BasicGuess(Gas& gas, double Mach_, double po_, double to_, double yaw_, double pitch_) :
        InitialGuess(gas),
        Mach{ Mach_ },
        po{ po_ },
        to{ to_ },
        yaw{ yaw_ },
        pitch{ pitch_ } {};

    void generate_guess(Grid& g) override;

  private:
    double Mach;
    double po; // stag p
    double to; // stag T
    double yaw;
    double pitch;
};

class UpperLowerWalls : InitialGuess
{
  public:
    UpperLowerWalls()  = default;
    ~UpperLowerWalls() = default;

    UpperLowerWalls(Gas& gas, double po_, double to_, double yaw_, double pitch_, double pdown_) :
        InitialGuess(gas),
        pdown{ pdown_ },
        po{ po_ },
        to{ to_ },
        yaw{ yaw_ },
        pitch{ pitch_ } {};

    void generate_guess(Grid& g) override;

  private:
    double pdown;
    double po; // stag p
    double to; // stag T
    double yaw;
    double pitch;
};

#endif // FLOWENT_INITIALGUESS_H
