//
// Created by Hong Zhang on 2022/10/30.
//

#ifndef ALICE_TRACER_HITTABLE_H
#define ALICE_TRACER_HITTABLE_H


#include "utils/include/alice_common.h"
#include "utils/include/alice_math.h"
#include "core/include/ray.h"
#include "movement.h"

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
        /*
         * transform
         */
        virtual void translate(AVec3 offset);
        virtual void scale(AVec3 scale);
        virtual void rotate(float angle, AVec3 axis);
    protected:
        uint32_t id_;
        Material * mtl_ = nullptr;
        BxDFBase * bxdf_ = nullptr;
        BoundingLimit * bound_ = nullptr;
        Movement * movement_ = nullptr;
        AMat4 transform_mat_ = AMat4(1.f);  // transformation
        AMat4 rot_mat_ = AMat4(1.f);  // rotation
    };


    // Sphere
    class Sphere: public Hittable{
    public:
        Sphere(Material *mtl, BxDFBase *bxdf);
        Sphere(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~Sphere() override = default;

    public:
        inline AVec3 center() const{return center_;}
        inline float radius() const{return radius_;}
        AVec3 center(float frame_time);
        AVec3 getNormal(AVec3 & point) override;
        float CheckHittable(Ray & ray) override;
    private:
        AVec3 center_ = AVec3(0.f);
        float radius_ = 1.f;
    };

    // Rectangle
    class RectangleXY: public Hittable{
    public:
        RectangleXY(Material *mtl, BxDFBase *bxdf);
        RectangleXY(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~RectangleXY() override = default;
    public:
        inline AVec3 center() const{return center_;}
        inline AVec2 size() const{return size_;}
        inline AVec3 normal() const{return normal_;}

        AVec3 center(float frame_time);
        AVec3 getNormal(AVec3 & point) override;
        float CheckHittable(Ray & ray) override;
    private:
        AVec3 center_ = AVec3(0.f);
        AVec3 normal_ = AVec3(0.f, 0.f, 1.f);
        AVec2 size_ = AVec2(1.f, 1.f);
    };


}


#endif //ALICE_TRACER_HITTABLE_H
