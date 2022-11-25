//
// Created by zhanghong50 on 2022/11/10.
//

#include "core/include/integrator.h"
#include "core/include/random_variable.h"
#include "core/include/post_process.h"
#include "utils/include/alice_log.h"

namespace ALICE_TRACER{
    Integrator::Integrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration, uint32_t min_num_iteration):
            num_sampler_per_pixel_(num_sample_per_pixel), max_num_iteration_(max_num_iteration), min_num_iteration_(min_num_iteration){

    }

    // ---------------
    // UniformIntegrator
    // ---------------
    UniformIntegrator::UniformIntegrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration, uint32_t min_num_iteration):
            Integrator(num_sample_per_pixel, max_num_iteration, min_num_iteration){

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
        // Tone Mapping
//        pixel_col = ACESFilm(pixel_col);
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
                // handle the exception
                if(!hit_res.bxdf_ || !hit_res.mtl_) {
                    AliceLog::submitDebugLog("the BxDF or material of hittable instance is undefined!!\n");
                    return;
                }
                // if it is hit by some certain hittable instance
                in_ray.color_ = hit_res.mtl_->emit();
                if((max_num_iteration_ - iteration) >= min_num_iteration_ && RussianRoulette::isEnd())  // return if iteration is greater than the minimum iteration
                    return;

                // step 1. generate the out ray from the hitting point
                Ray out_ray;
                float out_ray_pdf;
                hit_res.bxdf_->sampleBxDF(out_ray, out_ray_pdf, hit_res, in_ray);

                // step 2. trace the new ray
                traceRay(scene, out_ray, iteration - 1);

                // step 3. shading/evaluate the BxDF
                // evaluate the reflection/transmission equation, or a general scattering equation by the material of the instance
                // evaluate BxDF(x, n, i, o, mtl). Here, I consider that BxDF is related to 5 parameters
                AVec3 bxdf = hit_res.bxdf_->evaluateBxDF(hit_res, in_ray, out_ray);
//                if(AIsNan(bxdf)){
//                    assert("BxDF evaluation is null!!\n");
//                    return;
//                }

                // step 4. evaluate the reflection with RussianRoulette
                in_ray.color_ += bxdf / out_ray_pdf * out_ray.color_.ToVec3()/ RussianRoulette::prob();

            }
            else {
                // or not. we can do something else instead. For instance, sampling a skybox
                if (background_func) {
                    AVec3 col;
                    background_func(in_ray.dir_, col);
                    in_ray.color_ = col;
                } else {
                    in_ray.color_ = AVec3(0.f);
                }
            }
        }
    }

    // ---------------
    // NextEventEstimate
    // ---------------
    NEEIntegrator::NEEIntegrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration, uint32_t min_num_iteration):
            Integrator(num_sample_per_pixel, max_num_iteration, min_num_iteration){

    }

    Color NEEIntegrator::render(AVec2i pixel, AVec2i resolution, Scene *scene) {
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
        // Tone Mapping
//        pixel_col = ACESFilm(pixel_col);
        // transfer linear space to gamma space
        pixel_col = toGammaSpace(pixel_col);

        return pixel_col;
    }

    void NEEIntegrator::traceRay(Scene *scene, Ray &in_ray, uint32_t iteration) {
        auto & cluster_list = scene->cluster_;
        auto & background_func = scene->background_func_;

        if(iteration > 0) {
            HitRes hit_res;
            if(cluster_list)
                cluster_list->CheckHittable(in_ray, hit_res);
            if (hit_res.is_hit_) {
                // if it is hit by some certain hittable instance
                if((max_num_iteration_ - iteration) == 0)
                    in_ray.color_ = hit_res.mtl_->emit();
                if((max_num_iteration_ - iteration) >= min_num_iteration_ && RussianRoulette::isEnd())  // return if iteration is greater than the minimum iteration
                    return;
                // --------------------- direct --------------------------
                Ray direct_ray;
                Color direct;
                float pdf_light = LightSampler::sampleRandomLight(scene, hit_res, direct_ray);
                if(pdf_light > MIN_THRESHOLD) { //  pdf
                    AVec3 direct_bxdf = hit_res.bxdf_->evaluateBxDF(hit_res, in_ray, direct_ray);
                    direct = direct_bxdf / pdf_light * direct_ray.color_.ToVec3() / RussianRoulette::prob();
                }

                // --------------------- indirect --------------------------
                // step 1. generate the out ray from the hitting point
                Ray out_ray;
                float out_ray_pdf;
                hit_res.bxdf_->sampleBxDF(out_ray, out_ray_pdf, hit_res, in_ray);

                // step 2. trace the new ray
                traceRay(scene, out_ray, iteration - 1);
                // step 3. shading/evaluate the BxDF
                // evaluate the reflection/transmission equation, or a general scattering equation by the material of the instance
                if(!hit_res.bxdf_ || !hit_res.mtl_) {
                    AliceLog::submitDebugLog("the BxDF or material of hittable instance is undefined!!\n");
                    return;
                }
                // evaluate BxDF(x, n, i, o, mtl). Here, I consider that BxDF is related to 5 parameters
                Color indirect;
                AVec3 indirect_bxdf = hit_res.bxdf_->evaluateBxDF(hit_res, in_ray, out_ray);
                if(out_ray_pdf > MIN_THRESHOLD) {
                    if (AIsNan(indirect_bxdf)) {
                        AliceLog::submitDebugLog("BxDF evaluation is null!!\n");
                        return;
                    }
                    // step 4. evaluate the reflection with RussianRoulette
                    indirect = indirect_bxdf / out_ray_pdf * out_ray.color_.ToVec3() / RussianRoulette::prob();
                }

                // combine the indirect and indirect light
                in_ray.color_ += direct + indirect;
            }
            else {
                // or not. we can do something else instead. For instance, sampling a skybox
                if (background_func) {
                    AVec3 col;
                    background_func(in_ray.dir_, col);
                    in_ray.color_ = col;
                } else {
                    in_ray.color_ = AVec3(0.f);
                }
            }
        }
    }

    // ---------------
    // Multiple Importance Sampling
    // ---------------
    MISIntegrator::MISIntegrator(uint32_t num_sample_per_pixel, uint32_t max_num_iteration, uint32_t min_num_iteration):
            Integrator(num_sample_per_pixel, max_num_iteration, min_num_iteration){

    }

    float MISIntegrator::balanceHeuristic(float n1, float pdf1, float n2, float pdf2) {
        return n1 * pdf1 / (n1 * pdf1 + n2 * pdf2);
    }

    float MISIntegrator::powerHeuristic(float n1, float pdf1, float n2, float pdf2) {
        float f = n1 * pdf1, g = n2 * pdf2;
        return f * f / (f * f + g * g);
    }

    Color MISIntegrator::render(AVec2i pixel, AVec2i resolution, Scene *scene) {
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
        // Tone Mapping
        pixel_col = ACESFilm(pixel_col);
//        pixel_col = Reinhard(pixel_col);
        // transfer linear space to gamma space
        pixel_col = toGammaSpace(pixel_col);

        return pixel_col;
    }

    void MISIntegrator::traceRay(Scene *scene, Ray &in_ray, uint32_t iteration) {
        auto & cluster_list = scene->cluster_;
        auto & background_func = scene->background_func_;

        if(iteration > 0) {
            HitRes hit_res;
            if(cluster_list)
                cluster_list->CheckHittable(in_ray, hit_res);
            if (hit_res.is_hit_) {

                if((max_num_iteration_ - iteration) == 0)
                    in_ray.color_ = hit_res.mtl_->emit();
                // if it is hit by some certain hittable instance
                if ((max_num_iteration_ - iteration) >= min_num_iteration_ && RussianRoulette::isEnd())  // return if iteration is greater than the minimum iteration
                    return;

                // step 1. randomly pick one light source
                int32_t l_id = LightSampler::randomLight(scene);

                Color result;
                // -------------------- sample the light source -----------------------
                {
                    Ray light_ray;
                    float pdf_light = LightSampler::sampleSingleLight(scene, l_id, hit_res, light_ray); // sample the selected light
                    if (pdf_light > MIN_THRESHOLD) {
                        // Given the light sample ray, compute the bxdf pdf
                        float pdf_bxdf = hit_res.bxdf_->samplePDF(hit_res, in_ray, light_ray);
                        if(pdf_bxdf > MIN_THRESHOLD) {
                            // evaluate the light ray
                            AVec3 bxdf = hit_res.bxdf_->evaluateBxDF(hit_res, in_ray, light_ray);
                            // compute the weight by the multiple importance sampling
                            float weight = 1.f;
                            is_balance_heuristic_ ? weight = balanceHeuristic(1.f, pdf_light, 1.f, pdf_bxdf)
                                                  : weight = powerHeuristic(1.f, pdf_light, 1.f, pdf_bxdf);
                            result += bxdf * weight / pdf_light * light_ray.color_.ToVec3() / RussianRoulette::prob();
                        }
                    }
                }

                // -------------------- sample the bxdf -----------------------
                {
                    Ray bxdf_ray;
                    float bxdf_ray_pdf;
                    hit_res.bxdf_->sampleBxDF(bxdf_ray, bxdf_ray_pdf, hit_res, in_ray);
                    if (bxdf_ray_pdf > MIN_THRESHOLD) {
                        if (!isMatchMtlType(hit_res.mtl_->type(), MaterialType::Specular)) {
                            // track the ray
                            traceRay(scene, bxdf_ray, iteration - 1);
                            // evaluate the light ray
                            AVec3 bxdf = hit_res.bxdf_->evaluateBxDF(hit_res, in_ray, bxdf_ray);
                            // Given the light sample ray, compute the bxdf pdf
                            float pdf_light = LightSampler::samplePDF(scene, l_id, bxdf_ray);
                            // compute the weight by the multiple importance sampling
                            float weight;
                            is_balance_heuristic_ ? weight = balanceHeuristic(1.f, bxdf_ray_pdf, 1.f, pdf_light)
                                                  : weight = powerHeuristic(1.f, bxdf_ray_pdf, 1.f, pdf_light);
                            result += bxdf * weight / bxdf_ray_pdf * bxdf_ray.color_.ToVec3() / RussianRoulette::prob();
                        } else {
                            if(LightSampler::checkVisibility(scene, bxdf_ray))
                                bxdf_ray.color_ += scene->getLight(l_id)->mtl()->emit();
                            // track the ray
                            traceRay(scene, bxdf_ray, iteration - 1);
                            // evaluate the light ray
                            AVec3 bxdf = hit_res.bxdf_->evaluateBxDF(hit_res, in_ray, bxdf_ray);
                            result += bxdf * bxdf_ray.color_.ToVec3() / RussianRoulette::prob();
                        }
                    }
                }

                // sum up all the effects
                in_ray.color_  += result;
            }
            else {
                // or not. we can do something else instead. For instance, sampling a skybox
                if (background_func) {
                    AVec3 col;
                    background_func(in_ray.dir_, col);
                    in_ray.color_ = col;
                } else {
                    in_ray.color_ = AVec3(0.f);
                }
            }
        }
    }

}