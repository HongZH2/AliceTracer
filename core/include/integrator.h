//
// Created by zhanghong50 on 2022/11/10.
//

#ifndef ALICE_TRACER_INTEGRATOR_H
#define ALICE_TRACER_INTEGRATOR_H

#include "scene.h"
#include "random_variable.h"

namespace ALICE_TRACER{

    class RussianRoulette{
    public:
        RussianRoulette() = default;
        ~RussianRoulette() = default;

        static RussianRoulette & getInstace(){

        }

        static bool evaluate(Color & col){
            float random_val = ALICE_TRACER::random_val<float>();
            if(random_val < end_){

            }
        }
        float end_ = 0.5f; // the probability to end the path tracing
    };

    // ---------------
    // -- sampler: to sample lights and textures and so on
    // ---------------
    class Sampler{

    };

    // ---------------
    // Integrator for evaluating the Render Equation
    // ---------------
    class Integrator{
    public:
        Integrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration);
        ~Integrator() = default;

        virtual Color render(AVec2i pixel, AVec2i resolution, Scene * scene) = 0;
        inline void setNumOfSamples(uint32_t num_of_samples){ num_sampler_per_pixel_ = num_of_samples;}
    protected:
        uint32_t num_sampler_per_pixel_ = 1; // the number of the samples per pixel
        uint32_t max_num_iteration_ = 1;  // the maximum number of the tracing recursion
    };

    class UniformIntegrator: public Integrator{
    public:
        UniformIntegrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration);
        ~UniformIntegrator() = default;

        Color render(AVec2i pixel, AVec2i resolution, Scene * scene) override;
    protected:
        void traceRay(Scene * scene, Ray & in_ray, uint32_t iteration);        // to trace any ray
        void doShading(HitRes & hit_res, Ray & in_ray, Ray & out_ray);        // do the shading math
        Ray generateSampleRay(HitRes & hit_res);
    };
}

#endif //ALICE_TRACER_INTEGRATOR_H
