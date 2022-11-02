//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_RAY_H
#define ALICE_TRACER_RAY_H

#include "color.h"
#include "material.h"
#include "bxdf.h"

using namespace ALICE_UTILS;

namespace ALICE_TRACER{

    // Data Struct For Ray
    struct Ray{
        AVec3 start_;
        AVec3 dir_;
        float time_;  // time refers to fly time. Literally, intersection = start + time * dir;
        Color color_;
        float fm_t_ = -MAXFLOAT; // fm_t refers to frame time.
    };

    // Hit Response
    struct HitRes{
        bool is_hit_ = false;
        uint32_t hittable_id_;
        float frame_time_;
        AVec3 point_;
        AVec3 normal_;
        Material * mtl_;
        BxDFBase * bxdf_;

        void setNormal(AVec3 norm, AVec3 ray_dir){  // adjust the normal
            if(ADot(norm, ray_dir) < 0.f){
                normal_ = norm;
            }
            else{
                normal_ = -norm;
            }
        }
    };
}


#endif //ALICE_TRACER_RAY_H
