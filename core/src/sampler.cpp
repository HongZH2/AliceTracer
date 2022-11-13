//
// Created by zhanghong50 on 2022/11/11.
//

#include "core/include/sampler.h"
#include "core/include/random_variable.h"

namespace ALICE_TRACER{

    // sample the rectangle light
    float Sampler::sampleRectangleXZ(RectangleXZ *rect, HitRes &hit_res, Ray & out_ray) {
        AVec3 center = rect->center();
        AVec2 area = rect->area();
        // randomly pick a sample point on the rect light
        AVec2 offset = AVec2(ALICE_TRACER::random_val<float>(), ALICE_TRACER::random_val<float>());  // todo: low decrement
        offset = area * (offset - 0.5f);
        AVec3 on_light = center + AVec3(offset.x, 0.f, offset.y);
        AVec3 l_dir =  on_light - hit_res.point_;
        // get the random point on the light and check if it is hittable
        // compute the pdf_omega
        float dist2 = ADot(l_dir, l_dir);
        float pdf_area = 1.f/(area.x * area.y);
        // compute pdf_omega = pdf_area * dist^2 / cos<on_light, light_normal>
        float cos_alpha = AClamp(ADot(AVec3(0.f, -1.f, 0.f), -l_dir));
        if(cos_alpha < MIN_THRESHOLD){
            pdf_omega = MIN_THRESHOLD;
        }
        float pdf_omega = pdf_area * dist2 / cos_alpha;

        // generate the output ray
        out_ray.start_ = hit_res.point_;
        out_ray.dir_ = ANormalize(l_dir);
        out_ray.time_ = 0.f;
        out_ray.fm_t_ = hit_res.frame_time_;
        out_ray.color_ = AVec3(0.f);

        return pdf_omega;
    }

    float LightSampler::sampleLight(Scene * scene, HitRes & hit_res, Ray & out_ray) {
        // randomly pick a light // TODO
        if(scene->lights_.empty()) return FLT_MAX;
        int32_t light_id = ALICE_TRACER::random_int(0, (int32_t)scene->lights_.size()-1);
        return sampleLight(scene, light_id, hit_res, out_ray);
    }

    float LightSampler::sampleLight(ALICE_TRACER::Scene *scene, int32_t id, ALICE_TRACER::HitRes &hit_res,
                                    ALICE_TRACER::Ray &out_ray) {
        auto light = scene->lights_.at(id);
        int32_t light_t = (int32_t) light.index();
        switch (light_t) {
            case 0: {
                RectangleXZ *rect = std::get<RectangleXZ *>(light);
                float pdf = sampleRectangleXZ(rect, hit_res, out_ray);
                HitRes l_hit;
                scene->cluster_->CheckHittable(out_ray, l_hit);
                if(l_hit.is_hit_ && l_hit.uni_id_ == rect->id()){  // check if it is hit by the sample point
                    out_ray.color_ = l_hit.mtl_->emit();
                }
                return pdf * 1.f/(float )scene->lights_.size();
            }
            default:
                break;
        }
        return 1.f;
    }

}