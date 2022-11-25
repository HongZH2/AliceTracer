//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_POST_PROCESS_H
#define ALICE_TRACER_POST_PROCESS_H

#include "utils/include/alice_math.h"
#include "color.h"

namespace ALICE_TRACER{
    // -----------------------------
    // -- Color Space Transition ---
    // -----------------------------
    // transfer the data from the linear space to the gamma space
    Color toGammaSpace(Color & col){
        return APow(col.ToVec3(), AVec3(1.f/2.2f));
    }

    // transfer the data from the gamma space to the linear space
    Color toLinearSpace(Color & col){
        return APow(col.ToVec3(), AVec3(2.2f));
    }

    // -----------------------------
    // -- Tone Mapping ---
    // -----------------------------
    Color ACESFilm(Color & col){
        AVec3 x = col.ToVec3();
        return AClamp((x * (x * 2.51f + 0.03f)) / (x * (2.43f * x + 0.59f) + 0.14f), AVec3(0.f), AVec3(1.f));
    }




}


#endif //ALICE_TRACER_POST_PROCESS_H
