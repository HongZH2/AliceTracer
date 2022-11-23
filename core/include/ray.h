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
        float time_ = MAX_RAY_TIME;  // time refers to fly time. Literally, intersection = start + time * dir;
        Color color_;
        float fm_t_ = -1.f; // fm_t refers to frame time.
        float t_min_ = MIN_RAY_TIME;
        float t_max_ = MAX_RAY_TIME;
    };

    // Hit Response
    struct HitRes{
        bool is_hit_ = false;
        float frame_time_ = -1;
        uint32_t uni_id_ = INT_MAX;
        AVec3 point_;
        AVec3 normal_;
        bool is_inside_ = false;
        Material * mtl_ = nullptr;
        BxDFBase * bxdf_ = nullptr;

        void setNormal(AVec3 norm, AVec3 ray_dir){  // adjust the normal
            if(ADot(norm, ray_dir) < -MIN_THRESHOLD){
                normal_ = norm;
                is_inside_ = false;
            }
            else{
                normal_ = -norm;
                is_inside_ = true;
            }
        }
    };
}


#endif //ALICE_TRACER_RAY_H
