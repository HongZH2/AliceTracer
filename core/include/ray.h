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
    struct Ray{
        AVec3 start_;
        AVec3 dir_;
        float time_;
        Color color_;
    };
}


#endif //ALICE_TRACER_RAY_H
