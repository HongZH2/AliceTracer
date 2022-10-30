//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_RAY_H
#define ALICE_TRACER_RAY_H

#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
#include "color.h"

using namespace ALICE_UTILS;

namespace ALICE_TRACER{

    // Data Struct For Ray
    struct Ray{
        AVec3 start_;
        AVec3 dir_;
        float time_;
        Color color_;
    };

    // Hit Response
    struct HitRes{
        bool is_hit_;
        AVec3 point_;
        AVec3 normal_;
    };
}


#endif //ALICE_TRACER_RAY_H
