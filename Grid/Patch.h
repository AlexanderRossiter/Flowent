//
// Created by Alexander Rossiter on 27/03/2021.
//

#ifndef FLOWENT_PATCH_H
#define FLOWENT_PATCH_H
#include <vector>
#include <string>
struct Patch {
    int bid;
    int ist;
    int ien;
    int jst;
    int jen;
    int kst;
    int ken;

    Patch(int bid, std::vector<int>& extent) : bid{bid}, ist{extent[0]}, ien{extent[1]}, jst{extent[2]}, jen{extent[3]}, kst{extent[4]}, ken{extent[5]} {};
};

struct PeriodicPatch: Patch {
    int nxbid;
    std::string nexti;
    std::string nextj;
    std::string nextk;

    PeriodicPatch(int bid, std::vector<int>& extent, int nxbid,
                  std::vector<std::string>& next_dir) : Patch(bid, extent),
                                                        nxbid{nxbid},
                                                        nexti{next_dir[0]},
                                                        nextj{next_dir[1]},
                                                        nextk{next_dir[2]} {};
};

struct InletPatch: Patch {
    float Po;
    float To;
    float yaw;
    float pitch;
    InletPatch(int bid, std::vector<int>& extent,
               std::vector<float>& conditions) : Patch(bid, extent),
                                                Po{conditions[0]},
                                                To{conditions[1]},
                                                yaw{conditions[2]},
                                                pitch{conditions[3]} {};
};

struct ExitPatch: Patch {
    float p_exit;
    ExitPatch(int bid, std::vector<int>& extent, float p_exit) : Patch(bid, extent), p_exit{p_exit} {};
};
#endif //FLOWENT_PATCH_H
