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
        void overlapAABB(AABB & bbox1, AABB & bbox2);
        void unionAABB(AABB & bbox1, AABB & bbox2);
    public:
        static bool isOverlap(AABB &bbox1, AABB &bbox2);
    public:
        AVec3 b_min_, b_max_;  // define the lower bound and upper bound
    };

    // ------------------------------------------------------------
    // The Hittable Objects/Surfaces/Volume
    // ------------------------------------------------------------
    class Hittable{
    public:
        Hittable();
        Hittable(Material * mtl, BxDFBase * bxdf);
        Hittable(Material * mtl, BxDFBase * bxdf, Movement * movement);
        virtual ~Hittable();
        virtual bool CheckHittable(Ray & ray, HitRes & hit_res) = 0;  // check if it is hit or not
        virtual AABB * boundLimit(float frame_time);
    public:
        inline uint32_t id(){return id_;}
        inline void setMaterial(Material* mtl){mtl_ = mtl;}
        inline void setBxdf(BxDFBase * bxdf){bxdf_ = bxdf;}
        inline Material * mtl(){return mtl_;}
        inline BxDFBase * bxdf(){return bxdf_;}
    protected:
        uint32_t id_; // unique id
        Material * mtl_ = nullptr;
        BxDFBase * bxdf_ = nullptr;
        AABB * bound_ = nullptr;
        Movement * movement_ = nullptr;
    };

    // ---------------
    // -- Sphere -----
    // ---------------
    class Sphere: public Hittable{
    public:
        Sphere(AVec3 center, float radius, Material *mtl, BxDFBase *bxdf);
        Sphere(Material *mtl, BxDFBase *bxdf);
        Sphere(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~Sphere() override = default;

        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;
    private:
        AVec3 center(float frame_time);
        AVec3 center_ = AVec3(0.f);
        float radius_ = 1.f;
    };

    // ---------------
    // -- RectangleXY --
    // ---------------
    class RectangleXY: public Hittable{
    public:
        RectangleXY(AVec3 center, AVec2 scale, Material *mtl, BxDFBase *bxdf);
        RectangleXY(Material *mtl, BxDFBase *bxdf);
        RectangleXY(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~RectangleXY() override = default;

        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;

        inline AVec3 center(){return center_;}
        inline AVec2 area(){return area_;}
    private:
        AVec3 center(float frame_time);
        AVec3 center_ = AVec3(-0.5f, -0.5f, 0.f); // left bottom
        AVec2 area_ = AVec2(1.f); // area
    };

    // ---------------
    // -- RectangleXZ --
    // ---------------
    class RectangleXZ: public Hittable{
    public:
        RectangleXZ(AVec3 center, AVec2 scale, Material *mtl, BxDFBase *bxdf);
        RectangleXZ(Material *mtl, BxDFBase *bxdf);
        RectangleXZ(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~RectangleXZ() override = default;

        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;

        inline AVec3 center(){return center_;}
        inline AVec2 area(){return area_;}
    private:
        AVec3 center(float frame_time);
        AVec3 center_ = AVec3(-0.5f, -0.5f, 0.f); // left bottom
        AVec2 area_ = AVec2(1.f); // area
    };

    // ---------------
    // -- RectangleXZ --
    // ---------------
    class RectangleYZ: public Hittable{
    public:
        RectangleYZ(AVec3 center, AVec2 scale, Material *mtl, BxDFBase *bxdf);
        RectangleYZ(Material *mtl, BxDFBase *bxdf);
        RectangleYZ(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~RectangleYZ() override = default;

        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;

        inline AVec3 center(){return center_;}
        inline AVec2 area(){return area_;}
    private:
        AVec3 center(float frame_time);
        AVec3 center_ = AVec3(-0.5f, -0.5f, 0.f); // left bottom
        AVec2 area_ = AVec2(1.f); // area
    };

    // ---------------
    // -- Cone --
    // ---------------
    class Cone: public Hittable{
    public:
        Cone(Material *mtl, BxDFBase *bxdf);
        Cone(Material *mtl, BxDFBase *bxdf, Movement * movement);
        ~Cone() override = default;

        bool CheckHittable(Ray & ray, HitRes & hit_res) override;
        AABB * boundLimit(float frame_time) override;
    public:
        float h_ = 1.f;
        float radius_ = 1.f;
        AVec3 center = AVec3(0.f);
    };

}


#endif //ALICE_TRACER_HITTABLE_H
