//
// Created by zhanghong50 on 2022/11/2.
//

#ifndef ALICE_TRACER_MOVEMENT_H
#define ALICE_TRACER_MOVEMENT_H

#include "utils/include/alice_math.h"
using namespace ALICE_UTILS;

namespace ALICE_TRACER{

    // Define some movement
    class Movement{
    public:
        Movement() = default;
        ~Movement() = default;
        float start_ = 0.f;
        float end_ = 0.f;

        virtual AVec3 movementFunc(AVec3 pos, float time) = 0;
    };

    /*
     * Linear movement
     */
    class LinearMovement: public Movement{
    public:
        LinearMovement() = default;
        ~LinearMovement() = default;
        AVec3 velocity_;
        virtual AVec3 movementFunc(AVec3 pos, float time) override;
    };
}

#endif //ALICE_TRACER_MOVEMENT_H
