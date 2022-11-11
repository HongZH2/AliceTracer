//
// Created by zhanghong50 on 2022/11/11.
//

#ifndef ALICE_TRACER_SAMPLER_H
#define ALICE_TRACER_SAMPLER_H

#include "hittable.h"
#include "scene.h"

namespace ALICE_TRACER{

    // sampler
    class Sampler{
    public:
        Sampler() = default;
        ~Sampler() = default;
        static float sampleRectangleXZ(RectangleXZ *rect, HitRes &hit_res, Ray & out_ray);
    };

    class LightSampler:public Sampler{
    public:
        LightSampler() = default;
        ~LightSampler() = default;
        static float sampleLight(Scene * scene, HitRes &hit_res, Ray & out_ray);
    };

}


#endif //ALICE_TRACER_SAMPLER_H
