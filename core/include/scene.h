//
// Created by Hong Zhang on 2022/10/29.
//

#ifndef ALICE_TRACER_SCENE_H
#define ALICE_TRACER_SCENE_H

#include "utils/include/alice_common.h"
#include "accelerate_struct.h"
#include "camera.h"

namespace ALICE_TRACER{

    /*
     * Class Scene for managing the hittable instances
     */
    class Scene{
    public:
        Scene(uint32_t num_sample_per_pixel, uint32_t max_num_iteration);
        ~Scene();

        Color computePixel(AVec2i pixel, AVec2i resolution);  // start the Monte Carlo
        void addCamera(Camera & camera);
        void addHittable(Hittable * hittable);
        void removeHittable(Hittable * hittable);

        inline void setNumOfSamples(uint32_t num_of_samples){ num_of_samples_ = num_of_samples;}
        inline void setBgFunc(std::function<void(AVec3 &, AVec3 &)> func){ background_func_ = func;}  // set the background

    private:
        void traceRay(Ray & ray, uint32_t iteration);        // to trace any ray
        void doShading(HitRes & hit_res, Ray & in_ray, Ray & out_ray);        // do the shading math
        Ray generateSampleRay(HitRes & hit_res);

        uint32_t num_of_samples_ = 1;  // the number of the samples per pixel
        uint32_t max_num_iteration_ = 1; // the maximum number of the tracing recursion
        Camera * camera_;
        HittableCluster * cluster_;
        std::function<void(AVec3 &, AVec3 &)> background_func_ = nullptr;
    };
}

#endif //ALICE_TRACER_SCENE_H
