//
// Created by zhanghong50 on 2022/10/31.
//

#include "core/include/accelerate_struct.h"
#include "core/include/random_variable.h"

namespace ALICE_TRACER{
    // -------------------------------
    // BVH tree node
    // -------------------------------
    bool BVHNode::CheckHittable(Ray &ray, HitRes &hit_res) {
        AABB * bbox = boundLimit(ray.fm_t_);
        if(bbox->isHit(ray)){
            bool l_hit = left_->CheckHittable(ray, hit_res);
            bool r_hit = right_->CheckHittable(ray, hit_res);
            return l_hit || r_hit;
        }
        return false;
    }

    void ClusterList::buildBVH() {
        // build BVH recursively
        setUpBVH(bvh_tree_, hittable_array_, 0, hittable_array_.size());
    }

    void ClusterList::setUpBVH(Hittable *node, std::vector<Hittable *> hittable_list, uint32_t start, uint32_t end) {
        if(start >= end && start == 0)
            return;
        node = new BVHNode();
        auto hittable_inst = hittable_list;
        // step 1. randomly select one axis
        int32_t axis = random_int(0, 2);

        // step 2. sort the list
        if(start == end){
            node->hittable_id_ = hittable_inst[start]->ID();
        }
        else{
            std::sort(hittable_list);
            uint32_t mid = (uint32_t)((start + end)/2);
            setUpBVH(node->left_, ,start, mid);
        }

    }

    HitRes ClusterList::hitCheck(Ray & ray) {
        HitRes hit_res;
        float time = MAX_RAY_TIME;
        bool is_hit = root_->CheckHittable(ray, hit_res);
        if(is_hit){
            uint32_t id = hit_res_.hittable_id_;
            bool is_hit = hittable_array_[id]->CheckHittable(ray, hit_res);
            if(time > MIN_RAY_TIME && time < MAX_RAY_TIME){
                hit_res_.is_hit_ = true;
                hit_res_.point_ = ray.start_ + time * ray.dir_;
                hit_res_.mtl_ = hittable_array_[id]->mtl();
                hit_res_.bxdf_ = hittable_array_[id]->bxdf();
                hit_res_.frame_time_ = ray.fm_t_;
                AVec3 normal = hittable_array_[id]->getNormal(hit_res_.point_);
                hit_res_.setNormal(normal, ray.dir_);
            }
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