//
// Created by Hong Zhang on 2022/10/30.
//

#ifndef ALICE_TRACER_HITTABLE_H
#define ALICE_TRACER_HITTABLE_H

#include "material.h"
#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
using namespace ALICE_UTILS;

namespace ALICE_TRACER{

    // The Base Class for Bounding Limits
    class BoundingLimit{
    public:
        BoundingLimit() = default;
        virtual ~BoundingLimit() = default;
    };

    class BoundingBox: public BoundingLimit{
    public:
        BoundingBox(AVec3 b_min, AVec3 b_max);
        ~BoundingBox() override = default;
    private:
        AVec3 b_min_, b_max_;
    };

    class BoundingSphere: public BoundingLimit{
    public:
        BoundingSphere() = default;
        ~BoundingSphere() override = default;
    private:
        AVec3 center_;
        float radius_;
    };

    // The Hittable Objects/Surfaces/Volume
    class Hittable{
    public:
        Hittable() = default;
        virtual ~Hittable() = default;

        inline BoundingLimit * getBoundLimit(){return bound_;}

    protected:
        BoundingLimit * bound_ = nullptr;
    };

    // Sphere
    class Sphere: public Hittable{
    public:
        Sphere(AVec3 center, float radius);
        ~Sphere() override = default;

        AVec3 center() const{return center_;}
        float radius() const{return radius_;}

        AVec3 getNormal(AVec3 & point);

    private:
        AVec3 center_;
        float radius_;
    };

    // Cube
    class Cube: public Hittable{
        Cube(AVec3 center, float length);
        ~Cube() override = default;
    private:
        AVec3 center_;
        float length_;
    };


}


#endif //ALICE_TRACER_HITTABLE_H
