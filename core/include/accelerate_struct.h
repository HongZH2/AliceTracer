//
// Created by Hong Zhang on 2022/10/30.
//

#ifndef ALICE_TRACER_ACCELERATE_STRUCT_H
#define ALICE_TRACER_ACCELERATE_STRUCT_H

#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
#include "hittable.h"

namespace ALICE_TRACER{
    // -------------------------------
    // BVH tree node
    // -------------------------------
    class BVHNode: public Hittable{
    public:
        BVHNode() = default;
        ~BVHNode() override = default;

        bool CheckHittable(Ray & ray, HitRes & hit_res);
    public:
        Hittable * left_ = nullptr;
        Hittable * right_ = nullptr;
    };

    // -------------------------------
    // A set of the hittable instances
    // -------------------------------
    class ClusterList{
    public:
        ClusterList() = default;
        ~ClusterList() = default;

        HitRes hitCheck(Ray & ray);
        void addHittableInst(Hittable * hittable_inst);
        void removeHittableInst(uint32_t id);
        void buildBVH();
    private:
        void setUpBVH(Hittable * node, std::vector<Hittable *> hittable_list, uint32_t start, uint32_t end);
        Hittable * bvh_tree_;
        std::vector<Hittable *> hittable_array_;  // a list of the hittable instances
    };


}


#endif //ALICE_TRACER_ACCELERATE_STRUCT_H
