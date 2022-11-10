//
// Created by zhanghong50 on 2022/11/10.
//

#include "core/include/integrator.h"
#include "core/include/random_variable.h"
#include "core/include/post_process.h"
#include "utils/include/alice_log.h"

namespace ALICE_TRACER{
    Integrator::Integrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration):
            num_sampler_per_pixel_(num_sample_per_pixel), max_num_iteration_(max_num_iteration){

    }

    UniformIntegrator::UniformIntegrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration):
            Integrator(num_sample_per_pixel, max_num_iteration){

    }

    Color UniformIntegrator::render(ALICE_UTILS::AVec2i pixel, ALICE_UTILS::AVec2i resolution,
                                    ALICE_TRACER::Scene *scene) {
        auto camera = scene->camera_;
        if(!camera){
            AliceLog::submitDebugLog("Camera is essential for the scene rendering!\n");
            return Color{0.f};
        }
        // set the offset with one pixel
        ALICE_UTILS::AVec2 offset;
        Color pixel_col;

        // sample multiple times
        for(uint32_t i = 0; i < num_sampler_per_pixel_; ++i) {
            // randomly assign the offset [0.f - 1.f]
            offset = ALICE_UTILS::AVec2(ALICE_TRACER::random_val<float>(),
                                        ALICE_TRACER::random_val<float>());
            ALICE_TRACER::Ray cam_ray = camera->getSingleRay(pixel, resolution, offset);
            // compute the color
            // ... start path tracing
            traceRay(scene, cam_ray, max_num_iteration_);
            pixel_col += cam_ray.color_;
        }
        pixel_col /= (float)num_sampler_per_pixel_; // average the results

        // transfer linear space to gamma space
        pixel_col = toGammaSpace(pixel_col);

        return pixel_col;
    }

    void UniformIntegrator::traceRay(ALICE_TRACER::Scene *scene, ALICE_TRACER::Ray &in_ray, uint32_t iteration) {
        auto & cluster_list = scene->cluster_;
        auto & background_func = scene->background_func_;

        if(iteration > 0) {
            HitRes hit_res;
            if(cluster_list)
                cluster_list->CheckHittable(in_ray, hit_res);
            if (hit_res.is_hit_) {
                // if it is hit by some certain hittable instance
                // step 1. generate the out ray from the hitting point
                Ray out_ray = generateSampleRay(hit_res);
                // step 2. trace the new ray
                traceRay(scene, out_ray, iteration - 1);
                // step 3. shading/evaluate the BxDF
                doShading(hit_res, in_ray, out_ray);
                return;
            }
            // or not. we can do something else instead. For instance, sampling a skybox
            if(background_func){
                AVec3 col;
                background_func(in_ray.dir_, col);
                in_ray.color_ = col;
            }
            else{
                in_ray.color_ = AVec3(0.f);
            }
        }
    }

    Ray UniformIntegrator::generateSampleRay(HitRes &hit_res) {
        // query the material of the hittable instance, then generate a sample ray
        Ray out_ray;
        if(!hit_res.bxdf_) {
            AliceLog::submitDebugLog("BxDF is null!!\n");
            return out_ray;
        }
        AVec3 dir = hit_res.bxdf_->sampleBxDF(hit_res.point_, hit_res.normal_, hit_res.mtl_);
        if(AIsNan(dir)){
            AliceLog::submitDebugLog("sampler dir is null!!\n");
            return out_ray;
        }
        out_ray.start_ = hit_res.point_;
        out_ray.dir_ = dir;
        out_ray.time_ = 0.f;
        out_ray.fm_t_ = hit_res.frame_time_;
        out_ray.color_ = AVec3(0.f);

        return out_ray;
    }

    void UniformIntegrator::doShading(HitRes &hit_res, Ray &in_ray, Ray &out_ray) {
        // evaluate the reflection/transmission equation, or a general scattering equation by the material of the instance
        if(!hit_res.bxdf_ || !hit_res.mtl_) {
            AliceLog::submitDebugLog("the BxDF or material of hittable instance is undefined!!\n");
            return;
        }
        // evaluate BxDF(x, n, i, o, mtl). Here, I consider that BxDF is related to 5 parameters
        AVec3 bxdf = hit_res.bxdf_->evaluateBxDF(hit_res.point_, hit_res.normal_, in_ray.dir_, out_ray.dir_, hit_res.mtl_);
        float pdf = hit_res.bxdf_->samplePDF(out_ray.dir_, hit_res.normal_, hit_res.mtl_);
        if(AIsNan(bxdf)){
            AliceLog::submitDebugLog("BxDF evaluation is null!!\n");
            return;
        }
        in_ray.color_ = hit_res.mtl_->emit() + bxdf / pdf * out_ray.color_.ToVec3();
    }

}