//
// Created by Alexander Rossiter on 27/03/2021.
//

#ifndef FLOWENT_PATCH_H
#define FLOWENT_PATCH_H

#include <utility>
#include <vector>
#include <string>
#include "Grid.h"


struct Extent {
    int ist;
    int ien;
    int jst;
    int jen;
    int kst;
    int ken;
    [[nodiscard]] std::string to_string() const {
        return "ist: " + std::to_string(ist)
            + ", ien: " + std::to_string(ien)
            + ", jst: " + std::to_string(jst)
            + ", jen: " + std::to_string(jen)
            + ", kst: " + std::to_string(kst)
            + ", ken: " + std::to_string(ken);
    }
};

struct NextDir {
    std::string nexti;
    std::string nextj;
    std::string nextk;

    [[nodiscard]] std::string to_string() const{
        return "next-I: " + nexti + ", next-J: " + nextj + ", next-K: " + nextk;
    }
};

struct InletConds {
    float Po;
    float To;
    float yaw;
    float pitch;
    [[nodiscard]] std::string to_string() const{
        return "Po: " + std::to_string(Po) + " To: "
        + std::to_string(To) + " Yaw: "
        + std::to_string(yaw) + " Pitch: "
        + std::to_string(pitch);
    }
};

struct Patch {
    int bid;
    Extent extent{};
    Patch(int bid, Extent extent) : bid{bid}, extent{extent} {};
    virtual void apply(Grid& g)=0;
    virtual std::string to_string()=0;
};

struct PeriodicPatch: Patch {
    int nxbid;
    NextDir nextDir{};

    PeriodicPatch(int bid, Extent extent, int nxbid, NextDir nextDir_) : Patch(bid, extent), nxbid{nxbid}, nextDir{std::move(nextDir_)}{};
    void apply(Grid& g) override;
    std::string to_string() override {
        std::string str(std::string("Periodic: bid: ") + std::to_string(bid) + std::string(", nxbid: ") + std::to_string(nxbid) + std::string(", ") + extent.to_string() + std::string(", ") + nextDir.to_string());
        return str;
    }

};

struct InletPatch: Patch {
    InletConds conditions{};
    InletPatch(int bid, Extent extent, InletConds conditions_) : Patch(bid, extent), conditions{conditions_}{};

    void apply(Grid& g) override;
    std::string to_string() override{
        std::string str(std::string("Inlet: bid: ") + std::to_string(bid) + std::string(" ") + extent.to_string() + std::string(" ") + conditions.to_string());
        return str;
    }

};

struct ExitPatch: Patch {
    float p_exit;
    ExitPatch(int bid, Extent extent, float p_exit) : Patch(bid, extent), p_exit{p_exit} {};

    void apply(Grid& g) override;
    std::string to_string() override{
        std::string str(std::string("Exit: bid: ") + std::to_string(bid) + std::string(" ") + extent.to_string() + std::string(" p_exit: ") + std::to_string(p_exit));
        return str;
    }
};
#endif //FLOWENT_PATCH_H
