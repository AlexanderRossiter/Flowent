//
// Created by Alexander Rossiter on 27/03/2021.
//

#ifndef FLOWENT_PATCH_H
#define FLOWENT_PATCH_H

#include <utility>
#include <vector>
#include <string>
#include "Block.h"

class Grid;

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
public:
    int bid;
    int face;

    Extent extent{};
    virtual void apply(Grid& g)=0;
    virtual std::string to_string()=0;
    virtual void alter_block_iteration_extent(Block& b)=0;
    void shift_patch_extent(int ishift, int jshift, int kshift);
    void find_face();

    Patch()=default;
    virtual ~Patch()=default;
    Patch(int bid, Extent extent) : bid{bid}, extent{extent} {find_face();};
};

struct PeriodicPatch: Patch {
public:
    PeriodicPatch()=default;
    ~PeriodicPatch() override = default;
    PeriodicPatch(int bid, Extent extent, int nxbid, NextDir nextDir_) : Patch(bid, extent), nxbid{nxbid}, nextDir{std::move(nextDir_)}{};

    int nxbid;
    NextDir nextDir{};

    void apply(Grid& g) override;
    void alter_block_iteration_extent(Block& b) override;
    std::string to_string() override;

};

struct InletPatch: Patch {
public:
    InletPatch()=default;
    ~InletPatch() override = default;
    InletPatch(int bid, Extent extent, InletConds conditions_) : Patch(bid, extent), conditions{conditions_}{};

    InletConds conditions{};

    void alter_block_iteration_extent(Block& b) override {};
    void apply(Grid& g) override;
    std::string to_string() override;

};

struct ExitPatch: Patch {
public:
    ExitPatch()=default;
    ~ExitPatch() override = default;
    ExitPatch(int bid, Extent extent, float p_exit) : Patch(bid, extent), p_exit{p_exit} {};

    float p_exit;

    void alter_block_iteration_extent(Block& b) override {};
    void apply(Grid& g) override;
    std::string to_string() override;
};
#endif //FLOWENT_PATCH_H
