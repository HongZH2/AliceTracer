//
// Created by Hong Zhang on 2022/10/30.
//

#ifndef ALICE_TRACER_ACCELERATE_STRUCT_H
#define ALICE_TRACER_ACCELERATE_STRUCT_H

#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"

namespace ALICE_TRACER{
    // A set of the hittable instances
    class HittableCluster{
    public:
        HittableCluster() = default;
        virtual ~HittableCluster() = default;

        virtual void hitCheck() = 0;
    };

    class ClusterList: public HittableCluster{
    public:
        ClusterList();
        ~ClusterList() override;

        void hitCheck() override;
    private:
//        std::vector<>
    };


}


#endif //ALICE_TRACER_ACCELERATE_STRUCT_H
