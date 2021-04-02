//
// Created by Alexander Rossiter on 27/03/2021.
//

#ifndef FLOWENT_PATCH_H
#define FLOWENT_PATCH_H

#include <utility>
#include <vector>
#include <string>
#include "Block.h"

class Solver;

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
    int pid;
    int face;

    Extent extent{};
    virtual void apply(Solver& solver)=0;
    virtual std::string to_string()=0;
    virtual void alter_block_iteration_extent(Block& b)=0;
    void shift_patch_extent(int ishift, int jshift, int kshift);
    void find_face();

    Patch()=default;
    virtual ~Patch()=default;
    Patch(int bid, int pid, Extent extent) : bid{bid}, pid{pid}, extent{extent} {find_face();};
};

struct PeriodicPatch: Patch {
public:
    PeriodicPatch()=default;
    ~PeriodicPatch() override = default;
    PeriodicPatch(int bid, int pid, Extent extent, int nxbid, int nxpid, NextDir nextDir_) : Patch(bid, pid, extent), nxbid{nxbid}, nxpid{nxpid}, nextDir{std::move(nextDir_)}{};

    int nxbid;
    int nxpid;
    NextDir nextDir{};

    void apply(Solver& solver) override;
    void alter_block_iteration_extent(Block& b) override;
    std::string to_string() override;

};

struct InletPatch: Patch {
public:
    InletPatch()=default;
    ~InletPatch() override = default;
    InletPatch(int bid, int pid, Extent extent, InletConds conditions_, float rfin) : Patch(bid, pid, extent), conditions{conditions_}, rfin{rfin}{};

    InletConds conditions{};
    float rfin;
    float ro_nm1=0;

    void alter_block_iteration_extent(Block& b) override {};
    void apply(Solver& solver) override;
    std::string to_string() override;

};

struct ExitPatch: Patch {
public:
    ExitPatch()=default;
    ~ExitPatch() override = default;
    ExitPatch(int bid, int pid, Extent extent, float p_exit) : Patch(bid, pid, extent), p_exit{p_exit} {};

    float p_exit;

    void alter_block_iteration_extent(Block& b) override {};
    void apply(Solver& solver) override;
    std::string to_string() override;
};
#endif //FLOWENT_PATCH_H
