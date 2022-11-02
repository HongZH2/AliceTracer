//
// Created by zhanghong50 on 2022/11/2.
//
#include "core/include/movement.h"

namespace ALICE_TRACER{
     AVec3 LinearMovement::movementFunc(AVec3 pos, float time) {
         float c_time = AClamp(time, start_, end_);
         return pos + (c_time - start_)/(end_ - start_) * velocity_;
    }
}