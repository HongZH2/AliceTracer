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
        setUpBVH(bvh_tree_, hittable_array_);
    }

    void ClusterList::setUpBVH(Hittable* & node, std::vector<Hittable *> & hittable_list) {
        if(hittable_array_.size() <= 0)
            return;
        if(hittable_list.size() == 1){
            node = hittable_list[0];
            return;
        }
        std::vector<Hittable *> hittable_inst = hittable_list;
        // step 1. randomly select one axis
        int32_t axis = random_int(0, 2);
        // step 2. sort the list
        std::sort(hittable_inst.begin(), hittable_inst.end(), [axis](Hittable * t1, Hittable * t2){
           AABB* bbox1 = t1->boundLimit(0.f);  // check the initial bounding box
           AABB* bbox2 = t2->boundLimit(0.f);
           return bbox1->b_min_[axis] < bbox2->b_min_[axis];
        });
        // create a BVH node
        node = new BVHNode();
        BVHNode * bvh_node = (BVHNode *)node;
        uint32_t mid = (hittable_inst.size())/2;
        std::vector<Hittable *> left_list {hittable_inst.begin(), hittable_inst.begin() + mid};
        std::vector<Hittable *> right_list {hittable_inst.begin() + mid, hittable_inst.end()};

        setUpBVH(bvh_node->left_, left_list);
        setUpBVH(bvh_node->right_, right_list);

        bvh_node->split_axis_ = axis;
        bvh_node->num_instance_ = (int32_t)(hittable_inst.size());
        AABB* bbox = bvh_node->boundLimit(0.f);
        bbox->unionAABB(*bvh_node->left_->boundLimit(0.f), *bvh_node->right_->boundLimit(0.f));
    }

    HitRes ClusterList::hitCheck(Ray & ray) {
        HitRes hit_res;
        if(bvh_tree_)
            bvh_tree_->CheckHittable(ray, hit_res);
        return hit_res;
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