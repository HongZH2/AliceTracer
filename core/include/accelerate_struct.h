//
// Created by Hong Zhang on 2022/10/30.
//

#ifndef ALICE_TRACER_ACCELERATE_STRUCT_H
#define ALICE_TRACER_ACCELERATE_STRUCT_H

#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
#include "hittable.h"

namespace ALICE_TRACER{
    // A set of the hittable instances
    class HittableCluster{
    public:
        HittableCluster() = default;
        virtual ~HittableCluster() = default;
        virtual HitRes hitCheck(Ray & ray) = 0;
        virtual void addHittableInst(Hittable * hittable_inst) = 0;
        virtual void removeHittableInst(uint32_t id) = 0;

    protected:
        float t_min_ = 0.f;
        float t_max_ = MAXFLOAT;
    };

    // A naive hittable list
    class ClusterList: public HittableCluster{
    public:
        ClusterList() = default;
        ~ClusterList() override = default;

        HitRes hitCheck(Ray & ray) override;
        void addHittableInst(Hittable * hittable_inst)override;
        void removeHittableInst(uint32_t id)override;

    private:
        std::vector<Hittable *> hittable_array_;  // a list of the hittable instances
    };


}


#endif //ALICE_TRACER_ACCELERATE_STRUCT_H
