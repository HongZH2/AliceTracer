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
        Scene();
        ~Scene();

        Color computePixel(AVec2i pixel, AVec2i resolution);  // start the Monte Carlo
        void addCamera(Camera & camera);
        void addHittable(Hittable * hittable);
        void removeHittable(Hittable * hittable);

        inline void setNumOfSamples(uint32_t num_of_samples){ num_of_samples_ = num_of_samples;}

    private:
        void traceRay(Ray & ray, uint32_t iteration);        // to trace any ray
        void doShading(HitRes & hit_res, Ray & in_ray, Ray & out_ray);        // do the shading math
        Ray generateSampleRay(HitRes & hit_res);

        uint32_t num_of_samples_ = 8;  // the number of the samples per pixel
        uint32_t max_num_recursion_ = 5; // the maximum number of the tracing recursion
        Camera * camera_;
        HittableCluster * cluster_;
    };
}

#endif //ALICE_TRACER_SCENE_H
