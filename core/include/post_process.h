//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_POST_PROCESS_H
#define ALICE_TRACER_POST_PROCESS_H

#include "utils/include/alice_math.h"

namespace ALICE_TRACER{
    // -----------------------------
    // -- Color Space Transition ---
    // -----------------------------
    // transfer the data from the linear space to the gamma space
    template<typename T>
    T toGammaSpace(T & val){
        return APow(val, AVec3(1.f/2.2f));
    }

    // transfer the data from the gamma space to the linear space
    template<typename T>
    T toLinearSpace(T & val){
        return APow(val, AVec3(2.2f));
    }

    // -----------------------------
    // -- Tone Mapping ---
    // -----------------------------





}


#endif //ALICE_TRACER_POST_PROCESS_H