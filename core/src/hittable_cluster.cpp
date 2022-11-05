//
// Created by zhanghong50 on 2022/10/31.
//

#include "core/include/hittable_cluster.h"
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

    // -------------------------------
    // ClusterList
    // -------------------------------
    ClusterList::~ClusterList()  {
        releaseBVH(bvh_tree_);
    }

    void ClusterList::releaseBVH(Hittable *node) {
        if(!node) return;
        BVHNode * bvh_node = dynamic_cast<BVHNode *>(node);
        if(!bvh_node || (!bvh_node->left_ && !bvh_node->right_)){
            delete node;
            return;
        }
        releaseBVH(bvh_node->left_);
        releaseBVH(bvh_node->right_);
        delete bvh_node;
    }

    void ClusterList::buildBVH() {
        setUpBVH(bvh_tree_, hittable_array_);        // build BVH recursively
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

    bool ClusterList::CheckHittable(Ray &ray, HitRes &hit_res) {
        if(bvh_tree_)
            bvh_tree_->CheckHittable(ray, hit_res);
        return hit_res.is_hit_;
    }

    AABB *ClusterList::boundLimit(float frame_time) {
        if(bvh_tree_)
            return bvh_tree_->boundLimit(frame_time);
        return boundLimit(frame_time);
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

    // -------------------------------
    // Box(6-face cluster list)
    // -------------------------------
    Box::Box(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf){
        mtl_ = mtl;
        bxdf_ = bxdf;
        // create the box
        std::array<AVec3, 6> offset = {  // offset
                AVec3(0.f, 0.f, 0.5f),
                AVec3(0.f, 0.f, -0.5f),
                AVec3(0.5f, 0.f, 0.0f),
                AVec3(-0.5f, 0.f, 0.0f),
                AVec3(0.0f, 0.5f, 0.0f),
                AVec3(0.0f, -0.5f, 0.0f)
        };
        std::array<AVec4, 6> rotate = {  // rotate: axis and angle
                AVec4(0.f, 0.f, 1.0f, 0.f),
                AVec4(0.f, 0.f, 1.0f, 0.f),
                AVec4(0.f, 1.f, 0.0f, 90.f),
                AVec4(0.f, 1.f, 0.0f, -90.f),
                AVec4(1.f, 0.f, 0.0f, 90.f),
                AVec4(1.f, 0.f, 0.0f, -90.f)
        };
        for(int32_t i = 0; i < 6; ++i) {
            RectangleXY * rect = new RectangleXY(mtl, bxdf);
            float angle = rotate[i].w;
            rect->rotate(ARadians(angle), AVec3(rotate[i]));
            rect->translate(offset[i]);
            hittable_array_.addHittableInst(rect);
        }
        hittable_array_.buildBVH();
    }

    Box::Box(ALICE_TRACER::Material *mtl, ALICE_TRACER::BxDFBase *bxdf, ALICE_TRACER::Movement *movement):
            Box(mtl, bxdf){
        movement_ = movement;
    }

    AABB *Box::boundLimit(float frame_time) {
        return hittable_array_.boundLimit(frame_time);
    }

    bool Box::CheckHittable(ALICE_TRACER::Ray &ray, ALICE_TRACER::HitRes &hit_res) {
        return hittable_array_.CheckHittable(ray, hit_res);
    }
}