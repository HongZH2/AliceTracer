//
// Created by zhanghong50 on 2022/11/1.
//

#include "core/include/scene.h"
#include "core/include/random_variable.h"
#include "core/include/tone_mapping.h"

namespace ALICE_TRACER{

    Scene::Scene() {
        cluster_ = new ClusterList();
    }

    Scene::~Scene() {
        delete cluster_;
    }

    void Scene::addCamera(Camera &camera) {
        camera_ = &camera;
    }

    void Scene::addHittable(Hittable * hittable) {
        cluster_->addHittableInst(hittable);
    }

    void Scene::removeHittable(Hittable * hittable) {
        cluster_->removeHittableInst(hittable->ID());
    }

    void Scene::traceRay(Ray & in_ray, uint32_t iteration) {
        if(iteration > 0) {
            HitRes hit_res = cluster_->hitCheck(in_ray);
            if (hit_res.is_hit_) {
                // if it is hit by some certain hittable instance
                // step 1. generate the out ray from the hitting point
                Ray out_ray = generateSampleRay(hit_res);
                // step 2. trace the new ray
                traceRay(out_ray, iteration - 1);
                // step 3. shading/evaluate the BxDF
                doShading(hit_res, in_ray, out_ray);
                return;
            }
        }
        // or not. we can do something else instead. For instance, sampling a skybox
        AVec3 unit_direction = in_ray.dir_;
        float t = 0.5*(unit_direction.y + 1.0);
        in_ray.color_.rgb_ = (1.0f - t) * AVec3(1.0, 1.0, 1.0) + t*AVec3(0.5, 0.7, 1.0);

    }

    Ray Scene::generateSampleRay(HitRes &hit_res) {
        // query the material of the hittable instance, then generate a sample ray
        Ray out_ray;
        if(!hit_res.bxdf_) assert("BxDF is null!!\n");
        AVec3 dir = hit_res.bxdf_->sampleBxDF(hit_res.point_, hit_res.normal_, hit_res.mtl_);

        out_ray.start_ = hit_res.point_;
        out_ray.dir_ = dir;

        return out_ray;
    }

    void Scene::doShading(HitRes &hit_res, Ray &in_ray, Ray &out_ray) {
        // evaluate the reflection/transmission equation, or a general scattering equation by the material of the instance
        if(!hit_res.bxdf_) assert("BxDF is null!!\n");
        AVec3 bxdf = hit_res.bxdf_->evaluateBxDF(hit_res.point_, hit_res.normal_, in_ray.dir_, out_ray.dir_, hit_res.mtl_);
        in_ray.color_.rgb_ = bxdf * out_ray.color_.rgb_;
    }

    Color Scene::computePixel(AVec2i pixel, AVec2i resolution) {
        if(!camera_){
            assert("Camera is essential for the scene rendering!\n");
            return Color{AVec3(0.f)};
        }
        // set the offset with one pixel
        ALICE_UTILS::AVec2 offset;
        Color pixel_col;
        // sample multiple times 
        for(uint32_t i = 0; i < num_of_samples_; ++i) {
            offset = ALICE_UTILS::AVec2(ALICE_TRACER::random_val<float>(),
                                        ALICE_TRACER::random_val<float>());
            ALICE_TRACER::Ray cam_ray = camera_->getSingleRay(pixel, resolution, offset);
            // compute the color
            // ... start path tracing
            traceRay(cam_ray, max_num_recursion_);
            pixel_col.rgb_ += cam_ray.color_.rgb_;
        }
        pixel_col.rgb_ /= (float)num_of_samples_;

        // transfer linear space to gamma space
        pixel_col.rgb_ = toGammaSpace(pixel_col.rgb_);

        return pixel_col;
    }
}