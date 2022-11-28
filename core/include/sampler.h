//
// Created by zhanghong50 on 2022/11/11.
//

#ifndef ALICE_TRACER_SAMPLER_H
#define ALICE_TRACER_SAMPLER_H

#include "hittable.h"
#include "scene.h"

namespace ALICE_TRACER{

    // ---------------------------
    // sampler
    // ---------------------------
    class Sampler{
    public:
        Sampler() = default;
        ~Sampler() = default;
        static float sampleRectangleXZ(RectangleXZ *rect, HitRes &hit_res, Ray & out_ray);
        static float samplePDFRectXZ(RectangleXZ *rect, HitRes &hit_res, Ray & ray);
    };

    // ---------------------------
    // light sampler
    // ---------------------------
    class LightSampler:public Sampler{
    public:
        LightSampler() = default;
        ~LightSampler() = default;
        static bool checkVisibility(Scene * scene, Ray ray);
        static bool checkSingleVisibility(Scene * scene, int32_t id, Ray ray); // Check the visibility of the light
        static float samplePDF(Scene * scene, int32_t id, Ray ray);  // Given a sample ray, compute the pdf
        static int32_t randomLight(Scene * scene); // randomly pick a light, return the light id
        static float sampleRandomLight(Scene * scene, HitRes &hit_res, Ray & out_ray); // randomly pick a light, and compute the pdf and color
        static float sampleSingleLight(Scene * scene, int32_t id, HitRes &hit_res, Ray & out_ray); //  Given an id of the light, compute the pdf and color
    };

    // ---------------------------
    // material sampler
    // sample the texture image
    // ---------------------------
    class MaterialSampler{
    public:
        MaterialSampler() = default;
        ~MaterialSampler() = default;

        template <typename T>
        static T bilinearInterpolate(T v1, T v2, T v3, T v4, AVec2 uv){
            T v12 = v1 * (1.f - uv.x) + v2 * uv.x;
            T v34 = v1 * (1.f - uv.x) + v2 * uv.x;
            T val = v12 * (1.f - uv.y) + v34 * uv.y;
            return val;
        }

        static AVec3 sampleRGB(ImageBase * img, AVec2 uv);
    };
}


#endif //ALICE_TRACER_SAMPLER_H
