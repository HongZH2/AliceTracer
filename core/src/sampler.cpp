//
// Created by zhanghong50 on 2022/11/11.
//

#include "core/include/sampler.h"
#include "core/include/random_variable.h"

namespace ALICE_TRACER{

    // ---------------------------
    // sampler
    // ---------------------------
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
        float pdf_omega;
        if(cos_alpha < MIN_THRESHOLD){
            pdf_omega = 0.f;
        }
        else{
            pdf_omega = pdf_area * dist2 / cos_alpha;
        }

        // generate the output ray
        out_ray.start_ = hit_res.point_;
        out_ray.dir_ = ANormalize(l_dir);
        out_ray.time_ = 0.f;
        out_ray.fm_t_ = hit_res.frame_time_;
        out_ray.color_ = AVec3(0.f);
        out_ray.ray_t_ = ShadowRay;

        return pdf_omega;
    }

    float Sampler::samplePDFRectXZ(ALICE_TRACER::RectangleXZ *rect, ALICE_TRACER::HitRes &hit_res, ALICE_TRACER::Ray &ray) {
        // query the center and area of the rectangle
        AVec3 center = rect->center();
        AVec2 area = rect->area();
        // compute the distance between shading point and hit point
        float dist = ALength(ray.start_ - hit_res.point_);
        float cos_alpha = AClamp(ADot(AVec3(0.f, -1.f, 0.f), -ray.dir_));
        float pdf_area = 1.f/(area.x * area.y);
        float pdf_omega = 0.f;
        if(cos_alpha < MIN_THRESHOLD){
            pdf_omega = 0.f;
        }
        else{
            pdf_omega = pdf_area * dist * dist / cos_alpha;
        }
        return pdf_omega;
    }

    // ---------------------------
    // light sampler
    // ---------------------------
    float LightSampler::samplePDF(ALICE_TRACER::Scene *scene, int32_t id, ALICE_TRACER::Ray ray) {
        if(id >= scene->lights_.size() || id < 0)
            return 0.f;
        // check the visibility
        HitRes l_hit;
        scene->cluster_->CheckHittable(ray, l_hit);
        if(l_hit.is_hit_ && l_hit.uni_id_ == id){  // check if it is hit by the sample point
            auto light = scene->lights_.at(id);
            int32_t light_t = (int32_t) light.index();
            switch (light_t) {
                case 0: {
                    RectangleXZ *rect = std::get<RectangleXZ *>(light);
                    return Sampler::samplePDFRectXZ(rect, l_hit, ray);
                }
                default:
                    break;
            }
        }
        return 0.f;
    }

    bool LightSampler::checkVisibility(ALICE_TRACER::Scene *scene, ALICE_TRACER::Ray ray) {
        HitRes l_hit;
        scene->cluster_->CheckHittable(ray, l_hit);  // check the visibility
        for(int32_t id = 0 ; id < scene->lights_.size(); ++id){
            Hittable * light = scene->getLight(id);
            if(l_hit.is_hit_ && l_hit.uni_id_ == light->id())  // check if it is hit by the sample point
                return true;
        }
        return false;
    }

    bool LightSampler::checkSingleVisibility(ALICE_TRACER::Scene *scene, int32_t id, ALICE_TRACER::Ray ray) {
        Hittable * light = scene->getLight(id);
        HitRes l_hit;
        scene->cluster_->CheckHittable(ray, l_hit);  // check the visibility
        if(l_hit.is_hit_ && l_hit.uni_id_ == light->id())  // check if it is hit by the sample point
            return true;
        return false;
    }

    int32_t LightSampler::randomLight(ALICE_TRACER::Scene *scene) {
        // randomly pick a light
        if(scene->lights_.empty()) return INT32_MIN;
        return ALICE_TRACER::random_int(0, (int32_t)scene->lights_.size()-1);
    }

    float LightSampler::sampleRandomLight(Scene * scene, HitRes & hit_res, Ray & out_ray) {
        // randomly pick a light // TODO
        int32_t light_id = randomLight(scene);
        return sampleSingleLight(scene, light_id, hit_res, out_ray);
    }

    float LightSampler::sampleSingleLight(ALICE_TRACER::Scene *scene, int32_t id, ALICE_TRACER::HitRes &hit_res,
                                    ALICE_TRACER::Ray &out_ray) {
        if(id >= scene->lights_.size() || id < 0)
            return 0.f;
        auto light = scene->lights_.at(id);
        int32_t light_t = (int32_t) light.index();
        switch (light_t) {
            case 0: {
                RectangleXZ *rect = std::get<RectangleXZ *>(light);
                float pdf = sampleRectangleXZ(rect, hit_res, out_ray);
                if(pdf <= MIN_THRESHOLD) return 0.f;
                // check the visibility
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


    // ---------------------------
    // material sampler
    // sample the texture image
    // ---------------------------
    AVec3 TextureSampler::accessTexture2D(ALICE_TRACER::ImageBase *img, ALICE_UTILS::AVec2 uv) {
        auto t = img->type();
        AVec3 rgb;
        switch (t) {
            case ImageType::IMG_RGB_UByte:{
                auto img_rgb = dynamic_cast<ImageUByte *>(img);
                if(img_rgb){
                    uint32_t w0 = floor(img_rgb->w() * uv.x);
                    uint32_t h0 = floor(img_rgb->h() * uv.y);
                    uint32_t w1 = w0 + 1 >= img_rgb->w() ? w0 : w0 + 1;
                    uint32_t h1 = h0 + 1 >= img_rgb->h() ? h0 : h0 + 1;
                    AVec3 v1 = (*img_rgb)(h0, w0)/256.f;
                    AVec3 v2 = (*img_rgb)(h1, w0)/256.f;
                    AVec3 v3 = (*img_rgb)(h0, w1)/256.f;
                    AVec3 v4 = (*img_rgb)(h1, w1)/256.f;
                    rgb = bilinearInterpolate(v1, v2, v3, v4, uv);
                    rgb = ALinearSpace(rgb);
                }
                break;
            }
            default:
                break;
        }
        return rgb;
    }

    AVec3 TextureSampler::accessTexture3D(ALICE_TRACER::ImageBase *img, ALICE_UTILS::AVec3 dir) {
        if(!img) return AVec3(0.f);
        auto t = img->type();
        AVec3 rgb;
        AVec3 n_dir = ANormalize(dir);
        if(t == ImageType::IMG_RGB_Float){
            AVec3 up = AVec3(0.f, 1.f, 0.f);
            AVec3 forward = AVec3(0.f, 0.f, 1.f);
            AVec3 right = AVec3(1.f, 0.f, 0.f);
            AVec2 uv;
            float udotd = ADot(n_dir, up);
            float theta = acos(udotd);
            AVec3 dir_p = ANormalize(n_dir - udotd * up);
            float phi = acos(ADot(dir_p, right));
            if(ADot(dir_p, forward) > 0.f){
                uv.x = 1.f - phi * M_1_PI / 2.f;
            }
            else{
                uv.x = phi * M_1_PI / 2.f;
            }
            uv.y = theta * M_1_PI;

            // sample the 2d texture
            auto img_rgb = dynamic_cast<ImageFloat *>(img);
            uint32_t w0 = floor(img_rgb->w() * uv.x);
            uint32_t h0 = floor(img_rgb->h() * uv.y);
            uint32_t w1 = w0 + 1 >= img_rgb->w() ? w0 : w0 + 1;
            uint32_t h1 = h0 + 1 >= img_rgb->h() ? h0 : h0 + 1;
            AVec3 v1 = (*img_rgb)(h0, w0);
            AVec3 v2 = (*img_rgb)(h1, w0);
            AVec3 v3 = (*img_rgb)(h0, w1);
            AVec3 v4 = (*img_rgb)(h1, w1);
            rgb = bilinearInterpolate(v1, v2, v3, v4, uv);
        }
        return rgb;
    }

}