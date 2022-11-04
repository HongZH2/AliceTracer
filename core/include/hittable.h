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
    // Axis Aligned Bounding Box
    // ------------------------------------------------------------
    class AABB{
    public:
        AABB() = default;
        AABB(AVec3 b_min, AVec3 b_max);
        ~AABB() = default;

        bool isInside(AABB & bbox1) const;
        bool isHit(Ray & ray) const;
    public:
        static bool isOverlap(AABB &bbox1, AABB &bbox2);
        static AABB overlapAABB(AABB & bbox1, AABB & bbox2);
        static AABB unionAABB(AABB & bbox1, AABB & bbox2);
    public:
        AVec3 b_min_, b_max_;  // define the lower bound and upper bound
    };

    // ------------------------------------------------------------
    // The Hittable Objects/Surfaces/Volume
    // ------------------------------------------------------------
    class Hittable{
    public:
        Hittable() = default;
        Hittable(Material * mtl, BxDFBase * bxdf);
        Hittable(Material * mtl, BxDFBase * bxdf, Movement * movement);
        virtual ~Hittable() = default;
    public:
        inline void setID(uint32_t id) {id_ = id;}
        inline uint32_t ID() const{return id_;}
    public:
        virtual bool CheckHittable(Ray & ray, HitRes & hit_res) = 0;  // check if it is hit or not
        virtual AABB * boundLimit(float frame_time);
    protected:
        uint32_t id_;
        Material * mtl_ = nullptr;
        BxDFBase * bxdf_ = nullptr;
        AABB * bound_ = nullptr;
        Movement * movement_ = nullptr;
    };

    // Sphere
    class Sphere: public Hittable{
    public:
        Sphere(Material *mtl, BxDFBase *bxdf);
        Sphere(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~Sphere() override = default;

        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;
    public:
        void translate(AVec3 offset);
        void scale(AVec3 scale);
        void rotate(float angle, AVec3 axis) ;
    private:
        AVec3 center(float frame_time);
        AVec3 center_ = AVec3(0.f);
        float radius_ = 1.f;
    };

    // Rectangle
    class RectangleXY: public Hittable{
    public:
        RectangleXY(Material *mtl, BxDFBase *bxdf);
        RectangleXY(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~RectangleXY() override = default;

        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;
    public:
        void translate(AVec3 offset);
        void scale(AVec3 scale);
        void rotate(float angle, AVec3 axis);
    private:
        AVec3 cornel(float frame_time);
        AVec3 l_b_ = AVec3(-0.5f, -0.5f, 0.f); // left bottom
        AVec2 area_ = AVec2(1.f); // area
        AVec3 norm_ = AVec3(0.f, 0.f, 1.f); // normal
        AVec3 u_w_ = AVec3(1.f, 0.f, 0.f);  // u axis
        AVec3 v_h_ = AVec3(0.f, 1.f, 0.f); // v axis
    };


}


#endif //ALICE_TRACER_HITTABLE_H
