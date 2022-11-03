//
// Created by zhanghong50 on 2022/10/31.
//

#include "core/include/accelerate_struct.h"

namespace ALICE_TRACER{

    HitRes ClusterList::hitCheck(Ray & ray) {
        // brute force to find the closet intersection
        HitRes hit_res_;
        float time = MAXFLOAT;
        uint32_t id = INT32_MAX;
        Material * mtl = nullptr;
        BxDFBase * bxdf = nullptr;
        for(auto const & hittable : hittable_array_){
            float t = hittable->CheckHittable(ray);
            if(t > 0.f && time > t){
                time = t;
                id = hittable->ID();
                mtl = hittable->mtl();
                bxdf = hittable->bxdf();
            }
        }
        if(time < MAXFLOAT && time > t_min_ && time < t_max_){
            hit_res_.is_hit_ = true;
            hit_res_.point_ = ray.start_ + time * ray.dir_;
            hit_res_.hittable_id_ = id;
            hit_res_.mtl_ = mtl;
            hit_res_.bxdf_ = bxdf;
            hit_res_.frame_time_ = ray.fm_t_;
            AVec3 normal = hittable_array_[id]->getNormal(hit_res_.point_);
            hit_res_.setNormal(normal, ray.dir_);
        }
        return hit_res_;
    }

    void ClusterList::addHittableInst(Hittable *hittable_inst) {
        if(hittable_inst) {
            hittable_inst->setID((uint32_t)hittable_array_.size());
            hittable_array_.push_back(hittable_inst);
        }
    }

    void ClusterList::removeHittableInst(uint32_t id) {
        if(id < hittable_array_.size())
            hittable_array_.erase(hittable_array_.begin() + id);
    }

}