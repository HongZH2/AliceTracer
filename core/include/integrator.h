//
// Created by zhanghong50 on 2022/11/10.
//

#ifndef ALICE_TRACER_INTEGRATOR_H
#define ALICE_TRACER_INTEGRATOR_H

#include "sampler.h"
#include "random_variable.h"

namespace ALICE_TRACER{
    // ---------------
    // -- RussianRoulette
    // ---------------
    class RussianRoulette{
    public:
        static RussianRoulette & getInstance(){
            static RussianRoulette instance;
            return instance;
        }
        static bool isEnd(){
            auto & instance = getInstance();
            float random_val = ALICE_TRACER::random_val<float>();
            if(random_val < instance.end_){
                return false;
            }
            return true;
        }
        static float prob(){
            auto & instance = getInstance();
            return instance.end_;
        }

    private:
        RussianRoulette() = default;
        ~RussianRoulette() = default;
        RussianRoulette(const RussianRoulette &) = delete;
        RussianRoulette & operator=(const RussianRoulette &) = delete;
        float end_ = 0.8f; // the probability to end the path tracing
    };

    // ---------------
    // Integrator for evaluating the Render Equation
    // ---------------
    class Integrator{
    public:
        Integrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration, uint32_t min_num_iteration);
        ~Integrator() = default;

        virtual Color render(AVec2i pixel, AVec2i resolution, Scene * scene) = 0;
        inline void setNumOfSamples(uint32_t num_of_samples){ num_sampler_per_pixel_ = num_of_samples;}
    protected:
        uint32_t num_sampler_per_pixel_ = 1; // the number of the samples per pixel
        uint32_t max_num_iteration_ = 1;  // the maximum number of the tracing recursion
        uint32_t min_num_iteration_ = 1;  // the minimum number of the tracing recursion
    };

    // ---------------
    // UniformIntegrator
    // ---------------
    class UniformIntegrator: public Integrator{
    public:
        UniformIntegrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration, uint32_t min_num_iteration);
        ~UniformIntegrator() = default;

        Color render(AVec2i pixel, AVec2i resolution, Scene * scene) override;
    protected:
        void traceRay(Scene * scene, Ray & in_ray, uint32_t iteration);        // to trace any ray
    };

    // ---------------
    // NextEventEstimate
    // ---------------
    class NEEIntegrator: public Integrator{
    public:
        NEEIntegrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration, uint32_t min_num_iteration);
        ~NEEIntegrator() = default;

        Color render(AVec2i pixel, AVec2i resolution, Scene * scene) override;
    protected:
        void traceRay(Scene * scene, Ray & in_ray, uint32_t iteration);        // to trace any ray
    };

    // ---------------
    // Multiple Importance Sampling
    // ---------------
    class MISIntegrator: public Integrator{
    public:
        MISIntegrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration, uint32_t min_num_iteration);
        ~MISIntegrator() = default;
        Color render(AVec2i pixel, AVec2i resolution, Scene * scene) override;

        bool is_balance_heuristic_ = true;
    protected:
        void traceRay(Scene * scene, Ray & in_ray, uint32_t iteration);        // to trace any ray
        static float balanceHeuristic(float n1, float pdf1, float n2, float pdf2);
        static float powerHeuristic(float n1, float pdf1, float n2, float pdf2);
    };

}

#endif //ALICE_TRACER_INTEGRATOR_H
