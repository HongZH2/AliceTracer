//
// Created by Hong Zhang on 2022/10/30.
//

#ifndef ALICE_TRACER_HITTABLE_H
#define ALICE_TRACER_HITTABLE_H


#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
#include "core/include/ray.h"
#include "core/include/movement.h"

using namespace ALICE_UTILS;
namespace ALICE_TRACER{

    // ------------------------------------------------------------
    // The Base Class for Bounding Limits, Like BBOX, BSphere
    // ------------------------------------------------------------
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

//    class BoundingSphere: public BoundingLimit{
//    public:
//        BoundingSphere() = default;
//        ~BoundingSphere() override = default;
//    private:
//        AVec3 center_;
//        float radius_;
//    };

    // ------------------------------------------------------------
    // The Hittable Objects/Surfaces/Volume
    // ------------------------------------------------------------
    class Hittable{
    public:
        Hittable(Material * mtl, BxDFBase * bxdf);
        Hittable(Material * mtl, BxDFBase * bxdf, Movement * movement);
        virtual ~Hittable() = default;

    public:
        inline BoundingLimit * boundLimit(){return bound_;}
        inline void setID(uint32_t id) {id_ = id;}
        inline uint32_t ID() const{return id_;}
        inline Material* mtl(){return mtl_;}
        inline BxDFBase* bxdf(){return bxdf_;}
    public:
        /*
         * Compute the normal vector by the point
         */
        virtual AVec3 getNormal(AVec3 & point) = 0;
        /*
         * Check if it is hit or not
         */
        virtual float CheckHittable(Ray & ray) = 0;
    protected:
        uint32_t id_;
        Material * mtl_ = nullptr;
        BxDFBase * bxdf_ = nullptr;
        BoundingLimit * bound_ = nullptr;
        Movement * movement_ = nullptr;
    };


    // Sphere
    class Sphere: public Hittable{
    public:
        Sphere(AVec3 center, float radius, Material *mtl, BxDFBase *bxdf);
        Sphere(AVec3 center, float radius, Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~Sphere() override = default;

    public:
        inline AVec3 center() const{return center_;}
        inline float radius() const{return radius_;}
    public:
        AVec3 center(float frame_time);
        AVec3 getNormal(AVec3 & point) override;
        float CheckHittable(Ray & ray) override;
    private:
        AVec3 center_;
        float radius_;
    };

    // Rectangle
    class Rectangle: public Hittable{
    public:
        Rectangle(AVec3 center, AVec2 size, AVec3 norm, Material *mtl, BxDFBase *bxdf);
        Rectangle(AVec3 center, AVec2 size, AVec3 norm, Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~Rectangle() override = default;
    public:
        inline AVec3 center() const{return center_;}
        inline AVec2 size() const{return size_;}
        inline AVec3 normal() const{return normal_;}
    public:
        AVec3 center(float frame_time);
        AVec3 getNormal(AVec3 & point) override;
        float CheckHittable(Ray & ray) override;
    private:
        AVec3 center_;
        AVec3 normal_;
        AVec2 size_;
    };


}


#endif //ALICE_TRACER_HITTABLE_H
