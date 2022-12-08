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

        // sample Rectangle XZ
        static float sampleRectangleXZ(RectangleXZ *rect, HitRes &hit_res, Ray & out_ray);
        static float samplePDFRectXZ(RectangleXZ *rect, HitRes &hit_res, Ray & ray);
        // sample Texture
        static float sampleEnv(EnvironmentalLight * env, HitRes &hit_res, Ray & out_ray);
        static float samplePDFEnv(EnvironmentalLight * env, HitRes &hit_res, Ray & ray);
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
    class TextureSampler{
    public:
        TextureSampler() = default;
        ~TextureSampler() = default;

        template <typename T>
        static T bilinearInterpolate(T v1, T v2, T v3, T v4, AVec2 uv){
            T v12 = v1 * (1.f - uv.x) + v2 * uv.x;
            T v34 = v1 * (1.f - uv.x) + v2 * uv.x;
            T val = v12 * (1.f - uv.y) + v34 * uv.y;
            return val;
        }

        static AVec3 accessTexture2D(ImageBase * img, AVec2 uv);
        static AVec3 accessTexture3D(ImageBase * img, AVec3 dir); // Given a light probe(hdr) and direction, sample the Light probe
    };

    // ---------------------------
    // material sampler
    // sample the texture image
    // ---------------------------
    class Discrete1DSampler{
    public:
        Discrete1DSampler(std::vector<double> & func);
        ~Discrete1DSampler() = default;
        float sampleFunc(float & v);
        float samplePDF(float & v);
    protected:
        int32_t binarySearch(double u, int32_t a, int32_t b);
        std::vector<double> cdf_;
        std::vector<double> pdf_;
    };
}


#endif //ALICE_TRACER_SAMPLER_H
